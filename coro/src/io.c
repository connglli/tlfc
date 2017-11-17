#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "def.h"
#include "io.h"

// static functions
// ====

#define CORO_IO_ASSERT_FLAG(fd, flag)                                          \
  do {                                                                         \
    int flags = fcntl((fd), F_GETFL);                                          \
    assert((flags & (flag)) != 0);                                             \
  } while(0)

// _coro_io_wait yields cpu to other coroutines
static inline
void _coro_io_wait() {
  yield();
}

// coro io implementation
// ====

/**
 * coro_io_set_nonblock makes fd non-blocking
 * @param  fd file descriptor
 * @return    -1 if there are errors
 */
int coro_io_set_nonblock(int fd) {
  int flags = fcntl(fd, F_GETFL);

  // error
  if (flags < 0) { return flags; }

  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/**
 * coro_io_read read nbyte bytes data from fd to buf non-blockingly, 
 * during which, cpu will be automatically yielded
 * @param  fd    file descriptor
 * @param  buf   buffer to put data
 * @param  nbyte bytes to read
 * @return       bytes that is read
 */
ssize_t coro_io_read(int fd, void *buf, size_t nbyte) {
  CORO_IO_ASSERT_FLAG(fd, O_NONBLOCK);

  ssize_t n_read;
  while((n_read = read(fd, buf, nbyte)) < 0 && 
        (errno == EAGAIN || (errno == EWOULDBLOCK))) {
    _coro_io_wait();
  }

  return n_read;
}

/**
 * coro_io_write writes nbyte bytes data from buf to fd non-blockingly,
 * during which, cpu will be automatically yielded
 * @param  fd    file descriptor
 * @param  buf   buffer where data is put
 * @param  nbyte bytes to write
 * @return       bytes that is written
 */
ssize_t coro_io_write(int fd, const void *buf, size_t nbyte) {
  CORO_IO_ASSERT_FLAG(fd, O_NONBLOCK);

  ssize_t n_write = 0, n_write_once = 0;

  while (n_write < nbyte) {
    // data not ready
    while((n_write_once = write(fd, buf + n_write, nbyte) < 0) &&
      (errno == EAGAIN || (errno == EWOULDBLOCK))) {
      _coro_io_wait();
    }

    // not `data not ready`, i.e. error generated
    if (n_write_once < 0) { return n_write_once; }

    // write done
    if (n_write_once == 0) { break; }

    // continue writing
    n_write += n_write_once;
  }

  return n_write;
}