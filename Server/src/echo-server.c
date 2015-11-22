#include <uv.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include "code-conv.h"
#include "echo-server.h"


void conn_done( uv_stream_t *listener, int status )
{
    int n;
    ctx_t *ctx = ( ctx_t *) malloc( sizeof( ctx_t ) ); //创建实例，返回实例指针
    ctx->tcp.data = ctx;
    ctx->isChunked = 0;
    ctx->db = ( sqlite3 *)listener->data;
    n = uv_tcp_init( listener->loop, &ctx->tcp );
    if ( n ) {
        return;
    }
    n = uv_accept( listener, ( uv_stream_t *)&ctx->tcp );
    if ( n ) {
        return;
    }
    n = uv_read_start( ( uv_stream_t *)&ctx->tcp, alloc_buffer, read_done );
    if ( n ) {
        return;
    }
}

void read_done( uv_stream_t *handle, ssize_t nread, uv_buf_t buf )
{
    ctx_t *ctx = ( ctx_t *)handle->data;
    sql_t *sql = ( sql_t *) malloc( sizeof( sql_t ) );
    uv_write_t *wreq = ( uv_write_t *) malloc( sizeof( uv_write_t ) );
    int n;
    if ( nread < 0 ) { /* Error or EOF */
        if ( buf.base ) {
            free( buf.base );
        }
        uv_close( ( uv_handle_t *)&ctx->tcp, close_done );
        free( wreq );
        return;
    }
    if ( nread == 0 ) { /* Everything OK, but nothing read. */
        free( buf.base );
        uv_close( ( uv_handle_t *)&ctx->tcp, close_done );
        free( wreq );
        return;
    }
    sql->db = ctx->db;
    sql->raw = buf.base;
    sql->size = nread;
    get_sql( sql );
    free( buf.base );
    nread = sql->size;
    ctx->tcp_nread = nread;
    uv_read_stop( ( uv_stream_t *)&ctx->tcp );
    wreq->data = ctx;
    buf.len =sql->size;
    buf.base = sql->raw;
    n = uv_write( wreq, ( uv_stream_t *)&ctx->tcp, &buf, 1, write_done );
    free( buf.base );
    free(sql);
}

void write_done( uv_write_t *wreq, int status )
{
    ctx_t *ctx = ( ctx_t *)wreq->data;
    uv_close( ( uv_handle_t *)wreq->handle, close_done ); //close client
    free( wreq );
}

void close_done( uv_handle_t *handle )
{
    ctx_t *ctx = ( ctx_t *)handle->data;
    uv_shutdown_t *req;
    req = ( uv_shutdown_t *)malloc( sizeof( uv_shutdown_t ) );
    req->data = ctx;
    uv_shutdown( req, ( uv_stream_t *)&ctx->tcp, shut_done );
}

void shut_done( uv_shutdown_t *req, int status )
{
    ctx_t *ctx = ( ctx_t *)req->data;
    uv_close( ( uv_handle_t *)&ctx->tcp, free_done );
    free( req );
}

void free_done( uv_handle_t *handle )
{
    ctx_t *ctx = ( ctx_t *)handle->data;
    free( ctx );
}

uv_buf_t alloc_buffer( uv_handle_t *handle, size_t size )
{
    return uv_buf_init( ( char *) malloc( size ), size );
}

int main( void )
{
    sqlite3 *db;
    struct sockaddr_in addr = uv_ip4_addr( "127.0.0.1", 9050 );
    int ret = 0;
    uv_tcp_t listener;
    uv_loop_t *loop;
    ret = sqlite3_open( "data.db", &db );
    if ( ret != 0 ) {
        printf( "Open SQLite Database Failed\n" );
        return -1;
    }
    loop = uv_default_loop();
    uv_tcp_init( loop, &listener );
    listener.data = db;
    uv_tcp_bind( &listener, addr );
    uv_listen( ( uv_stream_t *) &listener, 1 , conn_done );
    uv_run( loop, UV_RUN_DEFAULT );
}