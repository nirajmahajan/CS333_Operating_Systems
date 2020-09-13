#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

const int n_threads = 15;
int curr_print = 0;

pthread_cond_t* cvs = NULL;
pthread_mutex_t mutex;

void *printer(void* args){
	int target = *(int*)args;
	while (1){
		pthread_mutex_lock(&mutex);
		while(curr_print != target){
			// printf("wait %d\n", target);
			pthread_cond_wait(&cvs[target], &mutex);
		}
		printf("%d\n", target+1);
		usleep(200000);
		curr_print = (curr_print+1)%n_threads;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cvs[curr_print]);
	}

	return NULL;
}

int main(int argc, char const *argv[])
{
	// initialise mutex
	int rc = pthread_mutex_init(&mutex, NULL);
	assert(rc == 0);

	cvs = (pthread_cond_t*) malloc(sizeof(pthread_cond_t)*n_threads);

	// // wait all cvs except first
	// for (int i = 1; i < n_threads; i++){
	// 	pthread_cond_wait(&cvs[i],&mutex);
	// }

	int targs[n_threads];
	for (int i = 0; i < n_threads; i++){
		targs[i] = i;
	}
	
	pthread_t trs[n_threads];
	for (int i = 0; i < n_threads; i++){
		pthread_create(&trs[i], NULL, printer, &targs[i]);
	}
	for (int i = 0; i < n_threads; i++){
		pthread_join(trs[i], NULL);
	}
	return 0;
}