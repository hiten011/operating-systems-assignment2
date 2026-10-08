/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	int idx = leftstart, st = leftstart;

	while (leftstart <= leftend && rightstart <= rightend) {
		if (A[leftstart] < A[rightstart]) {
			B[idx++] = A[leftstart++];
		} else {
			B[idx++] = A[rightstart++];
		}
	}

    if (leftstart <= leftend) {
        int n = leftend - leftstart + 1;
        memcpy(&B[idx], &A[leftstart], sizeof(int) * n);
        idx += n;
    }

    // copy B to A
    memcpy(&A[st], &B[st], sizeof(int) * (idx - st));
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
   	int l = a->left, r = a->right, level = a->level;
	free(a);

	if (level >= cutoff || l >= r) {my_mergesort(l, r); return NULL;}

	// call with threads
	int mid = l + (r - l) / 2;
	pthread_t t1, t2;

	pthread_create(&t1, NULL, parallel_mergesort, buildArgs(l, mid, level + 1));
	pthread_create(&t2, NULL, parallel_mergesort, buildArgs(mid + 1, r, level + 1));

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);


	// merge the two sorted arrays into one
	merge(l, mid, mid + 1, r);
	return NULL;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
	struct argument *p = malloc(sizeof(struct argument));
	p->left = left; p->right = right; p->level = level;
	return p;
}

