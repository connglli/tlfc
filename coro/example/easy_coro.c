#include "coro.h"
#include <stdio.h>

void* coro1(void* args) {
  for (int i = 0; i < 10; ++i) {
    printf("coro1: %d\n", i);
  }

  yield();

  for (int i = 10; i < 20; ++i) {
    printf("coro1: %d\n", i);
  }

  return NULL;
}

void* coro2(void* args) {
  for (int i = 0; i < 5; ++i) {
    printf("coro2: %d\n", i);
  }

  yield();

  for (int i = 5; i < 10; ++i) {
    printf("coro2: %d\n", i);
  }

  return NULL;
}

void* coro3(void* args) {
  for (int i = 0; i < 10; ++i) {
    printf("coro3: %d\n", i);
  }

  return NULL;
}

int coro_main(int argc, char* argv[]) {
  coro(coro1, NULL);
  coro(coro2, NULL);
  coro(coro3, NULL);

  return 0;
}