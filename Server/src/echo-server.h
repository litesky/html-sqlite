#ifndef INC_H
#define INC_H
typedef struct {
    char *sql;
    sqlite3 *db;
    uv_tcp_t tcp;
    uv_tcp_t *sp;
    size_t tcp_nread;
    int isChunked;
} ctx_t;

typedef struct {
    char *raw;
    sqlite3 *db;
    size_t size;
} sql_t;

void conn_done( uv_stream_t *listener, int status );
void read_done( uv_stream_t *handle, ssize_t nread, uv_buf_t buf );
void write_done( uv_write_t *wreq, int status );
void close_done( uv_handle_t *handle );
void free_done( uv_handle_t *handle );
void shut_done( uv_shutdown_t *req, int status );
uv_buf_t alloc_buffer( uv_handle_t *handle, size_t size );
SQLITE_API char *SQLITE_STDCALL sqlite3_win32_utf8_to_mbcs(const char *zFilename);
SQLITE_API char *SQLITE_STDCALL sqlite3_win32_mbcs_to_utf8(const char *zFilename);
#endif