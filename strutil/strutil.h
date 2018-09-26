#ifndef __STRUTIL_H__
#define __STRUTIL_H__

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * strdup duplicates a string from s
 * @param  s string you want to be duplicated
 * @return   a duplicate of s, remember to free it
 */
char* strdup(const char* s);

/**
 * strndup duplicates a first n characters from s
 * @param  s string you want to be duplicated
 * @param  n number of characters you want to be duplicated
 * @return   a duplicate of s, remember to free it
 */
char* strndup(const char* s, size_t l);

/**
 * strrpt returns a string who repeats s for n times
 * @param  s string you want to be repeated
 * @param  n times to repeat
 * @return   a repeated string of s, remember to free it
 */
char* strrpt(const char* s, int n);

/**
 * strsts checks whether s starts with ss
 * @param  s  string
 * @param  ss starts to be checked
 * @return    1 if true else 0
 */
int strsts(const char* s, const char* ss);

/**
 * streds checks whether s ends with ss
 * @param  s  string
 * @param  ss ends to be checked
 * @return    1 if true else 0
 */
int streds(const char* s, const char* ss);

/**
 * strrvs revers a string
 * @param  s string to be reversed
 * @return   0 if succeeded else -1
 */
int strrvs(char* s);

/**
 * strtriml trims left spaces of a string, using isspace(defined in ctype.h)
 * @param  s string to be trimed left, s will be rewritten to trimed s
 * @return   trimed string
 */
char* strtriml(char* s);

/**
 * strtrim trims right spaces of a string, using isspace(defined in ctype.h)
 * @param  s string to be trimed rignt, s will be rewritten to trimed s
 * @return   trimed string
 */
char* strtrimr(char* s);

/**
 * strtrim trims spaces of a string, using isspace(defined in ctype.h)
 * @param  s string to be trimed, s will be rewritten to trimed s
 * @return   trimed string
 */
char* strtrim(char* s);

/**
 * strsplit splits a string according to characters in delim
 * @param  s     string to be split
 * @param  delim split character set
 * @return       tokens after splitting, terminated by NULL, 
 *               remember to free them all one by one
 */
char** strsplit(const char* s, const char* delim);

/**
 * strnsplit splits a string according to characters in delim
 * @param  s     string to be split
 * @param  delim split character set
 * @param  n     size number of tokens
 * @return       tokens after splitting, n pointed out size of tokens, 
 *               remember to free them all one by one
 */
char** strnsplit(const char* s, const char* delim, size_t* const n);

/**
 * strjoin joins all strings in tokens using delim
 * @param  tokens strings to be joined, terminated by NULL
 * @param  delim  delimeter used
 * @return        a joined string, remember to free it
 */
char* strjoin(char** tokens, const char* delim);

/**
 * strnjoin joins all strings in tokens using delim
 * @param  tokens strings to be joined, n points out its size
 * @param  n      size of tokens
 * @param  delim  delimeter used
 * @return        a joined string, remember to free it
 */
char* strnjoin(char** tokens, size_t n, const char* delim);

/**
 * stridx finds the first occurring index of p in t. It is implemented via KMP.
 * @param  t the main string
 * @param  p the pattern string
 * @return   the first occurring index of p in t, or -1
 */
int stridx(const char* t, const char* p);

#endif