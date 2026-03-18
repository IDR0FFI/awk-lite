CC = gcc
CFLAGS = -Wall -Wextra -g

main: main.c main.h
	$(CC) $(CFLAGS) -o main main.c

clean:
	rm -f main
.PHONY: clean
