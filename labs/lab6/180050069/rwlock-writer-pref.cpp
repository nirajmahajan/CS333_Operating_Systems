#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  	pthread_mutex_init(&rw->mutex, NULL);
  	pthread_cond_init(&rw->go_readers, NULL);
  	pthread_cond_init(&rw->go_writers, NULL);
  	int reader_count = 0;
	int writer_waiting = 0;
	int writer_present = 0;
}

void ReaderLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->mutex);
	while(rw->writer_present == 1 || rw->writer_waiting > 0){
		pthread_cond_wait(&rw->go_readers, &rw->mutex);
	}
	rw->reader_count += 1;
	pthread_mutex_unlock(&rw->mutex);
}

void ReaderUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->mutex);
	rw->reader_count--;
	if (rw->reader_count == 0){
		pthread_cond_signal(&rw->go_writers);
	}
	pthread_mutex_unlock(&rw->mutex);
}

void WriterLock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->mutex);
	rw->writer_waiting += 1;
	while(rw->reader_count > 0 || rw->writer_present == 1){
		pthread_cond_wait(&rw->go_writers, &rw->mutex);
	}
	rw->writer_waiting -= 1;
	rw->writer_present = 1;
	pthread_mutex_unlock(&rw->mutex);
}

void WriterUnlock(struct read_write_lock * rw)
{
	pthread_mutex_lock(&rw->mutex);
	rw->writer_present = 0;
	if (rw->writer_waiting == 0) {
		pthread_cond_broadcast(&rw->go_readers);
	} else {
		pthread_cond_signal(&rw->go_writers);
	}
	pthread_mutex_unlock(&rw->mutex);
}
