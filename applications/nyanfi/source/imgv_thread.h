//----------------------------------------------------------------------//
// NyanFi																//
//  �摜�r���[�����X���b�h												//
//----------------------------------------------------------------------//
#ifndef ImgViewThreadH
#define ImgViewThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
class TImgViewThread : public TThread
{
private:
	UnicodeString Img_f_name;
	__int64		  Img_f_size;
	TDateTime	  Img_f_time;
	UnicodeString Img_rot_str;	//���^�t�@�C���̉�]�E���]��������

	UnicodeString MsgStr;
	bool MsgIsErr;

	void __fastcall ClearImage();
	void __fastcall DrawImage();
	void __fastcall DrawViewBuff();
	void __fastcall DrawMessage();
	void __fastcall Execute();

public:
	UnicodeString FileName;	//�摜�t�@�C����
	UnicodeString FileName2;

	bool TaskBusy;			//������
	bool ReqRedraw;			//�ĕ`���v��
	bool ReqReload;			//�����[�h��v��
	bool ReqChange;			//�r�b�g�}�b�v�ύX��v��
	bool ReqClear;			//��ʃN���A��v��
	bool ReqVideo;			//����(�T���l�C���\����v��)
	int  Rotation;			//��]/���]��v�� (1,3,4,5)
	int  ExifOri;			//Exif �̕������
	int  ZoomRatio;			//�Y�[���{��(%)
	double ZoomRatioF;

	bool Fitted;			//�t�B�b�g�\��
	bool GrayScaled;		//�O���[�X�P�[���\��
	bool ShowGrid;			//�O���b�h��\��
	bool UseViewBuff;		//���C�����Őݒ肳�ꂽ�o�b�t�@���e���g�p

	bool ImgIsWmf;

	Graphics::TBitmap *ImgBuff;		//�摜�o�b�t�@(���T�C�Y)
	Graphics::TBitmap *ViewBuff;	//�\���o�b�t�@
	Graphics::TBitmap *ChgBuff;
	TMetafile *MetaBuff;			//���^�t�@�C���p�o�b�t�@

	__fastcall TImgViewThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
