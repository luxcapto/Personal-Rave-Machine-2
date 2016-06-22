#include <pthread.h>

extern char loop;

typedef struct {
	int *argc;
	char **argv;
} args_t;

typedef struct {
	int byte1; 
	int byte2; 
	int byte3; 
} effects_t;

extern args_t arguments;
extern effects_t effect;
extern pthread_mutex_t lock;
