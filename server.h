#ifndef SERVER_H
#define SERVER_H

#include "http.h"
#include <stdlib.h>

struct path_handler_pair {
    char path[200];
    const char *(*handler)(const char *);
};

int server_listen(unsigned short port);
void server_run(unsigned short port);
void server_register_path_handler(const char *path, const char *(*handler)(const char *));
HttpRequest *server_receive_request(int fd);
HttpResponse *server_handle_request(HttpRequest *hr);
void server_send_response(int fd, const char *response);

#endif
