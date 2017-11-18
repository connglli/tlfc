#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include "def.h"
#include "io.h"

extern void freeze();
extern void unfreeze(coro_id_t cid);

#define CORO_IO_MAX_POLL_FD  512
#define CORO_IO_POLL_TIMEOUT 30

static int           _g_coro_io_supervisor_started;
static int           _g_coro_io_max_pollfd_idx;
static struct pollfd _g_coro_io_pollfds[CORO_IO_MAX_POLL_FD];
static coro_id_t     _g_coro_io_coroids[CORO_IO_MAX_POLL_FD];

// static functions
// ====

// CORO_IO_ASSERT_FLAG assert fd has flag
#define CORO_IO_ASSERT_FLAG(fd, flag)                                          \
  do {                                                                         \
    int flags = fcntl((fd), F_GETFL);                                          \
    assert((flags & (flag)) != 0);                                             \
  } while(0)

// _coro_io_supervisor is the supervisor of all io-bound coroutines
static
void* _coro_io_supervisor(void *args) {
  while(1) {
    // no more polling
    if (_g_coro_io_max_pollfd_idx < 0) {
      break;
    }
    
    int ret = poll(
      _g_coro_io_pollfds, 
      _g_coro_io_max_pollfd_idx + 1,
      CORO_IO_POLL_TIMEOUT
    );

    // error generated
    if (ret < 0) { assert(0); }
    // timeout, yield and let others run, then continue
    else if (ret == 0) { yield(); continue; }

    // find available coroutine, and unfreeze it
    for (int i = 0; i <= _g_coro_io_max_pollfd_idx; i ++) {
      // nothing happended
      if (!(_g_coro_io_pollfds[i].revents & _g_coro_io_pollfds[i].events)) {
        continue;
      }

      // unpoll this fd, we just move the last one here
      _g_coro_io_pollfds[i].fd =
        _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].fd;
      _g_coro_io_pollfds[i].events =
        _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].events;
      _g_coro_io_pollfds[i].revents =
        _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].revents;
      _g_coro_io_max_pollfd_idx --;

      // unfreeze the coroutine
      unfreeze(_g_coro_io_coroids[i]);
    }
  }

  return NULL;
}

// _coro_io_wait creates a supervisor to handle io operations, 
// and yields cpu to other cpu-bound coroutines
static
void _coro_io_wait(int fd, int event) {
  if (!_g_coro_io_supervisor_started) {
    coro(_coro_io_supervisor, NULL);
    _g_coro_io_supervisor_started = 1;
  }

  // we assume by now that, it cannot be that, the number of fd be polled
  // is greater than CORO_IO_MAX_POLL_FD
  assert(_g_coro_io_max_pollfd_idx < CORO_IO_MAX_POLL_FD - 1);

  // add itself to pollfds
  _g_coro_io_max_pollfd_idx ++;
  _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].fd      =  fd;
  _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].events  = event;
  _g_coro_io_pollfds[_g_coro_io_max_pollfd_idx].revents = 0;
  _g_coro_io_coroids[_g_coro_io_max_pollfd_idx]         = self();

  // freeze itself, let supervisor watches fd for the coroutine itself
  freeze();
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
    _coro_io_wait(fd, POLLIN);
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
      _coro_io_wait(fd, POLLOUT);
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