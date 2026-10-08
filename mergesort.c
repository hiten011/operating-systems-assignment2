/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	int idx = leftstart, st = leftstart, len = rightend - leftstart + 1;

	while (leftstart <= leftend && rightstart <= rightend) {
		if (A[leftstart] < A[rightstart]) {
			B[idx++] = A[leftstart++];
		} else {
			B[idx++] = A[rightstart++];
		}
	}

	while (leftstart <= leftend) 	B[idx++] = A[leftstart++];
	while (rightstart <= rightend) 	B[idx++] = A[rightstart++];

	memcpy(&A[st], &B[st], sizeof(int) * len);
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
	if (left >= right) return;

	int mid = left + (right - left) / 2;
	my_mergesort(left, mid); 
	my_mergesort(mid + 1, right);
	
	// merge the two sorted arrays into one
	merge(left, mid, mid + 1, right);
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
	struct argument *a = arg;
	my_mergesort(a->left, a->right);
	return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
	struct argument *p = malloc(sizeof(struct argument));
	p->left = left; p->right = right; p->level = level;
	return p;
}

