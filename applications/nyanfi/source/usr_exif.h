//----------------------------------------------------------------------//
// Exif èÓïÒÇÃèàóù														//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrExifH
#define UsrExifH

#include <vcl.h>

//---------------------------------------------------------------------------
bool EXIF_check_read(TFileStream *fs, const char *s);
unsigned short EXIF_get_data_2(TFileStream *fs, bool bigsw);
unsigned int   EXIF_get_data_4(TFileStream *fs, bool bigsw);

float CIFF_ev(int v);
bool CIFF_GetInf(UnicodeString fnam, TStringList *lst);

void EXIF_get_idf_inf(TFileStream *fs, int top, bool bsw, TStringList *lst, UnicodeString id = EmptyStr);
void EXIF_format_inf(UnicodeString fext, TStringList *lst);
bool EXIF_GetInf(UnicodeString fnam, TStringList *lst);
bool Exif_GetImgSize(TStringList *lst, UnicodeString fext, unsigned int *w, unsigned int *h);
bool EXIF_GetExifTime(UnicodeString fnam, TDateTime &dt);
UnicodeString EXIF_GetExifTimeStr(UnicodeString fnam, UnicodeString format);
bool EXIF_SetExifTime(UnicodeString fnam);
int  EXIF_DelJpgExif(UnicodeString fnam, UnicodeString onam, bool keep_time = true);
//---------------------------------------------------------------------------
#endif
