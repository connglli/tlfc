#ifndef __CORO_MAILBOX_H__
#define __CORO_MAILBOX_H__

#include "coro.h"
#include "list/list.h"

/**
 * coro_mail_t represents the mail used for coroutine's communication
 */
typedef struct coro_msg_t     coro_mail_t;

/**
 * coro_mailbox_t represents the mailbox of a coroutine, it is a FIFO queue
 */
typedef struct coro_mailbox_t coro_mailbox_t;

/**
 * define a coro_mail_t list
 */
list_template(coro_mail_t, list_coro_mail_t, list_coro_mail_node_t);

/**
 * coro_mailbox_t represents the mailbox of a coroutine, it is a FIFO queue
 */
struct coro_mailbox_t {
  list_coro_mail_t mails;
};

// coro_mail_t implementation
// ====

static inline
void _coro_mailbox_init(coro_mailbox_t *mb) {
  list_init(&mb->mails);
}

static inline
void _coro_mailbox_deinit(coro_mailbox_t *mb) {
  list_deinit(&mb->mails);
}

static inline
void _coro_mailbox_put_mail(coro_mailbox_t *mb, coro_mail_t m) {
  list_append(&mb->mails, m);
}

static inline
coro_mail_t _coro_mailbox_get_mail(coro_mailbox_t *mb) {
  coro_mail_t m;
  list_coro_mail_node_t *mail_node = NULL;
  
  list_getn(&mb->mails, 0, &mail_node);
  m = list_node_data(mail_node);

  list_remove(&mb->mails, 0);

  return m;
}

static inline
int _coro_mailbox_is_empty(coro_mailbox_t *mb) {
  return list_size(&mb->mails) == 0;
}

static inline
int _coro_mailbox_size(coro_mailbox_t *mb) {
  return list_size(&mb->mails);
}

#endif