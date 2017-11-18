#include "coro.h"
#include <stdio.h>
#include <sys/time.h>

#define MAX_INDEX 43
#define MAX_ONCE  2

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

  return NULL;
}

CORO_EX(delayed_task_1, arg) {
  struct timeval tv;
  int            ret;

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr, 
            "error in delayed_task_1 gettimeofday, error code is %d\n", 
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_1 delay for 1s start at unix time %ld\n",
            tv.tv_sec);
  }

  delay(1000);

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr,
            "error in delayed_task_1 gettimeofday, error code is %d\n",
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_1 delay for 1s end at unix time %ld\n",
            tv.tv_sec);
  }

  return NULL;
}

CORO_EX(delayed_task_2, arg) {
  struct timeval tv;
  int            ret;

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr, 
            "error in delayed_task_2 gettimeofday, error code is %d\n", 
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_2 delay for 3s start at unix time %ld\n",
            tv.tv_sec);
  }

  delay(3000);

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr,
            "error in delayed_task_2 gettimeofday, error code is %d\n",
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_2 delay for 3s end at unix time %ld\n",
            tv.tv_sec);
  }

  return NULL;
}

CORO_EX(delayed_task_3, arg) {
  struct timeval tv;
  int            ret;

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr, 
            "error in delayed_task_3 gettimeofday, error code is %d\n", 
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_3 delay for 5s start at unix time %ld\n", tv.tv_sec);
  }

  delay(5000);

  ret = gettimeofday(&tv, 0);
  if (0 != ret) {
    fprintf(stderr,
            "error in delayed_task_3 gettimeofday, error code is %d\n",
            ret);
  } else{
    fprintf(stdout, 
            "delayed_task_3 delay for 5s end at unix time %ld\n", tv.tv_sec);
  }

  return NULL;
}

int coro_main(int argc, char *argv[]) {
  coro(delayed_task_1, NULL);
  coro(delayed_task_2, NULL);
  coro(delayed_task_3, NULL);
  coro(cpu_task, NULL);
  return 0;
}