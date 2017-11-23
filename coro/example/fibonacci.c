#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "coro.h"
#include "io.h"

#define MAX_INDEX      40
#define MAX_ONCE       10

#define MSG_WRITE_ONE  1
#define MSG_WRITE_OVER 2

typedef struct fib_arg_t {
  int idx;
  int fib_l32, fib_h32;
} fib_arg_t;

fib_arg_t* fib_arg_create(int idx, long long fib) {
  assert(idx >= 0 && fib >= 0);

  fib_arg_t *a = calloc(1, sizeof(fib_arg_t));

  a->idx = idx;
  a->fib_l32 = (int)(fib);
  a->fib_h32 = (int)(fib >> 32);

  return a;
}

void fib_arg_destroy(fib_arg_t *a) {
  assert(NULL != a);
  free(a);
}

long long fibonacci(int idx) {
  if (0 == idx) { return 0ll; }
  if (1 == idx) { return 1ll; }
  return fibonacci(idx - 2) + fibonacci(idx - 1);
}

// this is a io-bound coroutine
CORO_EX(fib_writer, args) {
  coro_id_t main_task_id = (coro_id_t)args;

  int fd = open("./fib.txt", O_WRONLY|O_CREAT, 00666);
  char buf[128];

  if (coro_io_set_nonblock(fd) < 0) {
    fprintf(stderr,
            "error while fib_writer is setting fd %d to O_NONBLOCK\n",
            fd);
    return NULL;
  }

  do {
    coro_msg_t m = receive(-1);

    if (MSG_WRITE_ONE == m.type) {
      fib_arg_t *a = (fib_arg_t*)m.data;

      int       idx = a->idx;
      long long fib = (a->fib_l32) | ((long long)(a->fib_h32) << 32);
      fib_arg_destroy(a);

      int len = sprintf(buf, "%d: %lld\n", idx, fib);

      if (coro_io_write(fd, buf, len) < 0) {
        fprintf(stderr, 
                "error while fib_writer is writing [%d, %lld] to fd %d\n",
                idx, fib, fd);
        close(fd);
        break;
      } else {
        fprintf(stdout, "fib_writer wrote [%d, %lld]\n", idx, fib);
      }
    } else if (MSG_WRITE_OVER == m.type) {
      close(fd);
      break;
    } else {
      close(fd);
      fprintf(stderr, 
              "error while fib_writer is receiving"
                " message of type %d from coro %d\n",
              m.type, m.from);
      break;
    }

  } while(1);

  return NULL;
}

// this is a cpu-bound coroutine
CORO_EX(main_task, args) {
  coro_id_t fib_writer_id = coro(fib_writer, (void*)self());
  
  long long fib;
  fib_arg_t *arg;

  for (int idx = 1; idx <= MAX_INDEX; idx ++) {
    fib = fibonacci(idx);
    arg = fib_arg_create(idx, fib);
    
    if (send(fib_writer_id, MSG_WRITE_ONE, (void*)arg) < 0) {
      fprintf(stderr, 
              "error while main_task is sending [%d, %lld] to fib_writer %d\n", 
              idx, fib, fib_writer_id);
      return NULL;
    } else {
      fprintf(stdout, 
              "main_task sent [%d, %lld] to fib_writer %d\n",
              idx, fib, fib_writer_id);
    }

    // cpu-bound task will never yield its cpu automatically,
    // so we yield manually
    if (0 == idx % MAX_ONCE) {
      yield();
    }
  }

  if (send(fib_writer_id, MSG_WRITE_OVER, NULL) < 0) {
    fprintf(stderr, 
            "error while main_task is sending MSG_WRITE_OVER to"
              " fib_writer %d\n",
            fib_writer_id);
    return NULL;
  } else {
    fprintf(stdout, 
            "main_task sent MSG_WRITE_OVER to fib_writer %d\n",
            fib_writer_id);    
  }

  return NULL;
}

int coro_main(int argc, char *argv[]) {
  coro(main_task, NULL);
  return 0;
}