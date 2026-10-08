/** Parallel merge sort over the shared arrays A and B. */

#include <stdlib.h>
#include <string.h>
#include "mergesort.h"

/* Sorting tiny ranges in place avoids merge setup at the leaves. */
#define INSERTION_SORT_LIMIT 32

void merge(int leftstart, int leftend, int rightstart, int rightend)
{
    int left = leftstart;
    int right = rightstart;
    int output = leftstart;
    size_t count;

    /* Both input ranges are sorted and disjoint. Only this range of B is used. */
    while (left <= leftend && right <= rightend) {
        if (A[left] <= A[right]) {
            B[output++] = A[left++];
        } else {
            B[output++] = A[right++];
        }
    }

    if (left <= leftend) {
        count = (size_t)(leftend - left + 1);
        memcpy(B + output, A + left, count * sizeof(*A));
        output += (int)count;
    }
    if (right <= rightend) {
        count = (size_t)(rightend - right + 1);
        memcpy(B + output, A + right, count * sizeof(*A));
    }

    count = (size_t)(rightend - leftstart + 1);
    memcpy(A + leftstart, B + leftstart, count * sizeof(*A));
}

void my_mergesort(int left, int right)
{
    int middle;
    int i;

    if (left >= right) {
        return;
    }

    if (right - left < INSERTION_SORT_LIMIT) {
        for (i = left + 1; i <= right; ++i) {
            int value = A[i];
            int position = i;
            while (position > left && A[position - 1] > value) {
                A[position] = A[position - 1];
                --position;
            }
            A[position] = value;
        }
        return;
    }

    middle = left + (right - left) / 2;
    my_mergesort(left, middle);
    my_mergesort(middle + 1, right);
    if (A[middle] > A[middle + 1]) {
        merge(left, middle, middle + 1, right);
    }
}

void *parallel_mergesort(void *arg)
{
    const struct argument *range = (const struct argument *)arg;
    int middle;
    struct argument *left_arg;
    struct argument *right_arg;
    pthread_t left_thread = {0};
    pthread_t right_thread = {0};
    int left_started = 0;
    int right_started = 0;

    if (range == NULL || range->left >= range->right) {
        return NULL;
    }

    if (range->level >= cutoff) {
        my_mergesort(range->left, range->right);
        return NULL;
    }

    middle = range->left + (range->right - range->left) / 2;
    left_arg = buildArgs(range->left, middle, range->level + 1);
    right_arg = buildArgs(middle + 1, range->right, range->level + 1);

    /* An allocation or thread limit must not leave either half unsorted. */
    if (left_arg != NULL) {
        left_started = pthread_create(&left_thread, NULL,
                                      parallel_mergesort, left_arg) == 0;
    }
    if (right_arg != NULL) {
        right_started = pthread_create(&right_thread, NULL,
                                       parallel_mergesort, right_arg) == 0;
    }

    if (!left_started) {
        my_mergesort(range->left, middle);
    }
    if (!right_started) {
        my_mergesort(middle + 1, range->right);
    }
    if (left_started) {
        pthread_join(left_thread, NULL);
    }
    if (right_started) {
        pthread_join(right_thread, NULL);
    }

    free(left_arg);
    free(right_arg);
    if (A[middle] > A[middle + 1]) {
        merge(range->left, middle, middle + 1, range->right);
    }
    return NULL;
}

struct argument *buildArgs(int left, int right, int level)
{
    struct argument *arg = malloc(sizeof(*arg));
    if (arg != NULL) {
        arg->left = left;
        arg->right = right;
        arg->level = level;
    }
    return arg;
}
