CC =  gcc
CFLAGS = -Wall -I./src -I./lib

SRC_FILES = $(shell find . -name "*.[Sc]" ! -path "./example/*")
OBJ_FILES = $(SRC_FILES:%.o=.c)

all: ping_pong_yield ping_pong_msg

ping_pong_yield: $(OBJ_FILES) ./example/ping_pong_yield.o
	$(CC) -o ping_pong_yield $(OBJ_FILES) ./example/ping_pong_yield.o $(CFLAGS)

ping_pong_msg: $(OBJ_FILES) ./example/ping_pong_msg.o
	$(CC) -o ping_pong_msg $(OBJ_FILES) ./example/ping_pong_msg.o $(CFLAGS)

clean:
	@rm -f ./example/*.o ping_pong_yield ping_pong_msg