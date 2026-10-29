#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mergesort.h"

// globals need for mergesort.c
int cutoff;
int *A;
int *B;

static int cmp_int(const void *a, const void *b) {

    int ia = *(const int *)a;
    int ib = *(const int *)b;

    if (ia > ib) {
        return 1;
    }
    else if (ia < ib) {
        return -1;
    }
    else {
        return 0;
    }
}

static int is_sorted(const int *arr, int n) {

    for (int i=0; i<n-1; i++) {

        if (arr[i] > arr[i+1]) {
            return 0;
        }
    }
    return 1;
}

static int is_permutation(const int *arr, const int *og, int n) {
    int *a_sorted = malloc(sizeof(int) * n);
    int *o_sorted = malloc(sizeof(int) * n);

    memcpy(a_sorted, arr, sizeof(int) * n);
    memcpy(o_sorted, og, sizeof(int) * n);

    qsort(a_sorted, n, sizeof(int), cmp_int);
    qsort(o_sorted, n, sizeof(int), cmp_int);

    int ok;
    if (memcmp(a_sorted, o_sorted, sizeof(int) * n) == 0) {
        ok = 1;
    } else {
        ok = 0;
    }

    free(a_sorted);
    free(o_sorted);
    return ok;
}

int main(void) {
    printf("edge cases tests for parallel_mergesort()\n");

    return 0;
}