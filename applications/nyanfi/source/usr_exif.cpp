//----------------------------------------------------------------------//
// Exif ���̏���														//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <algorithm>
#include <mmsystem.h>
#include <time.h>
#include <System.StrUtils.hpp>
#include "usr_str.h"
#include "usr_file_ex.h"
#include "usr_exif.h"

//---------------------------------------------------------------------------
// �t�@�C���̌��݈ʒu�Ɏw�蕶���񂪂��邩�H
// ����Β���ʒu�ցA������Ό��݈ʒu�̂܂�
//---------------------------------------------------------------------------
bool EXIF_check_read(TFileStream *fs, const char *s)
{
	int size = strlen(s);
	if (size==0) return false;

	int p0 = fs->Seek(0, soFromCurrent);
	try {
		std::unique_ptr<BYTE[]> buf(new BYTE[size]);
		if (fs->Read(buf.get(), size)!=size) Abort();
		for (int i=0; i<size; i++) if (s[i]!=buf[i]) Abort();
		return true;
	}
	catch (EAbort &e) {
		fs->Seek(p0, soFromBeginning);
		return false;
	}
}

//---------------------------------------------------------------------------
unsigned short EXIF_get_data_2(TFileStream *fs, bool bigsw)
{
	unsigned short n = 0;
	unsigned char xbuf[4];
	int rn = fs->Read(xbuf, 2);
	if (rn==2) {
		if (bigsw)
			n = (unsigned short)((xbuf[0] << 8) | xbuf[1]);
		else
			n = (unsigned short)((xbuf[1] << 8) | xbuf[0]);
	}
	return n;
}

//---------------------------------------------------------------------------
unsigned int EXIF_get_data_4(TFileStream *fs, bool bigsw)
{
	unsigned int n = 0;
	unsigned char xbuf[8];
	int rn = fs->Read(xbuf, 4);
	if (rn==4) {
		if (bigsw)
			n = (unsigned int)((xbuf[0] << 24) | (xbuf[1] << 16) | (xbuf[2] << 8) | xbuf[3]);
		else
			n = (unsigned int)(xbuf[0] | (xbuf[1] << 8) | (xbuf[2] << 16) | (xbuf[3] << 24));
	}
	return n;
}

//---------------------------------------------------------------------------
float CIFF_ev(int v)
{
	float sig = (v<0)? -1.0 : 1.0;
	if (v<0) v = -v;

	float fr = v & 0x1f;
	v -= fr;
	if		(fr==0x0c) fr = 0x20/3.0;
	else if (fr==0x14) fr = 0x40/3.0;
	return sig * (v + fr)/0x20;
}

//---------------------------------------------------------------------------
void CIFF_parse(TFileStream *fs, TStringList *lst, int ofs, int length, bool bsw)
{
	UnicodeString itmbuf;

	fs->Seek(ofs + length - 4, soFromBeginning);
	int tb_ofs = EXIF_get_data_4(fs, bsw) + ofs;
	fs->Seek(tb_ofs, soFromBeginning);
	int recn = EXIF_get_data_2(fs, bsw);
	for (int i=0; i<recn; i++) {
		int tag 	= EXIF_get_data_2(fs, bsw);
		int len 	= EXIF_get_data_4(fs, bsw);
		int aoff	= ofs + EXIF_get_data_4(fs, bsw);
		bool in_dir = (tag & 0x4000);

		int p = fs->Seek(0, soFromCurrent);
		if (((tag >> 8)==0x28 || (tag >> 8)==0x30) && !in_dir) {
			//Subdirectory
			CIFF_parse(fs, lst, aoff, len, bsw);
		}
		else {
			int tagID = tag & 0x3fff;
			if (in_dir) {
				fs->Seek(tb_ofs + i*10 + 4, soFromBeginning);
				//FocalLength
				if (tagID==0x1029) {
					fs->Seek(2, soFromCurrent);
					lst->Add(itmbuf.sprintf(_T("37386=%umm"), EXIF_get_data_2(fs, bsw)));
				}
			}
			else {
				fs->Seek(aoff, soFromBeginning);
				//Camera Name
				if (tagID==0x080a) {
					std::unique_ptr<BYTE[]> buf(new BYTE[len + 2]);
					if (fs->Read(buf.get(), len)==len) {
						UnicodeString s;
						int cp = 0;
						while (cp<len && buf[cp]!='\0') s.cat_sprintf(_T("%c"), buf[cp++]);
						lst->Add("271=" + s);
						cp++; s = EmptyStr;
						while (cp<len && buf[cp]!='\0') s.cat_sprintf(_T("%c"), buf[cp++]);
						lst->Add("272=" + s);
					}
				}
				//ImageSpec
				else if (tagID==0x1810) {
					//�T�C�Y
					lst->Add(itmbuf.sprintf(_T("256=%u"), EXIF_get_data_4(fs, bsw)));
					lst->Add(itmbuf.sprintf(_T("257=%u"), EXIF_get_data_4(fs, bsw)));
					fs->Seek(4, soFromCurrent);
					//����
					int a = EXIF_get_data_4(fs, bsw);
					a = (a==90)? 6 : (a==180)? 3 : (a==270)? 8 : 1;
					lst->Add(itmbuf.sprintf(_T("274=%u"), a));
				}
				//CapturedTime
				else if (tagID==0x180e) {
					TDateTime dt = EXIF_get_data_4(fs, bsw)/(24.0 * 60 * 60);
					dt += (TDateTime(1970, 1, 1) - TDateTime(0));
					lst->Add("36867=" + FormatDateTime("yyyy/mm/dd hh:nn:ss", dt));
				}
				//ShotInfo
				else if (tagID==0x102a) {
					//index=1..
					fs->Seek(aoff + 1*2, soFromBeginning);
					int a_iso = EXIF_get_data_2(fs, bsw);
					int b_iso = EXIF_get_data_2(fs, bsw);
					a_iso = exp(a_iso/32.0*log(2.0))*100.0;
					b_iso = exp(b_iso/32.0*log(2.0))*100/32.0 + 0.5;
					lst->Add(itmbuf.sprintf(_T("AutoISO=%u"), a_iso));
					lst->Add(itmbuf.sprintf(_T("BaseISO=%u"), b_iso));
					//index=4..
					fs->Seek(aoff + 4*2, soFromBeginning);
					int f  = EXIF_get_data_2(fs, bsw);
					int s  = EXIF_get_data_2(fs, bsw);
					int e  = (short)EXIF_get_data_2(fs, bsw);
					int wb = EXIF_get_data_2(fs, bsw);
					// 4:F
					float ff = exp(CIFF_ev(f)*log(2.0)/2);
					lst->Add(itmbuf.sprintf(_T("33437=%.1f"), ff));
					// 5:ET
					float ss = exp(-CIFF_ev(s)*log(2.0));
					if (ss>0 && ss<0.25001) {
						int r = (int)(0.5 + 1/ss); if (r>50) r -= (r%10);
						lst->Add(itmbuf.sprintf(_T("33434=1/%u"), r));
					}
					else
						lst->Add(itmbuf.sprintf(_T("33434=%.1f"), ss));
					// 6:EV
					float ev = CIFF_ev(e);
					lst->Add(itmbuf.sprintf(_T("37380=%s%.1f"), (ev>0)? _T("+") : null_TCHAR, ev));
					// 7:WB
					lst->Add(itmbuf.sprintf(_T("CN:4.7=%u"), wb));
				}
				//CameraSettings
				else if (tagID==0x102d) {
					//Index=16 ISO
					fs->Seek(aoff + 16*2, soFromBeginning);
					int v = EXIF_get_data_2(fs, bsw);
					UnicodeString vstr;
					if (v!=0x7fff) {
						if (v & 0x4000)
						    vstr = v & 0x3fff;
						else {
							switch (v) {
							//case  0: vstr = "n/a";       break;
							//case 14: vstr = "Auto High"; break;
							//case 15: vstr = "Auto";      break;
							case 16: vstr.USET_T("50");  break;
							case 17: vstr.USET_T("100"); break;
							case 18: vstr.USET_T("200"); break;
							case 19: vstr.USET_T("400"); break;
							case 20: vstr.USET_T("800"); break;
							}
						}
					}
					if (!vstr.IsEmpty()) lst->Add("CameraISO=" + vstr);
					//Index=23 �����Y
					fs->Seek(aoff + 23*2, soFromBeginning);
					lst->Add(itmbuf.sprintf(_T("CN:1.23=%u"), EXIF_get_data_2(fs, bsw)));
					lst->Add(itmbuf.sprintf(_T("CN:1.24=%u"), EXIF_get_data_2(fs, bsw)));
					lst->Add(itmbuf.sprintf(_T("CN:1.25=%u"), EXIF_get_data_2(fs, bsw)));
				}
			}
		}
		fs->Seek(p, soFromBeginning);
	}
}

//---------------------------------------------------------------------------
// �w��t�@�C���� CIFF �����擾���� Exif �ꗗ�ɕϊ�
//---------------------------------------------------------------------------
bool CIFF_GetInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//���ʊi�[���X�g
{
	lst->Clear();
	if (!file_exists(fnam)) return false;

	bool ok;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		unsigned char xbuf[16];
		bool BigEndian;
		if (fs->Read(xbuf, 2)<2) Abort();
		if 		(xbuf[0]==0x4d && xbuf[1]==0x4d) BigEndian = true;
		else if (xbuf[0]==0x49 && xbuf[1]==0x49) BigEndian = false;
		else Abort();
		int hlen = EXIF_get_data_4(fs.get(), BigEndian);
		if (!EXIF_check_read(fs.get(), "HEAPCCDR")) Abort();

		int p = fs->Seek(0, soFromEnd);
		CIFF_parse(fs.get(), lst, hlen, p - hlen, BigEndian);

		//ISO
		int iso   = get_ListIntVal(lst, _T("CameraISO"));
		int a_iso = get_ListIntVal(lst, _T("AutoISO"));
		int b_iso = get_ListIntVal(lst, _T("BaseISO"));
		if (iso==0) iso = b_iso * a_iso / 100;
		if (iso==0) iso = b_iso;
		if (iso>0) lst->Add(UnicodeString().sprintf(_T("34855=%u"), iso));

		ok = true;
	}
	catch (EAbort &e) {
		lst->Clear();
		ok = false;
	}
	return ok;
}

//---------------------------------------------------------------------------
//IFD��������擾
//---------------------------------------------------------------------------
void EXIF_get_idf_inf(
	TFileStream *fs,
	int  top,
	bool bsw,			//BigEndian ���H
	TStringList *lst,	//���ʊi�[���X�g
	UnicodeString id)	//�ǉ�ID	(default=EmptyStr)
{
	unsigned char vbuf[8];
	UnicodeString tmp;

	unsigned short entn = EXIF_get_data_2(fs, bsw);
	for (unsigned short i=0; i<entn; i++) {
		unsigned short tag   = EXIF_get_data_2(fs, bsw);	//�^�O
		unsigned short dtype = EXIF_get_data_2(fs, bsw);	//�l�̃^�C�v
		int count = EXIF_get_data_4(fs, bsw);				//�l�̐�
		fs->Read(vbuf, 4);									//�l�܂��͒l�ւ̃I�t�Z�b�g

		unsigned short v_s0 = bsw ? (unsigned short)((vbuf[0] << 8)|vbuf[1])
								  : (unsigned short)((vbuf[1] << 8)|vbuf[0]);
		unsigned short v_s1 = bsw ? (unsigned short)((vbuf[2] << 8)|vbuf[3])
								  : (unsigned short)((vbuf[3] << 8)|vbuf[2]);

		int v_ui = bsw ? (unsigned int)((vbuf[0] << 24) | (vbuf[1] << 16) | (vbuf[2] << 8) | vbuf[3])
					   : (unsigned int)(vbuf[0] | (vbuf[1] << 8) | (vbuf[2] << 16) | (vbuf[3] << 24));
		int v_si = bsw ? (int)((vbuf[0] << 24) | (vbuf[1] << 16) | (vbuf[2] << 8) | vbuf[3])
					   : (int)(vbuf[0] | (vbuf[1] << 8) | (vbuf[2] << 16) | (vbuf[3] << 24));

		UnicodeString val_str;
		int p;

		if (tag==37500) {	//���[�J�[�m�[�g
			val_str = v_ui;
		}
		else {
			switch (dtype) {
			case  1:	//BYTE
				for (int i=0; i<count && i<4; i++) {
					if (i>0) val_str.UCAT_T(",");
					val_str.cat_sprintf(_T("%u"), vbuf[i]);
				}
				break;
			case  3:	//SHORT
				if (count>2) {
					p = fs->Seek(0, soFromCurrent);  fs->Seek((int)(top + v_ui), soFromBeginning);
					for (int i=0; i<count; i++) {
						if (i>0) val_str.UCAT_T(",");
						val_str.cat_sprintf(_T("%u"), EXIF_get_data_2(fs, bsw));
					}
					fs->Seek(p, soFromBeginning);
				}
				else {
					val_str = v_s0;
					if (count==2) val_str.cat_sprintf(_T(",%u"), v_s1);
				}
				break;
			case  4:	//LONG
				if (count>1) {
					p = fs->Seek(0, soFromCurrent);  fs->Seek((int)(top + v_ui), soFromBeginning);
					for (int i=0; i<count; i++) {
						if (i>0) val_str.UCAT_T(",");
						val_str.cat_sprintf(_T("%u"), EXIF_get_data_4(fs, bsw));
					}
					fs->Seek(p, soFromBeginning);
				}
				else
					val_str = v_ui;
				break;
			case  9:	//SLONG
				if (count>1) {
					p = fs->Seek(0, soFromCurrent);  fs->Seek((int)(top + v_ui), soFromBeginning);
					for (int i=0; i<count; i++) {
						if (i>0) val_str.UCAT_T(",");
						val_str.cat_sprintf(_T("%d"), (int)EXIF_get_data_4(fs, bsw));
					}
					fs->Seek(p, soFromBeginning);
				}
				else
					val_str = v_si;
				break;

			case  2:	//ASCII
				if (count<=4)
					val_str = (char*)vbuf;
				else {
					std::unique_ptr<char[]> sbuf(new char[count + 4]);
					p = fs->Seek(0, soFromCurrent);
					fs->Seek((int)(top + v_ui), soFromBeginning);
					fs->Read((void*)sbuf.get(), count);
					fs->Seek(p, soFromBeginning);
					val_str = sbuf.get();
				}
				if (tag==36867 && val_str.Length()==19) {	// �B�e����
					val_str[5] = '/';  val_str[8] = '/';
				}
				break;

			case  5:	//RATIONAL
				p = fs->Seek(0, soFromCurrent);  fs->Seek((int)(top + v_ui), soFromBeginning);
				for (int i=0; i<count; i++) {
					if (i>0) val_str.UCAT_T(",");
					unsigned int n0 = EXIF_get_data_4(fs, bsw);
					unsigned int n1 = EXIF_get_data_4(fs, bsw);
					if (USAME_TS(id, "GPS:"))	//GPS
						val_str.cat_sprintf(_T("%.8f"), (n0>0 && n1>0)? 1.0*n0/n1 : 0.0);
					else if (tag==33437)		//F
						val_str.cat_sprintf(_T("%.1f"), (n0>0 && n1>0)? 1.0*n0/n1 : 0.0);
					else if (tag==37386)		//�œ_����
						val_str.cat_sprintf(_T("%umm"), (n0>0 && n1>0)? (int)((1.0*n0/n1) + 0.5) : 0);
					else if (USAME_TS(id, "NK:") && tag==132) {	//�����Y
						if (n0>0 && n1>0) {
							if (i<2)
								val_str.cat_sprintf(_T("%u"), (n0>0 && n1>0)? (int)((1.0*n0/n1)+0.5) : 0);	//�œ_����
							else
								val_str.cat_sprintf(_T("%.1f"), (n0>0 && n1>0)? 1.0*n0/n1 : 0.0);			//F
						}
						if (i==3) {
							TStringDynArray ibuf = get_csv_array(val_str, 4, true);
							val_str = (ibuf[0]==ibuf[1])? ibuf[0] : tmp.sprintf(_T("%s-%s"), ibuf[0].c_str(), ibuf[1].c_str());
							val_str.UCAT_T("mm F/");
							val_str += (ibuf[2]==ibuf[3])? ibuf[2] : tmp.sprintf(_T("%s-%s"), ibuf[2].c_str(), ibuf[3].c_str());
						}
					}
					else {
						if (n0>0 && n1>0) {
							for (int j=0; j<4; j++) {
								unsigned int nn;
								switch (j) {
								case 0: nn = 10; break;
								case 1: nn =  2; break;
								case 2: nn =  3; break;
								case 3: nn =  5; break;
								}
								while (n0>=nn && n1>=nn) {
									if (n0%nn!=0 || n1%nn!=0) break;
									n0/=nn; n1/=nn;
								}
							}
							if (n0>=n1 && n0%n1==0)
								val_str.cat_sprintf(_T("%u"), n0/n1);
							else {
								float r = 1.0*n0/n1;
								if (r>0 && r<0.25001)
									val_str.cat_sprintf(_T("1/%u"), (unsigned int)(0.5 + 1/r));
								else
									val_str.cat_sprintf(_T("%u/%u"), n0, n1);
							}
						}
						else val_str.UCAT_T("0");
					}
				}
				fs->Seek(p, soFromBeginning);
				break;

			case 10:	//SRATIONAL
				p = fs->Seek(0, soFromCurrent);  fs->Seek((int)(top + v_ui), soFromBeginning);
				for (int i=0; i<count; i++) {
					if (i>0) val_str.UCAT_T(",");

					int n0 = (int)EXIF_get_data_4(fs, bsw);
					int n1 = (int)EXIF_get_data_4(fs, bsw);

					for (int j=0; j<4; j++) {
						int nn;
						switch (j) {
						case 0: nn = 10; break;
						case 1: nn =  2; break;
						case 2: nn =  3; break;
						case 3: nn =  5; break;
						}
						while (n0>=nn && n1>=nn) {
							if (n0%nn!=0 || n1%nn!=0) break;
							n0/=nn; n1/=nn;
						}
					}

					if (tag==37380) {	//�I���␳
						if (n0==0 || n1==0)
							val_str.UCAT_T("0.0");
						else
							val_str.cat_sprintf(_T("%+.1f"), 1.0*n0/n1);
					}
					else if (n0==0 || n1==0)
						val_str.UCAT_T("0");
					else if (abs(n0)>=abs(n1) && n0%n1==0)
						val_str.cat_sprintf(_T("%d"), n0/n1);
					else {
						float r = 1.0*n0/n1;
						if (r>0 && r<0.25001)
							val_str.cat_sprintf(_T("1/%d"), (int)(0.5 + 1/r));
						else
							val_str.cat_sprintf(_T("%d/%d"), n0, n1);
					}
				}
				fs->Seek(p, soFromBeginning);
				break;

			case 7:	//UNDEFINED
				switch (tag) {
				case 36864:	//Exif Version
				case 40960:	//FlashPixVersion
					vbuf[4] = '\0';
					val_str = (char*)vbuf;
					break;
				case 37121:	//ComponentsConfiguration
					val_str.sprintf(_T("%u,%u,%u,%u"), vbuf[0], vbuf[1], vbuf[2], vbuf[3]);
					break;
				default:
					val_str = v_ui;
				}
				break;
			}
		}

		val_str = val_str.Trim();
		if (!val_str.IsEmpty()) lst->Add(tmp.sprintf(_T("%s%u=%s"), id.c_str(), tag, val_str.c_str()));
	}
}

//---------------------------------------------------------------------------
//���ړ��e�̏����𐮂���
//---------------------------------------------------------------------------
void EXIF_format_inf(UnicodeString fext, TStringList *lst)
{
	UnicodeString vnam, vstr, tmp;

	vnam.USET_T("274");
	int idx  = lst->IndexOfName(vnam);
	if (idx!=-1) {
		vstr = lst->Values[vnam];
		if (!vstr.IsEmpty()) {
			vstr = get_word_i_idx(_T("?|����E��(1)|?|?|?|?|����E�c(6)|?|�����E�c(8)"), vstr.ToIntDef(0));
			lst->Add(tmp.sprintf(_T("%sL=%s"), vnam.c_str(), vstr.c_str()));
		}
	}

	//ISO
	vnam.USET_T("34855");
	vstr = EmptyStr;
	idx = lst->IndexOfName(vnam);
	if (test_FileExt(fext, _T(".rw2"))) vstr = lst->Values["23"];
	if (vstr.IsEmpty()) vstr = lst->Values[vnam];
	if (vstr.IsEmpty() || USAME_TS(vstr, "0")) vstr = get_tkn_r(lst->Values["NK:2"], ',');
	if (!vstr.IsEmpty()) {
		if (idx!=-1) lst->ValueFromIndex[idx] = vstr;
		else		 lst->Add(tmp.sprintf(_T("%s=%s"), vnam.c_str(), vstr.c_str()));
	}

	//�I�o����
	vnam.USET_T("33434");
	vstr = lst->Values[vnam];
	if (!vstr.IsEmpty()) lst->Add(tmp.sprintf(_T("%sU=%s�b"), vnam.c_str(), vstr.c_str()));

	//�I�o�v���O����
	vnam.USET_T("34850");
	idx = lst->IndexOfName(vnam);
	if (idx!=-1) {
		vstr = lst->Values[vnam];
		if (!vstr.IsEmpty()) {
			int n = vstr.ToIntDef(0);
			vstr = get_word_i_idx(
				_T("�s��|�}�j���A��|�m�[�}��|�i��D��|�V���b�^�[�D��|Creative|Action|�|�[�g���[�g|�����h�X�P�[�v"), n);
			if (vstr.IsEmpty()) vstr = "�s��";
			lst->Add(tmp.sprintf(_T("%sL=%s"), vnam.c_str(), vstr.c_str()));
		}
	}

	//��������
	vnam.USET_T("37383");
	idx  = lst->IndexOfName(vnam);
	if (idx!=-1) {
		vstr = lst->ValueFromIndex[idx];
		if (!vstr.IsEmpty()) {
			int n = vstr.ToIntDef(0);
			if (n==255)
				vstr.USET_T("���̑�");
			else {
				vstr = get_word_i_idx(_T("�s��|����|�����d��|�X�|�b�g|�}���`�X�|�b�g|��������|��������"), n);
				if (vstr.IsEmpty()) vstr = "�s��";
			}
			lst->ValueFromIndex[idx] = vstr;
		}
	}

	//�t���b�V��
	vnam.USET_T("37385");
	idx  = lst->IndexOfName(vnam);
	if (idx!=-1) {
		vstr = lst->ValueFromIndex[idx];
		if (!vstr.IsEmpty()) {
			vstr.sprintf(_T("%s"), ((vstr.ToIntDef(0) & 1)==1)? _T("ON") : _T("OFF"));
			lst->ValueFromIndex[idx] = vstr;
		}
	}

	//WB
	vnam.USET_T("CN:4.7");
	idx = lst->IndexOfName(vnam);
	if (idx!=-1) {
		vstr = lst->ValueFromIndex[idx];
		if (!vstr.IsEmpty()) {
			vstr = get_word_i_idx(
				_T("Auto|Sunny|Cloudy|Tungsten|Fluorescent|Flash|Custom|Black & White|Shade|Manual"), vstr.ToIntDef(-1));
			lst->ValueFromIndex[idx] = vstr;
		}
	}

	//LensModel
	vstr = lst->Values["42036"];
	if (vstr.IsEmpty()) {
		//Nikon
		vstr = lst->Values["NK:132"];
		if (vstr.IsEmpty()) {
			//Canon (�œ_�����̂�)
			vstr = lst->Values["CN:1.23"];
			if (!vstr.IsEmpty()) {
				int t = vstr.ToIntDef(0);					//T�[
				int w = get_ListIntVal(lst, _T("CN:1.24"));	//W�[
				int u = get_ListIntVal(lst, _T("CN:1.25"));	//���Z�P��
				if (t>0 && w>0 && u>0) {
					t /=u; w /= u;
					vstr = w;
					if (w!=t) vstr.cat_sprintf(_T("-%u"), t);
					vstr.UCAT_T("mm");
				}
			}
		}
		if (!vstr.IsEmpty()) lst->Add(tmp.sprintf(_T("42036=%s"), vstr.c_str()));
	}

	//GPS
	for (int i=0; i<2; i++) {
		UnicodeString ref  = lst->Values[(i==0)? "GPS:1" : "GPS:3"];
		if (ref.Length()!=1) continue;

		UnicodeString vnam;
		vnam.sprintf(_T("%s"), (i==0)? _T("GPS:2") : _T("GPS:4"));
		idx  = lst->IndexOfName(vnam);	if (idx==-1) continue;
		vstr = lst->Values[vnam];		if (vstr.IsEmpty()) continue;
		TStringDynArray ibuf = get_csv_array(vstr, 3, true);

		try {
			double d = ibuf[0].ToDouble();
			double m = ibuf[1].ToDouble();
			double s = ibuf[2].ToDouble();
			//DEG
			double deg = d + m/60.0 + s/3600.0;
			vstr = (contained_wd_i(_T("S|W"), ref))? "-" : "";
			vstr.cat_sprintf(_T("%.8f ("), deg);
			//DMS
			m += modf(d, &d) * 60;
			s += modf(m, &m) * 60;
			vstr.cat_sprintf(_T("%s%.0f��%.0f��%.2f��)"), ref.c_str(), d, m, s);
		}
		catch (...) {
			vstr = EmptyStr;
		}

		lst->Strings[idx] = tmp.sprintf(_T("%s=%s"), vnam.c_str(), vstr.c_str());
	}
}

//---------------------------------------------------------------------------
// �w��t�@�C���� Exif ���ꗗ���擾
//---------------------------------------------------------------------------
bool EXIF_GetInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//���ʊi�[���X�g(�^�ONo.=������ �`��)
{
	lst->Clear();
	if (!file_exists(fnam)) return false;

	UnicodeString fext = get_extension(fnam);
	bool isJpeg  = test_FileExt(fext, _T(".jpg.jpeg.jpe"));
	bool isHeic	 = test_FileExt(fext, _T(".heic"));
	bool isNikon = test_FileExt(fext, _T(".nef.nrw"));
	bool isCR2	 = test_FileExt(fext, _T(".cr2"));
	bool isCRW	 = test_FileExt(fext, _T(".crw"));
	bool isRaf	 = test_FileExt(fext, _T(".raf"));
	bool isX3f	 = test_FileExt(fext, _T(".x3f"));

	unsigned char xbuf[16];
	UnicodeString itmbuf;

	try {
		if (isCRW) {
			if (!CIFF_GetInf(fnam, lst)) Abort();
		}
		else {
			std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			TFileStream *fsp = fs.get();

			int cfa_ofs = 0;
			int top = 0;
			if (isRaf) {
				if (!EXIF_check_read(fsp, "FUJIFILMCCD-RAW")) Abort();
				if (fs->Seek(84, soFromBeginning)!=84) Abort();
				top = EXIF_get_data_4(fsp, true);		if (top==0) Abort();
				fs->Seek(4, soFromCurrent);
				cfa_ofs = EXIF_get_data_4(fsp, true);
			}
			else if (isX3f) {
				if (!EXIF_check_read(fsp, "FOVb")) Abort();
				unsigned short vi = EXIF_get_data_2(fsp, false);
				unsigned short vj = EXIF_get_data_2(fsp, false);
				int v = vj*10 + vi;
				top = (v>=40)? 0x300 : (v>22)? 104 + 160 : 72 + 160;
				lst->Add(itmbuf.sprintf(_T("FOVbVer=%u.%u"), vj, vi));
				if (fs->Seek(top, soFromBeginning)!=top) Abort();
				fs->Seek((v>=40)? 40 : 28, soFromBeginning);
				int w = EXIF_get_data_4(fsp, false);
				int h = EXIF_get_data_4(fsp, false);
				int r = EXIF_get_data_4(fsp, false);
				if (r==90 || r==270) std::swap(w, h);
				if (w>0 && h>0) {
					lst->Add(itmbuf.sprintf(_T("X3F:256=%u"), w));
					lst->Add(itmbuf.sprintf(_T("X3F:257=%u"), h));
				}
				if (fs->Seek(top, soFromBeginning)!=top) Abort();
				if (!EXIF_check_read(fsp, "SECi")) Abort();
				top += 28;
			}

			if (fs->Seek(top, soFromBeginning)!=top) Abort();

			if (isRaf || isX3f) {
				//APP1
				if (fs->Read(xbuf, 4)<4) Abort();
				if (xbuf[0]!=0xff || xbuf[1]!=0xd8 || xbuf[2]!=0xff || xbuf[3]!=0xe1) Abort();
				fs->Seek(2, soFromCurrent);
				if (!EXIF_check_read(fsp, "Exif")) Abort();
				fs->Seek(2, soFromCurrent);
				top += 12;
			}
			else if (isJpeg) {
				if (fs->Read(xbuf, 4)<4) Abort();
				if (xbuf[0]!=0xff || xbuf[1]!=0xd8 || xbuf[2]!=0xff) Abort();
				//�擪��APP0
				if (xbuf[3]==0xe0) {
					//APP0���X�L�b�v
					unsigned short len = EXIF_get_data_2(fsp, true);
					fs->Seek(len - 2, soFromCurrent);
					//APP1
					if (fs->Read(xbuf, 2)<2) Abort();
					if (xbuf[0]!=0xff || xbuf[1]!=0xe1) Abort();
				}
				//�擪��APP1
				else if (xbuf[3]==0xe1) {
					fs->Seek(2, soFromCurrent);
					if (!EXIF_check_read(fsp, "Exif")) Abort();
					fs->Seek(2, soFromCurrent);
					top = fs->Position;
				}
				else Abort();
			}
			else if (isHeic) {
				int bsz = EXIF_get_data_4(fsp, true);	//Box�T�C�Y
				if (bsz!=24) Abort();
				if (!EXIF_check_read(fsp, "ftyp")) Abort();
				//meta
				fs->Seek(24, soFromBeginning);
				bsz = EXIF_get_data_4(fsp, true);
				if (!EXIF_check_read(fsp, "meta")) Abort();
				//mdat
				fs->Seek(24 + bsz, soFromBeginning);
				fs->Seek(4, soFromCurrent);
				if (!EXIF_check_read(fsp, "mdat")) Abort();
				fs->Seek(8, soFromCurrent);
				bsz = EXIF_get_data_4(fsp, true);
				if (bsz>6) {
					fs->Seek(bsz, soFromCurrent);
					bsz = EXIF_get_data_4(fsp, true);
				}
				if (bsz==6) {
					if (!EXIF_check_read(fsp, "Exif")) Abort();
					fs->Seek(2, soFromCurrent);
					top = fs->Position;
				}
				else Abort();
			}

			//Byte Order
			bool BigEndian;
			if (fs->Read(xbuf, 2)<2) Abort();
			if		(xbuf[0]==0x4d && xbuf[1]==0x4d) BigEndian = true;
			else if (xbuf[0]==0x49 && xbuf[1]==0x49) BigEndian = false;
			else Abort();

			//Magic Number
			unsigned short magic_no = EXIF_get_data_2(fsp, BigEndian);
			if (test_FileExt(fext, _T(".rw2.raw"))) {
				if (magic_no!=0x0055) Abort();
			}
			else if (test_FileExt(fext, _T(".orf"))) {
				if (magic_no!=0x4f52 && magic_no!=0x5352) Abort();
			}
			else if (magic_no!=0x002a) Abort();

			//0th IFD
			int idf_ofs = EXIF_get_data_4(fsp, BigEndian);
			fs->Seek(top + idf_ofs, soFromBeginning);
			EXIF_get_idf_inf(fsp, top,  BigEndian, lst);

			//Exif IFD
			UnicodeString tmpstr = lst->Values["34665"];
			if (!tmpstr.IsEmpty()) {
				idf_ofs = tmpstr.ToIntDef(0);
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					EXIF_get_idf_inf(fsp, top, BigEndian, lst);
				}
			}

			//SubIFD
			tmpstr = lst->Values["330"];
			if (!tmpstr.IsEmpty()) {
				TStringDynArray t_lst = SplitString(tmpstr, ",");
				//Sub-0
				idf_ofs = (t_lst.Length>0)? t_lst[0].ToIntDef(0) : 0;
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					EXIF_get_idf_inf(fsp, top, BigEndian, lst, "S0:");
					//�v���r���[�摜���
					int typ  = get_ListIntVal(lst, _T("S0:259"));
					int ptr  = get_ListIntVal(lst, _T("S0:513"));
					int size = get_ListIntVal(lst, _T("S0:514"));
					if (typ==6 && ptr>0 && size>0) {
						fs->Seek(ptr, soFromBeginning);
						for (;;) {
							if (fs->Read(xbuf, 1)<1) break;
							if (xbuf[0]!=0xff) continue;
							if (fs->Read(xbuf, 1)<1) break;
							int mark = xbuf[0];
							if (mark>=0xd0 && mark<=0xd9) continue;	//SOI,EOI,RSTn�̓X�L�b�v
							unsigned short seglen = EXIF_get_data_2(fsp, true);
							if (mark>=0xc0 && mark<=0xcf && mark!=0xc4 && mark!=0xc8) {
								//SOF
								fs->Seek(1, soFromCurrent);
								unsigned short h = EXIF_get_data_2(fsp, true);
								unsigned short w = EXIF_get_data_2(fsp, true);
								if (h>0 && w>0) {
									lst->Add(itmbuf.sprintf(_T("SOF:256="), w));
									lst->Add(itmbuf.sprintf(_T("SOF:257="), h));
								}
								break;
							}
							else fs->Seek(seglen - 2, soFromCurrent);
						}
					}
				}

				//Sub-1
				idf_ofs = (t_lst.Length>1)? t_lst[1].ToIntDef(0) : 0;
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					EXIF_get_idf_inf(fsp, top, BigEndian, lst, "S1:");
				}

				//Sub-2
				idf_ofs = (t_lst.Length>2)? t_lst[2].ToIntDef(0) : 0;
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					EXIF_get_idf_inf(fsp, top, BigEndian, lst, "S2:");
				}
			}

			//���[�J�[�m�[�g
			tmpstr = lst->Values["37500"];
			if (!tmpstr.IsEmpty()) {
				idf_ofs = tmpstr.ToIntDef(0);
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					//Nikon Type-3
					if (isNikon && EXIF_check_read(fsp, "Nikon") && EXIF_get_data_2(fsp, true)==0x02) {
						fs->Seek(3 + 8, soFromCurrent);
						EXIF_get_idf_inf(fsp, top + idf_ofs + 10, BigEndian, lst, "NK:");
					}
					//Canon
					else if (isCR2) {
						unsigned short entn = EXIF_get_data_2(fsp, false);
						for (int i=0; i<entn; i++) {
							unsigned short tag = EXIF_get_data_2(fsp, false);	if (tag>4) break;
							unsigned short typ = EXIF_get_data_2(fsp, false);
							int cnt = EXIF_get_data_4(fsp, false);
							int ofs = EXIF_get_data_4(fsp, false);
							if ((tag==1 || tag==4) && typ==3) {
								lst->Add(itmbuf.sprintf(_T("CN:%u=%u,%u,%u"),tag, typ, cnt, ofs));
								int p = fs->Seek(0, soFromCurrent);
								fs->Seek(ofs, soFromBeginning);
								for (int j=0; j<cnt; j++)
									lst->Add(itmbuf.sprintf(_T("CN:%u.%u=%u"),
												tag, j, EXIF_get_data_2(fsp, false)));
								fs->Seek(p, soFromBeginning);
							}
						}
					}
				}
			}

			//FujiFilm(.raf) CFA Header
			if (isRaf && cfa_ofs>0) {
				UnicodeString val_str;
				if (fs->Seek(cfa_ofs, soFromBeginning)!=cfa_ofs) Abort();
				int n = EXIF_get_data_4(fsp, true);	if (n==0) Abort();
				for (int i=0; i<n; i++) {
					unsigned short tag = EXIF_get_data_2(fsp, true);	if (tag==0) break;
					unsigned short siz = EXIF_get_data_2(fsp, true);	if (siz==0) break;
					if (tag==0x111) {	//����RAW�摜�T�C�Y
						if (siz==4) {
							lst->Add(itmbuf.sprintf(_T("CFA:257=%u"), EXIF_get_data_2(fsp, true)));
							lst->Add(itmbuf.sprintf(_T("CFA:256=%u"), EXIF_get_data_2(fsp, true)));
						}
						break;
					}
					else fs->Seek(siz, soFromCurrent);
				}
			}

			//GPS
			tmpstr = lst->Values["34853"];
			if (!tmpstr.IsEmpty()) {
				idf_ofs = tmpstr.ToIntDef(0);
				if (idf_ofs>0) {
					fs->Seek(top + idf_ofs, soFromBeginning);
					EXIF_get_idf_inf(fsp, top, BigEndian, lst, "GPS:");
				}
			}
		}

		EXIF_format_inf(fext, lst);
		return true;
	}
	catch (EAbort &e) {
		lst->Clear();
		return false;
	}
	catch (...) {
		lst->Clear();
		return false;
	}
}

//---------------------------------------------------------------------------
//Exif���X�g����摜�T�C�Y���擾
//---------------------------------------------------------------------------
bool Exif_GetImgSize(TStringList *lst, UnicodeString fext, unsigned int *w, unsigned int *h)
{
	UnicodeString w_str, h_str;

	//RW2
	if (test_FileExt(fext, _T(".rw2.raw")) && !lst->Values["4"].IsEmpty()) {
		w_str = lst->Values["2"]; h_str = lst->Values["3"];
		int s_w = get_ListIntVal(lst, _T("7")) - get_ListIntVal(lst, _T("5"));
		if (s_w>0) w_str = s_w;
		int s_h = get_ListIntVal(lst, _T("6")) - get_ListIntVal(lst, _T("4"));
		if (s_h>0) h_str = s_h;
	}
	//X3F
	else if (test_FileExt(fext, _T(".x3f")) && !lst->Values["X3F:256"].IsEmpty()) {
		w_str = lst->Values["X3F:256"]; h_str = lst->Values["X3F:257"];
	}
	//RAF (CFA)
	else if (!lst->Values["CFA:256"].IsEmpty()) {
		w_str = lst->Values["CFA:256"]; h_str = lst->Values["CFA:257"];
	}
	else {
		int wd = 0, hi = 0;
		if (!ListVal_is_empty(lst, _T("40962"))) {
			wd = get_ListIntVal(lst, _T("40962"));
			hi = get_ListIntVal(lst, _T("40963"));
		}

		if (!test_FileExt(fext, _T(".jpg.jpeg.jpe")) || wd==0 || hi==0) {
			if (!ListVal_is_empty(lst, _T("S2:256"))) {
				wd = std::max(get_ListIntVal(lst, _T("S2:256")), wd);
				hi = std::max(get_ListIntVal(lst, _T("S2:257")), hi);
			}
			if (!ListVal_is_empty(lst, _T("S1:256"))) {
				wd = std::max(get_ListIntVal(lst, _T("S1:256")), wd);
				hi = std::max(get_ListIntVal(lst, _T("S1:257")), hi);
			}
			if (!ListVal_is_empty(lst, _T("S0:256"))) {
				wd = std::max(get_ListIntVal(lst, _T("S0:256")), wd);
				hi = std::max(get_ListIntVal(lst, _T("S0:257")), hi);
			}
			if (!ListVal_is_empty(lst, _T("256"))) {
				wd = std::max(get_ListIntVal(lst, _T("256")), wd);
				hi = std::max(get_ListIntVal(lst, _T("257")), hi);
			}
		}

		if (wd>0 && hi>0) {
			w_str = wd;
			h_str = hi;
		}
	}

	if (w_str.IsEmpty() || h_str.IsEmpty()) return false;

	//NEF,NRW
	//�����T�C�Y�ƃT���l�C���T�C�Y���r���ĉ摜�T�C�Y��ݒ�
	if (test_FileExt(fext, _T(".nef.nrw")) && !ListVal_is_empty(lst, _T("SOF:256"))) {
		float s0 = w_str.ToIntDef(0) * h_str.ToIntDef(0);
		float s1 = get_ListIntVal(lst, _T("SOF:256")) * get_ListIntVal(lst, _T("SOF:257"));
		if (fabs(s0/s1 - 1.0)<0.05) {	//***
			w_str = lst->Values["SOF:256"];
			h_str = lst->Values["SOF:257"];
		}
	}

	//����
	if (!test_FileExt(fext, _T(".3fr"))) { 
		int ori = get_ListIntVal(lst, _T("274"));
		if (ori==6 || ori==8) std::swap(w_str, h_str);
	}

	*w = w_str.ToIntDef(0);
	*h = h_str.ToIntDef(0);
	return true;
}

//---------------------------------------------------------------------------
//�t�@�C����Exif�B�e�������擾
//---------------------------------------------------------------------------
bool EXIF_GetExifTime(UnicodeString fnam, TDateTime &dt)
{
	bool res = false;
	std::unique_ptr<TStringList> taglst(new TStringList());
	if (EXIF_GetInf(fnam, taglst.get())) {
		UnicodeString tstr = taglst->Values["36867"];	// = �B�e����
		if (tstr.Length()==19) {
			try {
				dt	= TDateTime(tstr);
				res = true;
			}
			catch (EConvertError &e) {
				;
			}
		}
	}
	return res;
}

//---------------------------------------------------------------------------
//�t�@�C����Exif�B�e�������w�菑���Ŏ擾
//---------------------------------------------------------------------------
UnicodeString EXIF_GetExifTimeStr(UnicodeString fnam, UnicodeString format)
{
	UnicodeString ret_str;
	TDateTime dt;
	if (EXIF_GetExifTime(fnam, dt)) ret_str = FormatDateTime(format, dt);
	return ret_str;
}

//---------------------------------------------------------------------------
//�t�@�C���̃^�C���X�^���v�� Exif �B�e������
//---------------------------------------------------------------------------
bool EXIF_SetExifTime(UnicodeString fnam)
{
	bool ret = false;
	std::unique_ptr<TStringList> taglst(new TStringList());
	if (EXIF_GetInf(fnam, taglst.get())) {
		UnicodeString tstr = taglst->Values["36867"];	// = �B�e����
		if (tstr.Length()==19) {
			TDateTime dt;
			try {
				dt	= TDateTime(tstr);
				ret = set_file_age(fnam, dt);
			}
			catch (EConvertError &e) {
				ret = false;
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------------
//Jpeg�t�@�C������Exif�����폜
//  �߂�l:  0=���� -1=���s 1=Exif�Ȃ�
//---------------------------------------------------------------------------
int EXIF_DelJpgExif(
	UnicodeString fnam,		//�Ώۃt�@�C����
	UnicodeString onam,		//�o�̓t�@�C����
	bool keep_time)			//�^�C���X�^���v���ێ�
{
	try {
		if (!file_exists(fnam)) Abort();

		//�����O�Ƀ^�C���X�^���v������Ă���
		TDateTime ft;
		if (keep_time) ft = get_file_age(fnam);

		std::unique_ptr<TFileStream> i_fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		TFileStream *isp = i_fs.get();

		std::unique_ptr<BYTE[]> xbuf(new BYTE[32]);
		//APP1
		if (i_fs->Read(xbuf.get(), 4)<4) Abort();
		if (xbuf[0]!=0xff || xbuf[1]!=0xd8 || xbuf[2]!=0xff || xbuf[3]!=0xe1) throw EAbort("NoExif");
		unsigned short app1_len = EXIF_get_data_2(isp, true);
		if (!EXIF_check_read(isp, "Exif")) Abort();
		i_fs->Seek(2, soFromCurrent);
		int app1_ofs = app1_len + 4;
		int img_size = i_fs->Size - app1_ofs;

		//Byte Order
		bool BigEndian;
		if (i_fs->Read(xbuf.get(), 2)<2) Abort();
		if (xbuf[0]==0x4d) {
			if (xbuf[1]!=0x4d) Abort();
			BigEndian = true;
		}
		else if (xbuf[0]==0x49) {
			if (xbuf[1]!=0x49) Abort();
			BigEndian = false;
		}
		else Abort();

		//Magic Number
		if (EXIF_get_data_2(isp, BigEndian)!=0x002a) Abort();

		//0th IFD
		int top = 12;
		int idf_ofs = EXIF_get_data_4(isp, BigEndian);
		i_fs->Seek(top + idf_ofs, soFromBeginning);
		std::unique_ptr<TStringList> i_lst(new TStringList());
		EXIF_get_idf_inf(isp, top, BigEndian, i_lst.get());

		//�𑜓x���擾
		int rm = 2, rx = 72, ry = 72;	//�f�t�H���g 72dpi
		if (!ListVal_is_empty(i_lst.get(), _T("296"))) {
			rm = get_ListIntVal(i_lst.get(), _T("296"), 2);	//dpi
			rx = get_ListIntVal(i_lst.get(), _T("282"), 72);
			ry = get_ListIntVal(i_lst.get(), _T("283"), 72);
		}
		if (rm==3) rm = 2; else rm = 1;	//APP1�p�ɕϊ�

		//APP1���X�L�b�v
		i_fs->Seek(app1_ofs, soFromBeginning);
		//�������X�g���[���ɃC���[�W���R�s�[
		std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
		if (ms->CopyFrom(isp, img_size)<img_size) Abort();
		//���̓X�g���[����j��
		TFileStream *sp = i_fs.release();
		delete sp;

		//APP0���쐬���ď�������
		if (!file_exists(onam)) {
			if (!create_EmptyFile(onam)) Abort();
		}
		std::unique_ptr<TFileStream> o_fs(new TFileStream(onam, fmCreate));
		xbuf[0]  = 0xff; xbuf[1] = 0xd8;		//SOI
		xbuf[2]  = 0xff; xbuf[3] = 0xe0;		//APP0�}�[�J
		xbuf[4]  = 0x00; xbuf[5] = 0x10;
		xbuf[6]  = 'J'; xbuf[7] = 'F'; xbuf[8] = 'I'; xbuf[9] = 'F'; xbuf[10] = 0x00;
		xbuf[11] = 0x01; xbuf[12] = 0x01;		//�o�[�W����1.01
		xbuf[13] = rm;							//�𑜓x
		xbuf[14] = rx/256; xbuf[15] = rx%256;
		xbuf[16] = ry/256; xbuf[17] = ry%256;
		xbuf[18] = 0x00; xbuf[19] = 0x00;		//�T���l�C���Ȃ�
		o_fs->Write(xbuf.get(), 20);
		//�C���[�W����������
		o_fs->Write(ms->Memory, ms->Size);
		//�o�̓X�g���[����j��
		sp = o_fs.release();
		delete sp;

		//�^�C���X�^���v��ݒ�
		if (keep_time && (int)ft>0) set_file_age(onam, ft);

		return 0;
	}
	catch (EAbort &e) {
		return USAME_TI(e.Message, "NoExif")? 1 : -1;
	}
	catch (...) {
		//�G���[
		return -1;
	}
}
//---------------------------------------------------------------------------