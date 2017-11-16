#include "coro.h"
#include <stdio.h>
#include <assert.h>

#define MSG_PING 1
#define MSG_PONG 2

CORO_EX(pong, args) {
  int cnt = 0;

  coro_id_t pong_id = self();
  coro_id_t ping_id = (coro_id_t)args;

  do {
    coro_msg_t m = receive(-1); // blocked until a message received
    assert(m.from == ping_id && m.to == pong_id);

    if (m.type != MSG_PONG) {
      printf("error `pong received messages of type %d`\n", m.type);
      return NULL;
    }

    printf("pong: %d\n", cnt ++);

    if(send(ping_id, MSG_PING, NULL) != 0) {
      printf("error when `pong sends messages to %d`\n", ping_id);
      return NULL;
    }

  } while(1);

  return NULL;
}

CORO_EX(ping, args) {
  int cnt = 0;

  coro_id_t ping_id = self();
  coro_id_t pong_id = coro(pong, (void*)ping_id);

  do {
    printf("ping %d\n", cnt ++);

    if(send(pong_id, MSG_PONG, NULL) != 0) {
      printf("error when `ping sends messages to %d`\n", pong_id);
      return NULL;
    }

    coro_msg_t m = receive(-1); // blocked until a message received
    assert(m.from == pong_id && m.to == ping_id);

    if (m.type != MSG_PING) {
      printf("error `ping received messages of type %d`\n", m.type);
      return NULL;
    }
  } while(1);

  return NULL;
}

int coro_main(int argc, char *argv[]) {
  coro(ping, NULL);
  return 0;
}