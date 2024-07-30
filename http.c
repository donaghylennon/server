#include "http.h"
#include "hashtable.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wctype.h>
#include <ctype.h>

HttpRequest *create_http_request(HttpMethod method, char version[3], const char *path) {
    if (!path)
        return NULL;
    HttpRequest *hr = (HttpRequest *)malloc(sizeof(HttpRequest));
    size_t path_len = strlen(path) + 1;
    hr->path = (char *)malloc(path_len * sizeof(char));
    hr->method = method;
    strcpy(hr->version, version);
    strcpy(hr->path, path);
    return hr;
}

void destroy_http_request(HttpRequest *hr) {
    if (!hr)
        return;
    if (hr->path)
        free(hr->path);
    free(hr);
}

void http_request_add_header(HttpRequest *hr, const char *key, const char *value) {
    if (!hr->headers)
        hr->headers = hashtable_create();
    hashtable_put(hr->headers, key, value);
}

HttpMethod parse_method(const char *buffer, size_t buflen) {
    if (!strncmp("GET", buffer, buflen))
        return GET;
    if (!strncmp("HEAD", buffer, buflen))
        return HEAD;
    if (!strncmp("POST", buffer, buflen))
        return POST;
    if (!strncmp("PUT", buffer, buflen))
        return PUT;
    if (!strncmp("DELETE", buffer, buflen))
        return DELETE;
    if (!strncmp("CONNECT", buffer, buflen))
        return CONNECT;
    if (!strncmp("OPTIONS", buffer, buflen))
        return OPTIONS;
    if (!strncmp("TRACE", buffer, buflen))
        return TRACE;
    return INVALID;
}

HttpRequest *parse_from_request_line(const char *buffer, size_t buflen) {
    // TODO: Make not (as much of) a mess using strstr and strtok
    size_t i = 0;
    while (iswspace(buffer[i]) && i < buflen)
        i++;
    char method_text[8];
    size_t m = 0;
    while (isalpha(buffer[i]) && i < buflen && m < 7)
        method_text[m++] = buffer[i++];
    method_text[m] = '\0';
    fprintf(stderr, "method text: %s\n", method_text);
    if (!iswspace(buffer[i]))
        return NULL;
    while (iswspace(buffer[i]) && i < buflen)
        i++;
    char path[200];
    size_t p = 0;
    while (!iswspace(buffer[i]) && i < buflen) {
        fprintf(stderr, "buffer[i]: %c\n", buffer[i]);
        path[p++] = buffer[i++];
    }
    path[p] = '\0';
    while (buffer[i] != '/' && i < buflen)
        i++;
    i++;
    char version[4];
    size_t v = 0;
    while (!iswspace(buffer[i]) && i < buflen)
        version[v++] = buffer[i++];
    version[v] = '\0';

    HttpMethod method = parse_method(method_text, 7);
    if (p > 0)
        return create_http_request(method, version, path);

    return NULL;
}

void parse_headers_into(HttpRequest *hr, const char *buffer) {
    hr->headers = hashtable_create();
    const char *p = buffer;
    while (iswspace(*p) && *p != '\0') {
        p++;
    }
    p = strstr(p, "\r\n");
    while (p && *p != '\0') {
        p += 2;
        if (*p == '\0')
            break;
        char line_copy[200];
        const char *end_of_line = strstr(p, "\r\n");
        ptrdiff_t line_len = end_of_line - p;
        strncpy(line_copy, p, line_len);
        line_copy[line_len] = '\0';

        const char *field, *value;
        field = strtok(line_copy, ": ");
        value = strtok(NULL, ": ");
        if (field && value)
            http_request_add_header(hr, field, value);
        p = strstr(p, "\r\n");
    }
}

HttpRequest *try_parse_http_request(const char *buffer) {
    HttpRequest *hr;
    const char *end = strstr(buffer, "\r\n\r\n");
    ptrdiff_t i = end - buffer;
    if (i) {
        hr = parse_from_request_line(buffer, i);
        if (hr) {
            parse_headers_into(hr, buffer);
            return hr;
        }
    }
    return NULL;
}
