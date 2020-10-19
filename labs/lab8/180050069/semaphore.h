#include "spinlock.h"

/*----------xv6 sync lab----------*/
#define NSEM 10
struct semaphore
{
	int count;
	struct spinlock lk;
};
struct semaphore sems[NSEM];


int sem_init(int, int);
int sem_up(int);
int sem_down(int);
/*----------xv6 sync lab end----------*/
