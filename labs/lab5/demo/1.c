#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <assert.h>

int sum = 0;
pthread_mutex_t mutex;

void* adder(void * args){
	pthread_mutex_lock(&mutex);
	for(int i = 0; i < 10; i++) {
		sum += 1;
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	int N = 10;

	pthread_t trs[N];

	for (int i = 0; i < N; i++) {
		pthread_create(&trs[i], NULL, adder, NULL);
	}

	for (int i = 0; i < N; i++) {
		pthread_join(trs[i], NULL);
	}

	printf("%d\n", sum);
}