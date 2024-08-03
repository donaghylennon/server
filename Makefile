server: main.c server.c http.c hashtable.c
	gcc -ggdb -Wall -Wextra -Werror $^ -o $@
