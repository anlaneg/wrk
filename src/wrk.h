#ifndef WRK_H
#define WRK_H

#include "config.h"
#include <pthread.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <lua.h>

#include "stats.h"
#include "ae.h"
#include "http_parser.h"

#define RECVBUF  8192

#define MAX_THREAD_RATE_S   10000000
#define SOCKET_TIMEOUT_MS   2000
#define RECORD_INTERVAL_MS  100

extern const char *VERSION;

typedef struct {
    pthread_t thread;
    aeEventLoop *loop;
    /*被测机器*/
    struct addrinfo *addr;
    /*每线程连接数*/
    uint64_t connections;
    /*此线程完成的连接数*/
    uint64_t complete;
    /*此线程发起的未统计请求数*/
    uint64_t requests;
    uint64_t bytes;
    uint64_t start;
    lua_State *L;
    errors errors;
    /*记录connections个连接*/
    struct connection *cs;
} thread;

typedef struct {
    char  *buffer;
    size_t length;
    char  *cursor;
} buffer;

typedef struct connection {
    thread *thread;
    http_parser parser;
    enum {
        FIELD, VALUE
    } state;
    /*连接对应的fd*/
    int fd;
    SSL *ssl;
    /*是否延迟发送*/
    bool delayed;
    uint64_t start;
    char *request;
    size_t length;
    size_t written;
    uint64_t pending;
    buffer headers;
    buffer body;
    /*存放读写数据位置*/
    char buf[RECVBUF];
} connection;

#endif /* WRK_H */
