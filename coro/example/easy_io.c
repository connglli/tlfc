#include "coro.h"
#include "io.h"
#include <stdio.h>
#include <fcntl.h>

#define MAX_INDEX 45
#define MAX_ONCE  10

long long fibonacci(int idx) {
  if (0 == idx) { return 0ll; }
  if (1 == idx) { return 1ll; }
  return fibonacci(idx - 2) + fibonacci(idx - 1);
}

CORO_EX(cpu_task, arg) {
  long long fib;

  for (int idx = 1; idx <= MAX_INDEX; idx ++) {
    fib = fibonacci(idx);
    fprintf(stdout, "cpu_task compute [%d, %lld]\n", idx, fib);

    // cpu-bound task will never yield its cpu automatically,
    // so we yield manually
    if (0 == idx % MAX_ONCE) {
      yield();
    }
  }

  fprintf(stdout, "cpu_task done\n");

  return NULL;
}

#define BUF_SIZE 2048

#define SRC "/Users/apple/Downloads/01Introduction.pdf"
#define DST "./01Introduction.pdf"

CORO_EX(io_task, args) {
  int fd_r = open(SRC, O_RDONLY);
  int fd_w = open(DST, O_WRONLY|O_CREAT, 00666);

  int  nbytes = 0;
  char buf[BUF_SIZE];

  if (coro_io_set_nonblock(fd_r) < 0) {
    fprintf(stderr,
            "error while io_task is setting fd %d to O_NONBLOCK\n",
            fd_r);
    return NULL;
  }

  if (coro_io_set_nonblock(fd_w) < 0) {
    fprintf(stderr,
            "error while io_task is setting fd %d to O_NONBLOCK\n",
            fd_w);
    return NULL;
  }

  while(1) {
    nbytes = coro_io_read(fd_r, buf, BUF_SIZE);
    
    if (0 == nbytes) { break; }
    else if(0 > nbytes) {
      fprintf(stderr, "error while io_task is reading from fd %d\n", fd_r);
      break;
    }

    nbytes = coro_io_write(fd_w, buf, nbytes);
    if(0 > nbytes) {
      fprintf(stderr, "error while io_task is writing to fd %d\n", fd_w);
      break;
    }
  }

  close(fd_r);
  close(fd_w);

  fprintf(stdout, "io_task done\n");

  return NULL;
}

int coro_main(int argc, char *argv[]) {
  coro(io_task, NULL);
  coro(cpu_task, NULL);
  return 0;
}