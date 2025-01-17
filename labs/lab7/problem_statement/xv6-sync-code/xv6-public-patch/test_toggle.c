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
	  	while(ucounter_get(0) % 2 == 1){
	  		ucv_sleep(0,0);
	  	}
	    printf(1, "I am child\n");
	    ucounter_set(0, ucounter_get(0)+1);
	    ucv_wakeup(1);
	  	uspinlock_release(lock);
	  }
          exit();
	}
	else
	{
	  for(i=0; i < 10; i++) {
	  	uspinlock_acquire(lock);
	  	while(ucounter_get(0) % 2 == 0){
	  		ucv_sleep(1,0);
	  	}
	    printf(1, "I am parent\n");
	    ucounter_set(0, ucounter_get(0)+1);
	    ucv_wakeup(0);
	  	uspinlock_release(lock);
	  }
	  wait();
          exit();
	}
}

