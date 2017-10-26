# tlfc - tiny library for c

This is a tiny lib for c, implemented easily.

## content

This lib includes some data structures as well as some easy but often used functions

+ [ ] dynamic array - array.h
+ [x] linked list   - list.h
+ [x] pool          - pool.h
+ [x] hash map      - map.h
+ [x] string funcs  - strings.h
+ [x] logger funcs  - logger.h
+ [x] debug funcs   - debug.h

## data structures

+ array_t
+ list_t
+ pool_t
+ map_t

## api intro

The api of tlfc is all the same among these data structures:

+ `make_xxx`: create/alloc and then initialize a data structure, e.g. `make_array`, usually used as `array_int_t a = make_array()`
+ `xxx_init`: initialize a data structure, e.g. `list_init`, usually used as `array_int_t a; array_init(&a)`
+ `xxx_get`: get data/nodes of a data structure, e.g. `list_get`
+ `xxx_set`: set data/nodes of a data structure, e.g. `list_set`
+ `xxx_insert`: insert data/nodes of a data structure, e.g. `list_insert`
+ `xxx_remove`: remove data/nodes of a data structure, e.g. `list_remove`
+ `xxx_deinit`: destroy a data structure, e.g. `list_deinit`

In almost all api, if your access is invalid, the program will crash.

## ref

+ [rxi/vec](https://github.com/rxi/vec)
+ [clibs/list](https://github.com/clibs/list)
+ [petewarden/c_hashmap](https://github.com/petewarden/c_hashmap)