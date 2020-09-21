#include <pthread.h>

typedef struct zemaphore {
	pthread_mutex_t accesslock;
	pthread_cond_t go_sleepers;
	int count;
} zem_t;

void zem_init(zem_t *, int);
void zem_up(zem_t *);
void zem_down(zem_t *);
