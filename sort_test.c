#include <stdlib.h>
#include <stdio.h>

int a[9] = {16, 6, 22, 46, 8, 3, 7, 24, 18};

void q_sort(int *start, int *end)
{
	//A single array is always sorted
	if (start >= end) {
		return;
	}

	int *pivot = end;
	//Partition the array
    //Be careful; for the loops to work these start
    //one element outside on either side

    int *i = start;

    for (int *j = start; j < end; j++) {
        if (*j < *pivot) {
            int temp = *j;
            *j = *i;
            *i = temp;
            i++;
        }
    }

    int temp = *i;
    *end = *i;
    *i = temp;

    pivot = i;

	//Recursively sort both partitions
	q_sort(start, pivot - 1);
	q_sort(pivot + 1, end);
}

int main(void)
{
    int *start = &a[0];
    int *end = a + 8;

    q_sort(start, end);

    for (int *i = start; i < end; i++) {
        printf("%d ", *i);
    }
    printf("\n");

    return 0;
}
