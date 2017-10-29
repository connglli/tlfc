#include "coro.h"
#include "coro_def.h"

extern int  coro_main(int argc, char *argv[]);
extern void coro_scheduler_init(coro_scheduler_t *scheduler);
extern void coro_scheduler_run(coro_scheduler_t *scheduler);
extern void coro_scheduler_deinit(coro_scheduler_t *scheduler);

// global variables that will be passed to coro_main
static int    _g_coro_main_argc;
static char **_g_coro_main_argv;

// global scheduler
static coro_scheduler_t  _g_default_scheduler;
static coro_scheduler_t *_g_scheduler = &_g_default_scheduler;

// main coro
static
void* _coro_main(void *args);

coro_scheduler_t* coro_global_scheduler() {
	return _g_scheduler;
}

int main(int argc, char *argv[]) {
	_g_coro_main_argc = argc;
	_g_coro_main_argv = (char**)argv;

	coro_scheduler_init(_g_scheduler);
	coro(_coro_main, NULL);
	coro_scheduler_run(_g_scheduler);
	coro_scheduler_deinit(_g_scheduler);

	return 0;
}

static
void* _coro_main(void *args) {
	coro_main(_g_coro_main_argc, _g_coro_main_argv);
	return NULL;
}
