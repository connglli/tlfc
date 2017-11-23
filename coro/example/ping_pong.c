#include "coro.h"
#include <stdio.h>
#include <assert.h>

#define MAX_CNT 100

#define MSG_PING 1
#define MSG_PONG 2
#define MSG_OVER 3

CORO_EX(pong, args) {
  int cnt = 0;

  coro_id_t pong_id = self();
  coro_id_t ping_id = (coro_id_t)args;

  do {
    coro_msg_t m = receive(-1); // blocked until a message received
    assert(m.from == ping_id && m.to == pong_id);

    if (m.type == MSG_OVER) {
      break;
    } else if (m.type != MSG_PONG) {
      fprintf(stderr, "error `pong received messages of type %d`\n", m.type);
      break;
    }

    fprintf(stdout, "pong: %d\n", cnt ++);

    if(send(ping_id, MSG_PING, NULL) != 0) {
      fprintf(stderr,
              "error while `pong is sending messages MSG_PING to %d`\n",
              ping_id);
      break;
    }

  } while(1);

  return NULL;
}

CORO_EX(ping, args) {
  int cnt = 0;

  coro_id_t ping_id = self();
  coro_id_t pong_id = coro(pong, (void*)ping_id);

  do {
    fprintf(stdout, "ping %d\n", cnt ++);

    if(send(pong_id, MSG_PONG, NULL) != 0) {
      fprintf(stderr,
              "error while `ping is sending messages MSG_PONG to %d`\n",
              pong_id);
      return NULL;
    }

    coro_msg_t m = receive(-1); // blocked until a message received
    assert(m.from == pong_id && m.to == ping_id);

    if (m.type != MSG_PING) {
      fprintf(stderr, "error `ping received messages of type %d`\n", m.type);
      return NULL;
    }
  } while(cnt < MAX_CNT);

  if(send(pong_id, MSG_OVER, NULL) != 0) {
    fprintf(stderr,
            "error while `ping is sending messages MSG_OVER to %d`\n",
            pong_id);
    return NULL;
  }

  return NULL;
}

int coro_main(int argc, char *argv[]) {
  coro(ping, NULL);
  return 0;
}