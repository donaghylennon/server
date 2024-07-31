#include <stdio.h>

#include "server.h"

void handler_func(const char *path) {
    fprintf(stderr, "path is %s\n", path);
}

int main() {
    server_register_path_handler("/", handler_func);
    server_run(8080);

    return 0;
}
