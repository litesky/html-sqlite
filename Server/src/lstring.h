#include <wchar.h>

#ifndef _LSTR_Library_H
#define _LSTR_Library_H

#define LSTR_BLOCK_SIZE 64

struct lstr_t {
    char *data;
    size_t size; // Used Size
    size_t capa; // Capa
};

typedef struct lstr_t lstr_t;

char *cstr_sub( char *ptr, int len );

lstr_t *lstr_new (void);
void lstr_free (lstr_t *self);
int lstr_ext (lstr_t *self, size_t size);
int lstr_cat_cstr (lstr_t *self, const char *other, size_t size);
#endif
