#ifndef __CORO_H__
#define __CORO_H__

/*----------------------------------------------------------------------------
                                    coroutine
  ----------------------------------------------------------------------------*/

/**
 * coro_t represents an executable coroutine
 */
typedef int coro_id_t;

/**
 * coro_ex_t is the function executed by coroutine
 */
typedef void* (*coro_ex_t)(void*);


/**
 * CORO_EX defines a coro_ex_t
 * @param  ex_name name of this coro_ex_t
 * @param  ex_args args of this coro_ex_t
 * @return         definition of this coro_ex_t
 */
#define CORO_EX(ex_name, ex_args) void* ex_name(void *ex_args)

/**
 * coro creates and queues a coroutine of ex, does not run immediately
 * @param ex   exeutable function
 * @param args args of ex
 * @return     id of this coroutine
 */
coro_id_t coro(coro_ex_t ex, void *args);

/**
 * yield yields CPU to other coroutines, just like the key word `yield` 
 * in ECMA2015+ or python.
 */
void yield();

/**
 * self returns id of this coroutine
 * @return id of this coroutine
 */
coro_id_t self();

/*----------------------------------------------------------------------------
                                    messaging
  ----------------------------------------------------------------------------*/

/**
 * coro_msg_t represents the message used for coroutine's communication
 */
typedef struct coro_msg_t {
  coro_id_t  from; // sender
  coro_id_t  to;   // receiver
  int        type; // type of this message, positive for valid ones else invalid
  void      *data; // data of this message
} coro_msg_t;

/**
 * send sends a message to cid
 * @param  cid  id of the receiver
 * @param  type message type
 * @param  data message data
 * @return      0 when succeeded else -1
 */
int send(coro_id_t cid, int type, void *data);

/**
 * receive receives a message from other coroutine: 
 *  if there is a message, then return immediately;
 *  else if timeout > 0, then blocked until there is a message within timeout;
 *  else if timeout = 0, then return immediately;
 *  else blocked until there is a message
 * @param  timeout blocked time
 * @return         message received(or the message type will be a negative number)
 */
coro_msg_t receive(long long timeout);

#endif