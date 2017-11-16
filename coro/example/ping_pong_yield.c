#include "coro.h"
#include <stdio.h>

CORO_EX(ping, args) {
  do {
    printf("ping\n");
    yield();
  } while(1);

  return NULL;
}

CORO_EX(pong, args) {
  do {
    printf("pong\n");
    yield();
  } while(1);

  return NULL;
}

int coro_main(int argc, char* argv[]) {
  coro(ping, NULL);
  coro(pong, NULL);
  return 0;
}