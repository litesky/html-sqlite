#ifndef _JSON_CONV_
#define _JSON_CONV_
void dict_init();
int escape_json(char *buf, char *dst);
char *escape_json_pro(char *buf);
char *escape_json_ext(char *buf, char **dst);
#endif