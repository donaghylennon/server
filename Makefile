server: main.c server.c http.c hashtable.c
	gcc -Wall -Wextra -Werror $^ -o $@
