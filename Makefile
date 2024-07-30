server: main.c http.c hashtable.c
	gcc -Wall -Wextra -Werror $^ -o $@
