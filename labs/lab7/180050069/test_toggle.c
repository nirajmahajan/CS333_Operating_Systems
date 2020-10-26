#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int ret;
	int i;
	int channel1 = 2;
	int channel2 = 1;
	int lock = 0;
	uspinlock_init();
	ucounter_init();



	ret = fork();
	if(ret == 0)
	{
		for(i=0; i < 10; i++) {
			uspinlock_acquire(lock);
			while(ucounter_get(0) == 1){
				ucv_sleep(channel1, lock);
			}
			printf(1, "I am child\n");
			ucounter_set(0, ucounter_get(0)+1);
			ucv_wakeup(channel2);
			uspinlock_release(lock);

		}
		exit();
	}
	else
	{
		for(i=0; i < 10; i++) {
			uspinlock_acquire(lock);
			while(ucounter_get(0) == 0){
				ucv_sleep(channel2, lock);
			}
			printf(1, "I am parent\n");
			ucounter_set(0, ucounter_get(0)-1);
			ucv_wakeup(channel1);
			uspinlock_release(lock);
		}
		ucv_wakeup(channel1);
		wait();
		exit();
	}
}

