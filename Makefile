NAME=httpd
CC=gcc
CFLAGS=-g -Iinclude
OBJ=obj/response_handler.o obj/request_handler.o obj/connection_handler.o obj/main.o obj/config_parser.o

all: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

obj/%.o: src/%.c prepare
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj
	rm -f $(NAME)

.PHONY: prepare
prepare:
	mkdir -p obj

