all: coro_test

coro_test:
	gcc -o coro_test asm.S context.c list.c coro.c coro_test.c main.c -I. -Wall

clean:
	@rm -f coro_test