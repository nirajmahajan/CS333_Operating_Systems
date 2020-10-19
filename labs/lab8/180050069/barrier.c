/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "spinlock.h"
#include "defs.h"
#include "barrier.h"

//define any variables needed here
int count = 0;
int N = 0;
struct spinlock lk;
int channel = 1;

int
barrier_init(int n)
{
	initlock(&lk, "Lock");
	count = 0;
	N = n;
	return 0;
}

int
barrier_check(void)
{
	acquire(&lk);
	count ++;
	if (count == N){
		wakeup((void *)&channel);
	} else{
		sleep((void *)&channel, &lk);
	}
	release(&lk);
	return 0;
}

/*----------xv6 sync lock end----------*/
