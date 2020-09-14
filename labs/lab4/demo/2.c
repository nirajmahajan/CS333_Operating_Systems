#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <assert.h>

int to_print = 0;
pthread_mutex_t mutex;
pthread_cond_t *cvs;

void* printer(void * args){
	int id = * (int *)args;
	while(1){
		pthread_mutex_lock(&mutex);
		while (id != to_print) {
			pthread_cond_wait(&cvs[id], &mutex);

			if (id == 1000){
				pthread_mutex_unlock(&mutex);
				return NULL;
			}
		}
		printf("%d\n", id+1);
		usleep(600000);
		to_print = (to_print + 1)%10;
		pthread_cond_signal(&cvs[to_print]);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&mutex, NULL);
	int N = 10;

	cvs = (pthread_cond_t*)malloc(sizeof(*cvs) * N);

	pthread_t trs[N];
	int args[N];

	for (int i = 0; i < N; i++) {
		args[i] = i;
	}

	for (int i = 0; i < N; i++) {
		pthread_create(&trs[i], NULL, printer, &args[i]);
	}

	for (int i = 0; i < N; i++) {
		pthread_join(trs[i], NULL);
	}
}