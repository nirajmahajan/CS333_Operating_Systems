/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "defs.h"
#include "semaphore.h"

int
sem_init(int index, int val)
{
	initlock(&sems[index].lk,"Lock");
	sems[index].count = val;
	return 0;
}

int
sem_up(int index)
{
	acquire(&sems[index].lk);
	sems[index].count += 1;
	int channel = index + 1;
	wakeup_only((void *)channel);
	release(&sems[index].lk);
	return 0;
}

int
sem_down(int index)
{
	acquire(&sems[index].lk);
	while(sems[index].count <= 0){
		int channel = index + 1;
		sleep((void *)channel, &sems[index].lk);
	}
	sems[index].count -= 1;
	release(&sems[index].lk);
	return 0;
}

/*----------xv6 sync lab end----------*/
