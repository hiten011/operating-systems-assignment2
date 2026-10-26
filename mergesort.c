#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* This function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	// pos is the next element position in temporary array B.
    // start is the original starting index for copying sorted array back to A.
	int pos = leftstart, start = leftstart;

	while (leftstart <= leftend && rightstart <= rightend) {
		if (A[leftstart] < A[rightstart]) {
			// Copy the smaller left element into B and 
			// increment pos and leftstart after copying.
			B[pos++] = A[leftstart++];
		} else {
			// Copy the smaller right element into B and 
			// increment pos and leftstart after copying.
			B[pos++] = A[rightstart++];
		}
	}

    if (leftstart <= leftend) {
        int n = leftend - leftstart + 1;
		// Copy all remaining left elements from A into B.
        memcpy(&B[pos], &A[leftstart], sizeof(int) * n);
        pos += n;
    }

    // Copy temporary array B to A
	// Remaining right-side elements do not need copying.
    // They are already in their correct positions in A.
    memcpy(&A[start], &B[start], sizeof(int) * (pos - start));
}

/* This function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right) {
    // Base case: an empty or single-element subarray is already sorted.
    if (left >= right) {
        return;
    }

    // Calculate the midpoint to divide the array into two halves.
    int mid = left + (right - left) / 2;

    // Recursively call my_mergesort to sort the left half.
    my_mergesort(left, mid);

    // Recursively call my_mergesort sort the right half.
    my_mergesort(mid + 1, right);

    // Merge the two sorted halves into one sorted subarray.
    merge(left, mid, mid + 1, right);
}

/* This function will be called by the testing program. */
void * parallel_mergesort(void *arg){
	struct argument *a = arg;
   	int l = a->left, r = a->right, level = a->level;
	if (level > 0) {
        free(a);
    }

	// Empty or single-element subarray
	if (l >= r) {
		return NULL;
	}

	// Stop creating threads once cutoff is reached
	if (level >= cutoff) {
		my_mergesort(l, r);
		return NULL;
	}

	// Calculate midpoint
	int mid = l + (r - l) / 2;
	pthread_t t1, t2;
	int leftChildCreated = 0;
	int rightChildCreated = 0;

	// LEFT CHILD
	struct argument *leftChild = buildArgs(l, mid, level + 1);

	if (leftChild != NULL) {
		int rc = pthread_create(&t1,NULL,parallel_mergesort,leftChild);
		
		if (rc == 0) {
			leftChildCreated = 1;
		} else {
			free(leftChild);
		}
	}

	// RIGHT CHILD
	struct argument *rightChild = buildArgs(mid + 1, r, level + 1);

	if (rightChild != NULL) {
		int rc = pthread_create(&t2,NULL,parallel_mergesort,rightChild);

		if (rc == 0) {
			rightChildCreated = 1;
		} else {
			free(rightChild);
		}
	}

	// SEQUENTIAL FALLBACKS
	if (!leftChildCreated) {
		my_mergesort(l, mid);
	}

	if (!rightChildCreated) {
		my_mergesort(mid + 1, r);
	}

	// WAIT FOR BOTH THREADS
	if (leftChildCreated) {
		pthread_join(t1, NULL);
	}

	if (rightChildCreated) {
		pthread_join(t2, NULL);
	}

	// MERGE SORTED HALVES
	merge(l, mid, mid + 1, r);
	return NULL;
}

/* Build the argument for the parallel_mergesort function. */
struct argument *buildArgs(int left, int right, int level)
{
    struct argument *p = malloc(sizeof *p);
    if (p == NULL) {
        return NULL;
    }
    p->left = left;
    p->right = right;
    p->level = level;
    return p;
}

