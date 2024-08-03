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

typedef struct {
    char version[4];
    char code[4];
    char reason[200];
    HashTable *headers;
    char *body;
} HttpResponse;

HttpRequest *create_http_request(HttpMethod method, char version[3], const char *path);
void destroy_http_request(HttpRequest *hr);
void http_request_add_header(HttpRequest *hr, const char *key, const char *value);
HttpRequest *try_parse_http_request(const char *buffer);

HttpResponse *create_http_response(char version[4], char code[4], char *reason);
void destroy_http_response(HttpResponse *hr);
void http_response_add_header(HttpResponse *hr, const char *key, const char *value);
void http_response_add_body(HttpResponse *hr, const char *text);
const char *generate_response_text(HttpResponse *hr);

#endif
