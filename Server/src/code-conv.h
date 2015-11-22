#ifndef _CODE_CONV_
#define  _CODE_CONV_
char *utf8_to_wbcs( char *buf );
char *utf8_to_mbcs( char *buf );
char *mbcs_to_wbcs( char *buf );
char *mbcs_to_utf8( char *buf );
char *wbcs_to_mbcs( char *buf );
char *wbcs_to_utf8( char *buf );
char *mb2wc( char *mbstr, int cp );
char *wc2mb( char *wcstr, int cp );
#endif