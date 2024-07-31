#include <stdbool.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "server.h"
#include "http.h"

static struct path_handler_pair handlers[200];
static size_t handler_count = 0;

void server_run(unsigned short port) {
    int fd = server_listen(port);

    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_address_length;

        int connfd = accept(fd, (struct sockaddr *)&client_address, &client_address_length);
        if (connfd == -1) {
            fprintf(stderr, "Failed connection, skipping\n");
            continue;
        }
        fprintf(stdout, "Successful connection to %s\n", inet_ntoa(client_address.sin_addr));

        // For now, just assume one receive call gets everything and print it
        // Then send a "Not Implemented" response
        char rcbuf[1500];
        int rcvd = recv(connfd, rcbuf, 1500, 0);
        rcbuf[rcvd] = '\0';
        puts("Received:\n"); puts(rcbuf); puts("\n");
        fflush(stdout);

        HttpRequest *hr = try_parse_http_request(rcbuf);
        for (size_t i = 0; i < handler_count; i++) {
            if (!strcmp(handlers[i].path, hr->path)) {
                handlers[i].handler(hr->path);
            }
        }
        char reason[200] = "Not Implemented";
        HttpResponse *hrs = create_http_response("1.1", "501", reason);
        printf("Res: %s\n", generate_response_text(hrs));

        const char *buf = generate_response_text(hrs);
        int buflen = strlen(buf);
        int sentlen = 0;
        while (sentlen < buflen) {
            int sent = send(connfd, buf, buflen, MSG_NOSIGNAL);
            if (sent == -1) {
                fprintf(stderr, "Error sending data\n");
                perror("server");
                fflush(stdout);
                break;
            }
            sentlen += sent;
            printf("sent total %i\n", sentlen);
            fflush(stdout);
        }
        break;
    }
}

int server_listen(unsigned short port) {
    int backlog = 10;
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        fprintf(stderr, "Error connecting to socket\n");
        exit(1);
    }

    if (bind(fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Error binding to port: %i\n", port);
        exit(1);
    }

    if (listen(fd, backlog) == -1) {
        fprintf(stderr, "Error listening on port: %i\n", port);
        exit(1);
    }

    return fd;
}

int server_receive_message(int fd, char * buffer, size_t buflen) {
//server recv request
    ssize_t rcount = 0;
    ssize_t total_received = 0;
    int flags = MSG_DONTWAIT;
    bool stop = false;

    while (!stop) {
        rcount = recv(fd, buffer, buflen, flags);
        if (rcount == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;
            else {
                fprintf(stderr, "Error receiving message from client\n");
                return -1;
            }
        }
        total_received += rcount;
    }
    return 0;
}

void server_register_path_handler(const char *path, void (*handler)(const char *)) {
    strcpy(handlers[handler_count].path, path);
    handlers[handler_count].handler = handler;
    handler_count++;
}
