#include <stdio.h>
#include <pthread.h>

long long sum = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *adder(void* args){
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < 1000; i++){
		sum += 1;
	}
	pthread_mutex_unlock(&	mutex);
}

int main(int argc, char const *argv[])
{
	int n_threads = 10;
	pthread_t trs[n_threads];
	for (int i = 0; i < n_threads; i++){
		pthread_create(&trs[i], NULL, adder, NULL);
	}
	for (int i = 0; i < n_threads; i++){
		pthread_join(trs[i], NULL);
	}
	printf("%lld\n", sum);
	return 0;
}