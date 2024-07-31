#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>

struct path_handler_pair {
    char path[200];
    void (*handler)(const char *);
};

int server_listen(unsigned short port);
int server_receive_message(int fd, char * buffer, size_t buflen);
void server_run(unsigned short port);
void server_register_path_handler(const char *path, void (*)(const char *));

#endif
