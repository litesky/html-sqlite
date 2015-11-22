#include <wchar.h>
#include <string.h>
#include <stdio.h>
#include <windows.h>
#include "code-conv.h"

char *utf8_to_wbcs( char *buf )
{
    char *wcstr = NULL;
    char *mbstr = buf;
    if ( mbstr && ( *mbstr != 0x00 ) ) {

        wcstr = ( char *)mb2wc( mbstr, CP_UTF8 );
        if ( wcstr ) {
            return ( char *)wcstr;
        }
    }
    return NULL;
}

char *mbcs_to_wbcs( char *buf )
{
    char *wcstr = NULL;
    char *mbstr = buf;
    if ( mbstr && ( *mbstr != 0x00 ) ) {

        wcstr = (char *)mb2wc( mbstr, CP_ACP );
        if ( wcstr ) {
            return wcstr;
        }
    }
    return NULL;
}

char *wbcs_to_mbcs( char *buf )
{
    char *mbstr = NULL;
    char *wcstr =  buf;
    if ( wcstr && ( *wcstr != 0x00 ) ) {

        mbstr = (char *)wc2mb( wcstr, CP_ACP );
        if ( mbstr ) {
            return mbstr;
        }
    }
    #ifdef DEBUG___
    printf( "\nSource buffer is start with 0x00. 1201?\n");
    #endif
    return NULL;
}

char *wbcs_to_utf8( char *buf )
{
    char *mbstr = NULL;
    char *wcstr =  buf;
    if ( wcstr && ( *wcstr != 0x00 ) ) {

        mbstr = (char *)wc2mb( wcstr, CP_UTF8 );
        if ( mbstr ) {
            return mbstr;
        }
    }
    #ifdef DEBUG___
    printf( "\nSource buffer is start with 0x00. 1201?\n");
    #endif
    return NULL;
}

char *utf8_to_mbcs( char *buf )
{
    char *nmbstr = NULL;
    char *wcstr = NULL;
    char *mbstr =  buf;
    if ( mbstr && ( *mbstr != 0x00 ) ) {
        wcstr = (char *)mb2wc( mbstr, CP_UTF8 );
        if ( wcstr ) {
            nmbstr = (char *)wc2mb( wcstr, CP_ACP );
            free( wcstr );
        }
    }
    if ( nmbstr ) {
        return nmbstr;
    }
    return NULL;
}

char *mbcs_to_utf8( char *buf )
{
    char *nmbstr = NULL;
    char *wcstr = NULL;
    char *mbstr =  buf;
    if ( mbstr && ( *mbstr != 0x00 ) ) {

        wcstr = (char *)mb2wc( mbstr, CP_ACP );
        if ( wcstr ) {
            nmbstr = (char *)wc2mb( wcstr, CP_UTF8 );
            free( wcstr );
        }
    }
    if ( nmbstr ) {
        return nmbstr;
    }
    return NULL;
}

char *mb2wc( char *mbstr, int cp )
{
    char *wcstr = NULL;
    size_t size = 0;
    #ifdef _WIN32
    size = MultiByteToWideChar( cp, 0, ( LPCSTR )mbstr, -1, NULL, 0 ) * 2;
    #else
    size = mbstowcs( NULL, mbstr, 0 );
    #endif
    wcstr = ( char *) malloc( size );
    if ( wcstr ) {
        #ifdef _WIN32
        if ( (MultiByteToWideChar( cp, 0, ( LPCSTR )mbstr, -1, wcstr, size )) == 0 )
        #else
        if ( (mbstowcs( wcstr, mbstr, size + 1 ) ) == -1 )
        #endif
        {
            free( wcstr );
            return NULL;
        }
    }
    return wcstr;
}

char *wc2mb( char *wcstr, int cp )
{
    char *mbstr = NULL;
    size_t size = 0;
    #ifdef _WIN32
    size = WideCharToMultiByte( cp, 0, ( wchar_t *)wcstr, -1, NULL, 0, NULL, 0 );
    #else
    size = wcstombs( NULL, wcstr, 0 );
    #endif
    mbstr = ( char *) malloc( size );
    if ( mbstr ) {
        #ifdef _WIN32
        if ( (WideCharToMultiByte( cp, 0, ( wchar_t *)wcstr, -1, mbstr, size, NULL, 0 )) == 0 )
        #else
        if ( (wcstombs( mbstr, wcstr, size + 1 ) ) == -1 )
        #endif
        {
            free( mbstr );
            mbstr = NULL;
        }
    }
    return mbstr;
}