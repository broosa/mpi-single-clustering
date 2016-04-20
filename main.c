#include <stdio.h>
#include <stdlib.h>
//#include <mpi.h>
#include <stdbool.h>

bool is_match(char *a, char *b, int len, int max);

int main(int argc, char *argv)
{
	// MPI variables
	int numprocs, myrank, namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

	// Program variables
	bool debug = true;
	int i, j; // counters
	double start = 0.0, end = 0.0, wall_server, wall_client, wall_comp;

	// Main variables
	int string_len, max_num_strings = 1e6, num_strings = 0, threshold = 2;
	int diff, distance, local_distance, num_comps;
	char *a, *local_a, *stream;
	FILE *fstream = NULL;

	// Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(processor_name, &namelen);

	// Setup input
	if (argc < 3) {
		fprintf(stderr, "Usage: ./main file-name string-length\n");
		exit(1);
    }
	string_len = atoi(argv[2]);

	// Variables dependent on string length
	int array_len = string_len + 1;
	size_t size = sizeof(char) * max_num_strings * array_len;
	stream = (char *)malloc(sizeof(char) * array_len);
	char target[array_len], other[array_len];

	// Cleanup
	free(a); free(local_a);
	MPI_Finalize();
	exit(0);
}

bool is_match(char *a, char *b, int len, int max)
{

    int diff_count = 0;
    for (int i = 0; i < len; i++) {
        char a_char = *(a + i);
        char b_char = *(b + i);

        char is_match;

        //Check whether the twho characters match, but don't
        //use a branch. ASM is needed to do a left rotate
        //so that we can interpret the sign bit seperately.

        //If a == b, then a ^ b == 0. We then subtract one and
        //rotate the byte left by 1 bit so that the sign bit
        //can be masked off. The result will be 0 if the characters
        //match and 1 if they don't. We can then maintain a
        //running count of the distance between the two strings
        //without using a branch. This is probably unnecessary, but
        //is way too cool to do without.

        //NOTE: this only works if a and b are less than 128.
        //Otherwise the sign bits will
        asm ("mov %1, %%al\n\t"
            "xor %2, %%al\n\t"
            "sub $1, %%al\n\t"
            "rol $1, %%al\n\t"
            "and $0x01, %%al\n\t"
            "xor $0x01, %%al\n\t"
            "mov %%al, %0"
            : "=r" (is_match)
            : "r" (a_char), "r" (b_char)
            : "%al");

        printf("%d\n", is_match);

        //is_match will be 0 when the chars are equal and will
        //be one when they are different
        diff_count += is_match;

        if (diff_count > max) {
            return false;
        }
    }

    return true;
}
