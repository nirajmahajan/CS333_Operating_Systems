#include "alloc.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

void * p;
int arr_size = PAGESIZE/MINALLOC;
int starts[512];
int ends[512];

int init_alloc(){
	p = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
	if ((int)p == -1){
		return -1;
	}
	for (int i = 0; i < arr_size; i++){
		starts[i] = 0;
		ends[i] = 0;
	}
	return 0;

int cleanup() {
	for (int i = 0; i < arr_size; i++){
		starts[i] = 0;
		ends[i] = 0;
	}
	return munmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

char *alloc(int size) {
	if (size % MINALLOC != 0){
		return -1;
	}
	int size_units = size / MINALLOC;

	int state = 0;
	int probable_start = 0;
	int free_count = 0;
	for (int i = 0; i < arr_size; i++) {
		state += starts[i];

		if (state == 0) {
			free_count += 1;
		} else{
			probable_start = i+1;
			free_count = 0;
		}

		if (free_count == size_units) {
			starts[probable_start] = 1;
			ends[i] = 1;
			return p + probable_start*MINALLOC;
		}
		state -= ends[i];
	}
	return -1;

}

void dealloc(char * adrr){
	offset = ((int)adrr - (int)p)/MINALLOC;
	offset = (int) offset;

	if (starts[offset] == 0) {
		return -1;
	}

	for (int i = offset; i < arr_size; i++) {
		if (ends[i] == 1){
			starts[offset] = 0;
			ends[offset] = 0;
			return 0;
		}
	}

	printf("Debug: Shouldn't Come here\n");
	return -1;

}
