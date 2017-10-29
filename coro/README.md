# coro

> **IMPORTANT**
> 
> 1. We say libtask/context, which means any codes that has some relations with `ucontext_t`, developed by Russ Cox in his project [libtask]()
> 2. In any amd64 structure(macOS for example), `makecontext` have to pass 2 or a greater number for the third parameter, or the program will be crashed by segment fault(caused by `*(int*)0 = 0`). See more details in `context.c`.
> 3. Field `uc_link` of `struct ucontext_t` seems to not work at least in amd64 structure(may be because `struct libthread_ucontext` is exactly the same as `struct ucontext_t` or `struct __ucontext_t`), so never merely use return whenever you want to exit from some contexts, instead, you have to manually call `swapcontext`.