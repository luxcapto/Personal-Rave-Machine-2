#include <pthread.h>

extern char loop;

typedef struct {
	int *argc;
	char **argv;
} args_t;

typedef struct {
	int effect; 
} effects_t;

extern args_t arguments;
extern effects_t effects;
extern pthread_mutex_t lock;
