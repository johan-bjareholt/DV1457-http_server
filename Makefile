NAME=httpd
CC=gcc
CFLAGS=-g
SRC=server.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	rm $(NAME)
