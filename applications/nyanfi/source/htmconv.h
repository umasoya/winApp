//----------------------------------------------------------------------//
// HTML���e�L�X�g�ϊ������N���X											//
//																		//
//----------------------------------------------------------------------//
#ifndef htmconvH
#define htmconvH

//---------------------------------------------------------------------------
#define DEF_LINE_WIDTH		80
#define DEF_BLANK_LN_LIMIT	3				//�A����s����

//---------------------------------------------------------------------------
class HtmConv
{
private:
	UnicodeString TxtLineBuf;
	bool Skip;
	bool fPRE, fXMP;

	UnicodeString rel_to_abs(UnicodeString fnam, UnicodeString rnam);

	UnicodeString int_to_alpha(int n);
	UnicodeString int_to_roman(int n);
	void del_tag_block(TStringList *lst, const _TCHAR *start_wd, const _TCHAR *end_wd);

	UnicodeString GetTag(UnicodeString s);
	void FlushText(UnicodeString s = EmptyStr);
	void AddHR();

public:
	HtmConv();
	~HtmConv();

	UnicodeString FileName;
	TStringList *HtmBuf;		//HTML�������̓o�b�t�@
	TStringList *TxtBuf;		//�e�L�X�g�o�̓o�b�t�@

	int  CodePage;				//�o�̓R�[�h�y�[�W

	bool ToMarkdown;			//Markdown �ɕϊ�

	int  LineWidth;				//����
	UnicodeString HeaderStr;	//���o������(;��؂�)
	UnicodeString UlMarkStr;	//UL�}�[�N
	UnicodeString InsHrClass;	//�w��N���X��DIV�̑O�Ɍr���}��

	bool InsHdrInf;				//�w�b�_����}��
	bool InsHrSection;			//<section>�̑O�Ɍr���}��
	bool InsHrArticle;			//<article>�̑O�Ɍr���}��
	bool InsHrNav;				//<nav>�̑O�Ɍr���}��

	UnicodeString Title;
	UnicodeString Description;
	UnicodeString Keywords;

	UnicodeString GetTagAtr(UnicodeString s, UnicodeString t, const _TCHAR *a);
	UnicodeString RefEntity(UnicodeString s);
	void Convert();
};
//---------------------------------------------------------------------------
#endif