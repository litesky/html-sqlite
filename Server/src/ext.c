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
void db_opt( sql_t *sql );

/////////////////////
static const char *C0 = "HTTP/1.1 200 OK\r\nServer: HSI\r\nContent-Type: text/plain;charset=GBK\r\nAccess-Control-Allow-Origin: *\r\nConnection: keep-alive\r\nContent-Length: ";
static const char *C1 = "\r\n\r\n{\"code\":200,\"db\":[[ "; //000
static const char *C2 = "\r\n\r\n{\"code\":403,\"db\":[[\""; //000
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
    // Like https://github.com/c9s/h3/pull/14 ?
    if (size == 0) {
      raw = (char *)malloc(1);
      *raw = 0;
    } else {
      raw = wbcs_to_mbcs(header->RequestLineEnd);
    }
    if (raw == NULL) {
      raw = (char *)malloc(1);
      *raw = 0;
    }
    sql->raw = raw;
    printf(">>%s<<\n", raw);
    db_opt( sql );

    h3_request_header_free(header);
}

void db_opt( sql_t *sql )
{
    const char *tail;
    sqlite3_stmt *stmt;
    int xx, rrc, nCol;
    char *colText, *errmsg;
    char **colName = 0;

    char *buf = NULL;
    char *json_dst = NULL;
    lstr_t *strPre = NULL;
    lstr_t *strSuf = NULL;

    rrc = sqlite3_prepare(sql->db, sql->raw, strlen(sql->raw), &stmt, &tail);
    free(sql->raw);

    strSuf = lstr_new();
    strPre = lstr_new();
    lstr_cat_cstr( strPre, C0, 0 );

    if (rrc == SQLITE_OK) {
      rrc = sqlite3_step(stmt);
      nCol = sqlite3_column_count(stmt);
      colName = (char *)malloc(sizeof(const char *) *nCol);
      lstr_cat_cstr( strSuf, C1, 0 );
    } else {
      errmsg = sqlite3_errmsg(sql->db);
      printf( "<<%s>>\n", errmsg);
      lstr_cat_cstr( strSuf, C2, 0 );
      lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)errmsg, &json_dst), 0 );
      lstr_cat_cstr( strSuf, "\" ", 2 ); //000
      //free(errmsg);
      //sqlite3_free(errmsg);
    }
    if (rrc == SQLITE_ROW) {
      for(xx=0; xx < nCol; xx++) {
        colName[xx] = (char *)sqlite3_column_name(stmt, xx);
        lstr_cat_cstr( strSuf, "\"", 1 );
        lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)colName[xx], &json_dst), 0 );
        lstr_cat_cstr( strSuf, "\",", 2 ); //000
      }
    }
    strSuf->size = strSuf->size - 1; //000
    lstr_cat_cstr( strSuf, "],", 2 ); //000

    while(rrc == SQLITE_ROW) {
        lstr_cat_cstr( strSuf, "{", 1 );
        for(xx=0; xx < nCol; xx++) {
            lstr_cat_cstr( strSuf, "\"", 1 );
            //colName=sqlite3_column_name(stmt, xx);
            lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)colName[xx], &json_dst), 0 );
            lstr_cat_cstr( strSuf, "\":\"", 3 );
            colText = sqlite3_column_text(stmt, xx);
            if (colText != NULL) {
                lstr_cat_cstr( strSuf, (char *)escape_json_ext((char *)colText, &json_dst), 0 );
                //free(colText);
            } else {
                lstr_cat_cstr( strSuf, "NULL", 4 );
            }
            lstr_cat_cstr( strSuf, "\",", 2 ); //001
        }
        strSuf->size = strSuf->size - 1; //001
        lstr_cat_cstr( strSuf, "},", 2 ); //000
        rrc = sqlite3_step(stmt);
    }
    strSuf->size = strSuf->size - 1; //000
    lstr_cat_cstr( strSuf, "]}", 2 ); //000

    buf = ( char *)malloc( sizeof( char ) * (  32 ) );
    itoa( strSuf->size - 4, buf, 10 ); //\r\n\r\n
    lstr_cat_cstr( strPre, buf, 0 );

    lstr_cat_cstr( strPre, strSuf->data, strSuf->size );
    sql->raw = strPre->data;
    sql->size = strPre->size;
    if (strSuf->size> 82) {
        printf("%.*s\n\n",78,strSuf->data+4);
    } else {
        printf("%s\n\n",strSuf->data+4);
    }

    free(buf);
    free(strSuf->data);
    free(strSuf);
    free(strPre);
    if (json_dst!=NULL) {
        free(json_dst);
    }
    sqlite3_finalize(stmt);
    if (colName!=NULL) {
        free(colName);
    }
}
