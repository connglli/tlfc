CC =  gcc
CFLAGS = -Wall -I./src -I./lib

SRC_FILES = $(shell find . -name "*.[Sc]" ! -path "./example/*")
OBJ_FILES = $(SRC_FILES:%.o=.c)

all: ping_pong

ping_pong: $(OBJ_FILES) ./example/ping_pong.o
	$(CC) -o ping_pong $(OBJ_FILES) ./example/ping_pong.o $(CFLAGS)

clean:
	@rm -f ./example/*.o ping_pong