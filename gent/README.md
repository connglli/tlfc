# gent

gent is an easy implementation of generator (like the generator in python etc.) for C using [Duff's device](https://en.wikipedia.org/wiki/Duff%27s_device).

## Introduction

Every generator has and only has two blocks.

+ #### context block 

Here you defined your variables that will be used in the ex block. Every variable (e.g. `t`) defined can be accessed by `ctx->t`, where `ctx` is the name of the context you declared.

  ``` c
GENT_CONTEXT_BEGIN;
...
GENT_COTNEXT_END(ctx);
  ```

+ #### ex block

Here you write your generator logic using api of gent. and `ctx` is the context name you declared in above context. And here you can access the variables you defined in it.

``` c
GENT_EX_BEGIN(ctx);
...
GENT_EX_END;
```

## Attention

Every generator you defined using the api `GENT` is separated into three part by the tow blocks:

```c
GENT(long, fib_gen, int max_index) {
  // the first part: context block
  GENT_CONTEXT_BEGIN;
  int index;
  GENT_CONTEXT_END(ctx);

  // the second part: ex block
  GENT_EX_BEGIN(ctx);
  for(ctx->index = 0; ctx->index < max_index; ctx->index ++) {
    GENT_YIELD(fibonacci(ctx->index));
  }
  GENT_EX_END;

  // the third part: return value, you have to call it to do some finishing work whenever you want to terminates the generator, even if it is not in the ex block like the following example
  GENT_RETURN(-1);
}
```

