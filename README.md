# tlfc - tiny library for c

This is a tiny lib for c, implemented easily.

## content

This lib includes some data structures as well as some easy but often used functions

+ [ ] dynamic array - array.h
+ [ ] linked list   - list.h
+ [ ] pool          - pool.h
+ [ ] hash map      - map.h
+ [ ] string funcs  - strings.h
+ [ ] logger funcs  - logger.h
+ [ ] debug funcs   - debug.h

## data structures

+ array_t
+ list_t
+ pool_t
+ map_t

## api intro

The api of tlfc is all the same among these data structures:

+ `xxx_init`: create/alloc a data structure, e.g. `list_init`
+ `xxx_get`: get data/nodes of a data structure, e.g. `list_get`
+ `xxx_set`: set data/nodes of a data structure, e.g. `list_set`
+ `xxx_insert`: insert data/nodes of a data structure, e.g. `list_insert`
+ `xxx_remove`: remove data/nodes of a data structure, e.g. `list_remove`
+ `xxx_deinit`: destroy a data structure, e.g. `list_deinit`

## ref

+ [rxi/vec](https://github.com/rxi/vec)
+ [clibs/list](https://github.com/clibs/list)
+ [petewarden/c_hashmap](https://github.com/petewarden/c_hashmap)