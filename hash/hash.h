#ifndef __HASH_H__
#define __HASH_H__

/**
 * hash_f_t represents a hash function
 */
typedef unsigned int (*hash_f_t)(const char *str, int len);

unsigned int hash_simple(const char *str, int len);
unsigned int hash_rs(const char *str, int len);
unsigned int hash_js(const char *str, int len);
unsigned int hash_pjw(const char *str, int len);
unsigned int hash_elf(const char *str, int len);
unsigned int hash_bkdr(const char *str, int len);
unsigned int hash_sdbm(const char *str, int len);
unsigned int hash_djb(const char *str, int len);
unsigned int hash_ap(const char *str, int len);
unsigned int hash_crc(const char *str, int len);

#endif