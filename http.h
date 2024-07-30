#ifndef HTTP_H
#define HTTP_H

#include "hashtable.h"

typedef enum {
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    INVALID,
} HttpMethod;

typedef struct {
    HttpMethod method;
    char version[4];
    char *path;
    HashTable *headers;
} HttpRequest;

HttpRequest *create_http_request(HttpMethod method, char version[3], const char *path);
void destroy_http_request(HttpRequest *hr);
void http_request_add_header(HttpRequest *hr, const char *key, const char *value);
HttpRequest *try_parse_http_request(const char *buffer);

#endif
