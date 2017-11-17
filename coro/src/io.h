#ifndef __CORO_IO_H__
#define __CORO_IO_H__

#include <sys/types.h> // ssize_t

/**
 * coro_io_set_nonblock makes fd non-blocking
 * @param  fd file descriptor
 * @return    -1 if there are errors
 */
int coro_io_set_nonblock(int fd);

/**
 * coro_io_read read nbyte bytes data from fd to buf non-blockingly, 
 * during which, cpu will be automatically yielded
 * @param  fd    file descriptor
 * @param  buf   buffer to put data
 * @param  nbyte bytes to read
 * @return       bytes that is read
 */
ssize_t coro_io_read(int fd, void *buf, size_t nbyte);

/**
 * coro_io_write writes nbyte bytes data from buf to fd non-blockingly,
 * during which, cpu will be automatically yielded
 * @param  fd    file descriptor
 * @param  buf   buffer where data is put
 * @param  nbyte bytes to write
 * @return       bytes that is written
 */
ssize_t coro_io_write(int fd, const void *buf, size_t nbyte);

#endif