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

static int run_case(const char *name, const int *input, int n, int test_cutoff) {
    int *og = malloc(sizeof(int) * n);
    memcpy(og, input, sizeof(int) * n);

    A = malloc(sizeof(int) * n);
    B = malloc(sizeof(int) * n);
    memcpy(A, input, sizeof(int) * n);
    cutoff = test_cutoff;

    struct argument *arg = buildArgs(0, n - 1, 0);
    parallel_mergesort(arg);
    free(arg);

    int sorted_ok = is_sorted(A, n);
    int perm_ok = is_permutation(A, og, n);
    int pass = sorted_ok && perm_ok;

    if (pass) {
        printf("PASS  %-28s (n=%2d, cutoff=%d)\n", name, n, test_cutoff);
    } else {
        printf("FAIL %s (n=%d, cutoff=%d)\n", name, n, test_cutoff);

        if (sorted_ok== 1) {
            printf("Sorted: yes\n");
        } else {
            printf("Sorted: no\n");
        }
        if (perm_ok == 1) {
            printf("Permutation: yes\n");
        } else {
            printf("Permutation: no\n");
        }
    }

    free(A);
    free(B);
    free(og);
    return pass;
}

int main(void) {
    printf("edge cases tests for parallel_mergesort()\n");

    int reverse[] = {8, 7, 6, 5, 4, 3, 2, 1};
    run_case("reverse sorted", reverse, 8, 2);

    int sorted[] = {1, 2, 3, 4, 5, 6, 7, 8};
    run_case("already sorted", sorted, 8, 2);

    int mixed[] = {4, -2, 5, -9, 0, -2, 7, -9, 5};
    run_case("duplicate and -ve", mixed, 9, 3);

    int empty[1] = {0};
    run_case("empty array", empty, 0, 2);

    int single[] = {53};
    run_case("single element", single, 1, 2);

    int swapped[] = {5, 6, 7, 8, 1, 2, 3, 4};
    run_case("halves swapped", swapped, 8, 2);

    int intertwined[] = {1, 3, 5, 7, 9, 11, 2, 4, 6, 8, 10};
    run_case("intertwines halves, odd", intertwined, 11, 2);
    return 0;
}