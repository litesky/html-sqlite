#include <ctype.h>
#include <stdlib.h>
#include "escape-json.h"
extern char *dict =  NULL;
void dict_init()
{
    dict = (char *)malloc(256);
    memset(dict, 0, 256);
    dict[(unsigned int)'"']='"';
    dict[(unsigned int)'/']='/';
    dict[(unsigned int)'\b']= 'b';
    dict[(unsigned int)'\n']= 'n';
    dict[(unsigned int)'\r']= 'r';
    dict[(unsigned int)'\t']= 't';
    dict[(unsigned int)'\f']= 'f';
    //dict[(unsigned int)'\'']= '\'';
}
int escape_json(char *buf, char *dst)
{
    int ext = 0;
    int pos = 0;
    int mak = buf;
    if (dict == NULL) {
        dict_init();
    }
    if ((*buf)==0x00) {
        if(dst!=NULL) {
            dst[0] = 'N';
            dst[1] = 'U';
            dst[2] = 'L';
            dst[3] = 'L';
            dst[4] = '\0';
        }
        return 5;
    }
    while ((*buf)!=0x00 ) {
        if (dict[(unsigned char)(*buf)] != 0) {
            ++ext;
            if(dst!=NULL) {
                dst[pos] = '\\';
                ++pos;
                dst[pos] = dict[(unsigned char)(*buf)];
                ++pos;
            }
        } else {
            if(dst!=NULL) {
                dst[pos] = (*buf);
                ++pos;
            }
        }
        ++buf;
    }
    if(dst!=NULL) {
        dst[pos] = 0x00;
    }
    if (ext) {
      return (buf - mak) + (ext + 1);
    } else {
      return 0;
    }
    
}

char *escape_json_pro(char *buf)
{
    char *dst;
    int size;
    size = escape_json(buf, NULL);
    if ( size !=0 ) {
        dst= (char *)malloc(size);
        escape_json(buf, dst);
        free(buf);
    } else {
        dst = buf;
    }
    return dst;
}

char *escape_json_ext(char *buf, char **dst)
{
    int size;
    char *tmp = NULL;
    size = escape_json(buf, NULL);
    if ( size !=0 ) {
        if (*dst!=NULL) {
            free(*dst);
        }
        tmp= (char *)malloc(size);
        escape_json(buf, (char *)tmp);
        *dst= tmp;
        return tmp;
    } else {
        return buf;
    }
}