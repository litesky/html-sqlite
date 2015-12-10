#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <uv.h>
#include <sqlite3.h>
#include "h3.h"

#include "lstring.h"
#include "code-conv.h"
#include "echo-server.h"
#include "escape-json.h"

void get_sql( sql_t *sql );
void db_0( sql_t *sql );
void db_1( sql_t *sql );

/////////////////////
static const char *TB0 = "HTTP/1.1 200 OK\r\nServer: HSI\r\nContent-Type: text/plain;charset=GBK\r\nAccess-Control-Allow-Origin: *\r\nConnection: close\r\nContent-Length: ";
static const char *TB1 = "\r\n\r\n{\"code\":200,\"db\":[";
static const char *RTX = "HTTP/1.1 200 OK\r\nContent-Length: 20\r\nConnection: close\r\n\r\n{\"code\":403,\"db\":[]}";
static const char *RTO = "HTTP/1.1 200 OK\r\nContent-Length: 20\r\nConnection: close\r\n\r\n{\"code\":404,\"db\":[]}";
/////////////////////
void get_sql( sql_t *sql )
{
    RequestHeader *header;
    char opt;
    char *raw = NULL;
    int size = 0;
    header = h3_request_header_new();
    h3_request_header_parse(header, sql->raw, sql->size);
    opt = *(header->RequestURI+1);
    size = sql->raw + sql->size - header->RequestLineEnd;
    // If RequestLineEnd Is Not Initialized.
    // Initialize This In Function [h3_request_header_parse].
    // ?? https://github.com/c9s/h3/pull/14
    if (size == 0) {
        sql->size = strlen( RTX );
        sql->raw = cstr_sub( ( char *)(RTX), sql->size );
        h3_request_header_free(header);
        return;
    }
    raw = wbcs_to_mbcs(header->RequestLineEnd);
    sql->raw = raw;
    printf(">>%s<<\n", raw);
    switch ( opt ) {
        case '0': {
            db_0( sql );
            break;
        };
        case '1': {
            db_1( sql );
            break;
        };
        default: {
            free(sql->raw);
            sql->size = strlen( RTX );
            sql->raw = cstr_sub( ( char *)(RTX), sql->size );
        };
    }
    h3_request_header_free(header);
}

void db_0( sql_t *sql )
{
    char **pResult;
    char *errmsg;
    int i, j;
    int nRow, nCol, nResult, nIndex;
    char *buf = NULL;
    char *json_dst = NULL;
    lstr_t *strPre = NULL;
    lstr_t *strSuf = NULL;
    nResult = sqlite3_get_table( sql->db, sql->raw, &pResult, &nRow, &nCol, &errmsg );
    free(sql->raw);
    if ( nResult != SQLITE_OK ) {
        printf( "SQL: %s\n", errmsg );
        sqlite3_free( errmsg );
        sql->size = strlen( RTX );
        sql->raw = cstr_sub( ( char *)(RTX), sql->size );
        if (json_dst!=NULL) {
            free(json_dst);
        }
        return;
    }
    nIndex = nCol;
    strSuf = lstr_new();
    strPre = lstr_new();
    lstr_cat_cstr( strPre, TB0, 0 );
    lstr_cat_cstr( strSuf, TB1, 0 );
    // Block |v ColName Arrary
    lstr_cat_cstr( strSuf, "[ ", 2 );
    for ( j = 0; j < nCol; j++ ) {
        lstr_cat_cstr( strSuf, "\"", 1 );
        lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)pResult[j], &json_dst), 0 );
        lstr_cat_cstr( strSuf, "\",", 2 );
    };
    strSuf->size = strSuf->size - 1;
    lstr_cat_cstr( strSuf, "],", 2 );
    // Block ^|
    for ( i = 0; i < nRow; i++ ) {
        lstr_cat_cstr( strSuf, "{", 1 );
        for ( j = 0; j < nCol; j++ ) {
            lstr_cat_cstr( strSuf, "\"", 1 );
            lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)pResult[j], &json_dst), 0 );
            lstr_cat_cstr( strSuf, "\":\"", 3 );
            if (pResult[nIndex] != NULL) {
                lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)pResult[nIndex], &json_dst), 0 );
            } else {
                lstr_cat_cstr( strSuf, "NULL", 4 );
            }
            lstr_cat_cstr( strSuf, "\",", 2 );
            ++nIndex;
        };
        strSuf->size = strSuf->size - 1;
        lstr_cat_cstr( strSuf, "},", 2 );
    };
    strSuf->size = strSuf->size - 1;
    lstr_cat_cstr( strSuf, "]}", 2 );

    buf = ( char *)malloc( sizeof( char ) * (  32 ) );
    itoa( strSuf->size - 4, buf, 10 ); //\r\n\r\n
    lstr_cat_cstr( strPre, buf, 0 );

    lstr_cat_cstr( strPre, strSuf->data, strSuf->size );
    sql->raw = strPre->data;
    sql->size = strPre->size;
    if (strSuf->size> 82) {
        printf("%.*s\n",78,strSuf->data+4);
    } else {
        printf("%s\n",strSuf->data+4);
    }
    sqlite3_free_table( pResult );
    free(buf);
    free(strSuf->data);
    free(strSuf);
    free(strPre);
    if (json_dst!=NULL) {
        free(json_dst);
    }
}

void db_1( sql_t *sql )
{
    char *errmsg;
    int nResult;
    nResult = sqlite3_exec( sql->db, sql->raw, NULL, NULL, &errmsg );
    free(sql->raw);
    if ( nResult != SQLITE_OK ) {
        printf( "SQL: %s\n", errmsg );
        sqlite3_free( errmsg );
        sql->size = strlen( RTX );
        sql->raw = cstr_sub( ( char *)(RTX), sql->size );
        return;
    } else {
        sql->size = strlen( RTO );
        sql->raw = cstr_sub( ( char *)(RTO), sql->size );
        return;
    }
}
