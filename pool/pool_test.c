#include "pool.h"

typedef int connection_t;

pool_template(connection_t, pool_connection_t);

int main() {
  connection_t c = 1024;
  pool_fish_t f, f1, f2;
  pool_connection_t p;
  pool_init(&p, 20);

  assert(pool_capacity(&p) == 20);
  assert(p.alive_head == -1 && p.alive_tail == -1 && p.alive_size == 0);
  assert(p.dead_head == 0 && pool_dead_size(&p) == 20);
  for(int i = 1; i < 19; i ++) {
    assert(pool_is_fish_in_pool(&p, i));
    assert(pool_fish_state(&p, i) == POOL_FISH_STATE_DEAD);
    assert(!pool_is_fish_alive(&p, i));
    assert(p.fishes[i].next == i + 1);
    assert(p.fishes[i].prev == i - 1);
  }
  assert(p.fishes[0].prev == -1);
  assert(p.fishes[0].next == 1);
  assert(p.fishes[19].prev == 18);
  assert(p.fishes[19].next == -1);

  c = 1024;
  pool_fish(&p, &f);
  assert(f == 0);
  assert(!pool_is_fish_alive(&p, f));
  assert(pool_fish_state(&p, f) == POOL_FISH_STATE_ZOMBIE);
  assert(p.alive_head == -1 && p.alive_tail == -1 && p.alive_size == 0);
  assert(p.dead_head == 1 && p.dead_size == 19);
  assert(pool_capacity(&p) == 20);

  pool_revive(&p, f, c);
  assert(pool_get(&p, 0) == 1024);
  assert(pool_is_fish_alive(&p, f));
  assert(pool_fish_state(&p, f) == POOL_FISH_STATE_ALIVE);
  assert((p.fishes[f].data = c) && (p.fishes[f].prev = -1) && (p.fishes[f].next = -1));
  assert(p.alive_head == 0 && p.alive_tail == 0 && p.alive_size == 1);
  assert(p.dead_head == 1 && p.dead_size == 19);
  assert(pool_capacity(&p) == 20);

  c = 2024;
  pool_set(&p, 0, 2024);
  assert(pool_get(&p, 0) == 2024);

  c = 2048;
  pool_fish(&p, &f);
  assert(f == 1);
  assert(!pool_is_fish_alive(&p, f));
  assert(pool_fish_state(&p, f) == POOL_FISH_STATE_ZOMBIE);
  assert(p.alive_head == 0 && p.alive_tail == 0 && p.alive_size == 1);
  assert(p.dead_head == 2 && p.dead_size == 18);
  assert(pool_capacity(&p) == 20);

  pool_revive(&p, f, c);
  assert(pool_is_fish_alive(&p, f));
  assert(pool_fish_state(&p, f) == POOL_FISH_STATE_ALIVE);
  assert((p.fishes[f].data == c) && (p.fishes[f].prev == 0) && (p.fishes[f].next == -1));
  assert(p.alive_head == 0 && p.alive_tail == 1 && p.alive_size == 2);
  assert(p.dead_head == 2 && p.dead_size == 18);
  assert(pool_capacity(&p) == 20);

  pool_fish(&p, &f1);
  pool_fish(&p, &f2);

  assert(f1 == 2);
  assert(!pool_is_fish_alive(&p, f1));
  assert(pool_fish_state(&p, f1) == POOL_FISH_STATE_ZOMBIE);
  assert(p.alive_head == 0 && p.alive_tail == 1 && p.alive_size == 2);
  assert(p.dead_head == 4 && p.dead_size == 16);
  assert(pool_capacity(&p) == 20);

  assert(f2 == 3);
  assert(!pool_is_fish_alive(&p, f2));
  assert(pool_fish_state(&p, f2) == POOL_FISH_STATE_ZOMBIE);
  assert(p.alive_head == 0 && p.alive_tail == 1 && p.alive_size == 2);
  assert(p.dead_head == 4 && p.dead_size == 16);
  assert(pool_capacity(&p) == 20);

  pool_kill(&p, f2);
  assert(!pool_is_fish_alive(&p, f2));
  assert(pool_fish_state(&p, f2) == POOL_FISH_STATE_DEAD);
  assert((p.fishes[f2].prev == -1) && (p.fishes[f2].next == 4));
  assert(p.alive_head == 0 && p.alive_tail == 1 && p.alive_size == 2);
  assert(p.dead_head == 3 && p.dead_size == 17);
  assert(pool_capacity(&p) == 20);

  pool_kill(&p, f);
  assert(!pool_is_fish_alive(&p, f));
  assert(pool_fish_state(&p, f) == POOL_FISH_STATE_DEAD);
  assert((p.fishes[f].prev == -1) && (p.fishes[f].next == 3));
  assert(p.alive_head == 0 && p.alive_tail == 0 && p.alive_size == 1);
  assert(p.dead_head == 1 && p.dead_size == 18);
  assert(pool_capacity(&p) == 20);

  pool_expand_to_cap(&p, 23);
  assert(pool_capacity(&p) == 23);
  assert(p.alive_head == 0 && p.alive_tail == 0 && p.alive_size == 1);
  assert(p.dead_head == 20 && p.dead_size == 21);
  assert((p.fishes[20].prev == -1) && (p.fishes[20].next == 21));
  assert((p.fishes[21].prev == 20) && (p.fishes[21].next == 22));
  assert((p.fishes[22].prev == 21) && (p.fishes[22].next == 1));

  pool_deinit(&p);

  return 0;
}