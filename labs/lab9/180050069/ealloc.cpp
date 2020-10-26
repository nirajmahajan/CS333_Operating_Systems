#include "ealloc.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

char* p1 = NULL;
char* p2 = NULL;
char* p3 = NULL;
char* p4 = NULL;
static const int arr_size = PAGESIZE/MINALLOC;
int start1[arr_size];
int start2[arr_size];
int start3[arr_size];
int start4[arr_size];
int end1[arr_size];
int end2[arr_size];
int end3[arr_size];
int end4[arr_size];

void init_alloc(){
	for (int i = 0; i < arr_size; i++){
		start1[i] = 0;
		start2[i] = 0;
		start3[i] = 0;
		start4[i] = 0;
		end1[i] = 0;
		end2[i] = 0;
		end3[i] = 0;
		end4[i] = 0;
	}
	return;
}

void cleanup() {
	for (int i = 0; i < arr_size; i++){
		start1[i] = 0;
		start2[i] = 0;
		start3[i] = 0;
		start4[i] = 0;
		end1[i] = 0;
		end2[i] = 0;
		end3[i] = 0;
		end4[i] = 0;
	}
}

char *alloc(int size) {
	if (size % MINALLOC != 0){
		return NULL;
	}
	int size_units = size / MINALLOC;
	
	if (p1 == NULL) {
		p1 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p1 == (void *) -1){
			return NULL;
		}
	}

	int state = 0;
	int probable_start = 0;
	int free_count = 0;
	for (int i = 0; i < arr_size; i++) {
		state += start1[i];

		if (state == 0) {
			free_count += 1;
		} else{
			probable_start = i+1;
			free_count = 0;
		}

		if (free_count == size_units) {
			start1[probable_start] = 1;
			end1[i] = 1;
			return p1 + probable_start*MINALLOC;
		}
		state -= end1[i];
	}

	if (p2 == NULL) {
		p2 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p2 == (void *) -1){
			return NULL;
		}
	}

	state = 0;
	probable_start = 0;
	free_count = 0;
	for (int i = 0; i < arr_size; i++) {
		state += start2[i];

		if (state == 0) {
			free_count += 1;
		} else{
			probable_start = i+1;
			free_count = 0;
		}

		if (free_count == size_units) {
			start2[probable_start] = 1;
			end2[i] = 1;
			return p2 + probable_start*MINALLOC;
		}
		state -= end2[i];
	}

	if (p3 == NULL) {
		p3 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p3 == (void *) -1){
			return NULL;
		}
	}
	state = 0;
	probable_start = 0;
	free_count = 0;
	for (int i = 0; i < arr_size; i++) {
		state += start3[i];

		if (state == 0) {
			free_count += 1;
		} else{
			probable_start = i+1;
			free_count = 0;
		}

		if (free_count == size_units) {
			start3[probable_start] = 1;
			end3[i] = 1;
			return p3 + probable_start*MINALLOC;
		}
		state -= end3[i];
	}

	if (p4 == NULL) {
		p4 = (char *) mmap(0, PAGESIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	
		if (p4 == (void *) -1){
			return NULL;
		}
	}

	state = 0;
	probable_start = 0;
	free_count = 0;
	for (int i = 0; i < arr_size; i++) {
		state += start4[i];

		if (state == 0) {
			free_count += 1;
		} else{
			probable_start = i+1;
			free_count = 0;
		}

		if (free_count == size_units) {
			start4[probable_start] = 1;
			end4[i] = 1;
			return p4 + probable_start*MINALLOC;
		}
		state -= end4[i];
	}
	return NULL;

}

void dealloc(char * addr){

	if (p1 == NULL) {
		return;
	}
	char * temp = p1;
	int found = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) {
		if (temp == addr) {
			start1[i] = 0;
			found = 1;
		}

		if (found == 1 && end1[i] == 1){
			end1[i] = 0;
			return;
		}
	}

	if (p2 == NULL) {
		return;
	}
	temp = p2;
	found = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) {
		if (temp == addr) {
			start2[i] = 0;
			found = 1;
		}

		if (found == 1 && end2[i] == 1){
			end2[i] = 0;
			return;
		}
	}

	if (p3 == NULL) {
		return;
	}
	temp = p3;
	found = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) {
		if (temp == addr) {
			start3[i] = 0;
			found = 1;
		}

		if (found == 1 && end3[i] == 1){
			end3[i] = 0;
			return;
		}
	}

	if (p4 == NULL) {
		return;
	}
	temp = p4;
	found = 0;
	for (int i = 0; i < arr_size; i++,temp += MINALLOC) {
		if (temp == addr) {
			start4[i] = 0;
			found = 1;
		}

		if (found == 1 && end4[i] == 1){
			end4[i] = 0;
			return;
		}
	}
	return;
}


