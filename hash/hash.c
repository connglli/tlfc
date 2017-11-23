#include <string.h>
#include "hash.h"

/* A Simple Hash Function */
unsigned int hash_simple(const char *str, int len) {
  register unsigned int   hash = 0;
  register unsigned char *p = (unsigned char *)str;

  int i = 0;
  for(int i = 0; i < len ; i ++, p++) {
    hash = 31 * hash + *p;
  }

  return (hash & 0x7FFFFFFF);
}

/* RS Hash Function */
unsigned int hash_rs(const char *str, int len) {
  unsigned int b = 378551;
  unsigned int a = 63689;
  unsigned int hash = 0;

  for (int i = 0; i < len; i ++) {
    hash = hash * a + (*str++);
    a *= b;
  }

  return (hash & 0x7FFFFFFF);
}

/* JS Hash Function */
unsigned int hash_js(const char *str, int len) {
  unsigned int hash = 1315423911;

  for (int i = 0; i < len; i ++) {
    hash ^= ((hash << 5) + (*str++) + (hash >> 2));
  }

  return (hash & 0x7FFFFFFF);
}

/* P. J. Weinberger Hash Function */
unsigned int hash_pjw(const char *str, int len) {
  unsigned int bits_in_uint   = (unsigned int)(sizeof(unsigned int) * 8);
  unsigned int three_quarters = (unsigned int)((bits_in_uint   * 3) / 4);
  unsigned int one_eighth     = (unsigned int)(bits_in_uint / 8);

  unsigned int high_bits = (unsigned int)(0xFFFFFFFF) << (bits_in_uint - one_eighth);
  unsigned int hash      = 0;
  unsigned int test      = 0;

  for (int i = 0; i < len; i ++) {
    hash = (hash << one_eighth) + (*str++);
    if ((test = hash & high_bits) != 0) {
      hash = ((hash ^ (test >> three_quarters)) & (~high_bits));
    }
  }

  return (hash & 0x7FFFFFFF);
}

/* ELF Hash Function */
unsigned int hash_elf(const char *str, int len) {
  unsigned int hash = 0;
  unsigned int x    = 0;

  for (int i = 0; i < len; i ++) {
    hash = (hash << 4) + (*str++);
    if ((x = hash & 0xF0000000L) != 0) {
      hash ^= (x >> 24);
      hash &= ~x;
    }
  }

  return (hash & 0x7FFFFFFF);
}

/* BKDR Hash Function */
unsigned int hash_bkdr(const char *str, int len) {
  unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
  unsigned int hash = 0;

  for (int i = 0; i < len; i ++) {
    hash = hash * seed + (*str++);
  }

  return (hash & 0x7FFFFFFF);
}

/* SDBM Hash Function */
unsigned int hash_sdbm(const char *str, int len) {
  unsigned int hash = 0;

  for (int i = 0; i < len; i ++) {
    hash = (*str++) + (hash << 6) + (hash << 16) - hash;
  }

  return (hash & 0x7FFFFFFF);
}

/* DJB Hash Function */
unsigned int hash_djb(const char *str, int len) {
  unsigned int hash = 5381;

  for (int i = 0; i < len; i ++) {
    hash += (hash << 5) + (*str++);
  }


  return (hash & 0x7FFFFFFF);
}

/* AP Hash Function */
unsigned int hash_ap(const char *str, int len) {
  unsigned int hash = 0;

  for (int i = 0; i < len; i ++) {
    if ((i & 1) == 0) {
      hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
    } else {
      hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
    }
  }

  return (hash & 0x7FFFFFFF);
}

/* CRC Hash Function */
unsigned int hash_crc(const char *str, int len) {
  unsigned int        nleft  = len;
  unsigned long long  sum    = 0;
  unsigned short int *w      = (unsigned short int *)str;
  unsigned short int  answer = 0;

  // Our algorithm is simple, using a 32 bit accumulator (sum), we add
  // sequential 16 bit words to it, and at the end, fold back all the
  // carry bits from the top 16 bits into the lower 16 bits.
  while ( nleft > 1 ) {
    sum += *w++;
    nleft -= 2;
  }

  // mop up an odd byte, if necessary
  if ( 1 == nleft ) {
    *( unsigned char * )( &answer ) = *( unsigned char * )w ;
    sum += answer;
  }

  // add back carry outs from top 16 bits to low 16 bits
  // add hi 16 to low 16
  sum = ( sum >> 16 ) + ( sum & 0xFFFF );

  // add carry
  sum += ( sum >> 16 );

  // truncate to 16 bits
  answer = ~sum;

  return (answer & 0xFFFFFFFF);
}

