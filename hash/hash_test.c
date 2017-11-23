#include <stdio.h>
#include <string.h>
#include "hash.h"

typedef struct test_struct {
  char buf[16];
  int  x;
} test_struct;

int main() {
  for (int i = 0; i < 10; i ++) {
    test_struct o = { {'\0'}, i };
    sprintf(o.buf, "test");
    sprintf(o.buf + strlen(o.buf) + 1, "\n%d", i);
    fprintf(stdout, "== %d\n", i);
    fprintf(stdout, "%08x\n", hash_simple((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_rs((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_js((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_pjw((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_elf((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_bkdr((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_sdbm((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_djb((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_ap((const char*)(void*)&o, sizeof(o)));
    fprintf(stdout, "%08x\n", hash_crc((const char*)(void*)&o, sizeof(o)));
  }
}