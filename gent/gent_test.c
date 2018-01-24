#include <stdio.h>
#include "gent.h"

#define MAX_INDEX 30

long fibonacci(int index) {
  if (0 == index || 1 == index) {
    return 1;
  }

  return fibonacci(index - 1) + fibonacci(index - 2);
}

GENT(long, fib_gen, int max_index) {
  GENT_CONTEXT_BEGIN;
  int index;
  GENT_CONTEXT_END(ctx);

  GENT_EX_BEGIN(ctx);
  for(ctx->index = 0; ctx->index < max_index; ctx->index ++) {
    GENT_YIELD(fibonacci(ctx->index));
  }
  GENT_EX_END;

  GENT_RETURN(-1);
}

int main(int argc, char* argv[]) {
  gent_handler_t hdlr;
  GENT_HANDLER_INIT(&hdlr);

  int x;

  x = fib_gen(&hdlr, MAX_INDEX);
  if (-1 == x) {
    return 0;
  }

  do {
    printf("%d\n", x);
    x = fib_gen(&hdlr, MAX_INDEX);
  } while(-1 != x);

  return 0;
}
