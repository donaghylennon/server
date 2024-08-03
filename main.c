#include <stdio.h>

#include "server.h"

const char *handler_func(const char *path) {
    fprintf(stderr, "path is %s\n", path);

    FILE *f = fopen("index.html", "rb");
    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);

    char *buffer = (char *)malloc(fsize*sizeof(char));
    fread(buffer, fsize, 1, f);
    fclose(f);
    buffer[fsize] = '\0';

    return buffer;
}

int main() {
    server_register_path_handler("/", handler_func);
    server_run(8080);

    return 0;
}
