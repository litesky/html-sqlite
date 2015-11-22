#include <stdlib.h>
#include "lstring.h"
lstr_t *lstr_new ( void )
{
    lstr_t *s;

    s = malloc ( sizeof ( lstr_t ) );
    if ( NULL == s ) {
        return 0;
    }

    s->data = malloc ( LSTR_BLOCK_SIZE );
    if ( NULL == s->data ) {
        free ( s );
        s = NULL;
    } else {
        s->capa = LSTR_BLOCK_SIZE;
        s->size = 0;
    }

    return s;
}
void lstr_free ( lstr_t *self )
{
    if ( NULL == self ) {
        return;
    }

    if ( NULL != self->data ) {
        free ( self->data );
    }

    free ( self );
}

int lstr_ext ( lstr_t *self, size_t size )
{
    char *buf;

    if ( NULL == self ) {
        return 0;
    }

    buf = malloc ( self->capa + size );
    if ( NULL == buf ) {
        return 0;
    }

    memcpy ( buf, self->data, self->size );
    buf[self->size] = '\0';

    free ( self->data );

    self->data = buf;
    self->capa += size;

    return 1;
}

char *cstr_sub( char *ptr, int len )
{
    char *dst = ( char *)malloc( sizeof( char ) * ( len + 1 ) );
    memcpy( dst, ptr, len );
    dst[len] = '\0';
    return dst;
};

int lstr_cat_cstr ( lstr_t *self, const char *other, size_t size )
{
    size_t len, acc;

    if ( NULL == self
            || NULL == other ) {
        return 0;
    }
    if ( size == 0 ) {
        len = strlen ( other );
    } else {
        len = size;
    }

    if ( 0 == len ) {
        return 1;
    }
    acc = self->capa - 1;
    if ( acc < ( self->size + len ) ) {
        acc = self->size + len - acc;
        if ( ( acc % LSTR_BLOCK_SIZE ) != 0 ) {
            acc = acc - ( acc % LSTR_BLOCK_SIZE ) + LSTR_BLOCK_SIZE;
        }
        lstr_ext ( self, acc );
    }
    memcpy ( ( self->data ) + self->size, other, len );
    self->data[self->size + len] = '\0';
    self->size = self->size + len;
    return 1;
}
