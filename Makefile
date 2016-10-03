NAME=httpd
CC=gcc
SRC=server.c

all:
	$(CC) $(SRC) -o $(NAME)

clean:
	rm $(NAME)
