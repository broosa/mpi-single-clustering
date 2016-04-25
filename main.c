// File: main.c
// Authors: Edward Ly, Byron Roosa(, and possibly George Crowson)
// Last Updated: 22 April 2016
// Single-linkage clustering program on strings using MPI implementation.
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>
#include <string.h>

bool is_match(char *a, char *b, int len, int max);
int is_match_char(char a, char b);

int main(int argc, char **argv)
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
	int diff, num_matches, local_distance, num_comps;
	int *count;
	bool *merged;
	char *a, *local_a, *stream;
	FILE *fstream = NULL;

	// Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Get_processor_name(processor_name, &namelen);

	// Setup input
	if (argc < 3) {

		fprintf(stderr, "Usage: %s file-name string-length\n", argv[0]);
		exit(1); }

	string_len = atoi(argv[2]);

	// Variables dependent on string length
	int array_len = string_len + 1;
	int a_char_size = num_strings * string_len;
	int num_local_strings = num_strings / numprocs;
	size_t size = sizeof(char) * max_num_strings * array_len;
	size_t local_size = sizeof(char) * num_local_strings * array_len;
	stream = (char *)malloc(sizeof(char) * array_len);
	char target[array_len], other[array_len];

	// Start clock
	start = MPI_Wtime();

	// Open file
	fstream = fopen(argv[1], "r");
	if (fstream == NULL) {
		fprintf(stderr, "Unable to open file '%s'\n", argv[1]);
		exit(1); }

	// Malloc a
	a = (char *)malloc(size);
	if (!a && myrank == 0) {
		perror("unable to allocate array a: ");
		exit(-1); }

	// Read file to a
	i = 0;
	while (fscanf(fstream, "%s", stream) != EOF) {
		if (debug)
			fprintf(stderr, "%.30s\n", stream);
		strcpy(&a[i], stream);
		i += string_len;
		num_strings++; }

	// Close file and stream
	free(stream);
	fclose(fstream);

	// Stop clock and record read time
	end = MPI_Wtime();
	wall_server = end - start;

	// Variables dependent on num_strings
	size_t count_size = sizeof(int) * num_strings;
	size_t merged_size = sizeof(bool) * num_strings;

	// Malloc count and merged arrays
	count = (int *)malloc(count_size);
	if (!count && myrank == 0) {
		perror("unable to allocate array count: ");
		exit(-1); }
	merged = (bool *)malloc(merged_size);
	if (!merged && myrank == 0) {
		perror("unable to allocate array merged: ");
		exit(-1); }

	// Sort strings from highest count to lowest count (WIP)



	// Start clock
	start = MPI_Wtime();

	// Malloc local copy of a
	local_a = (char *)malloc(local_size);
	if (!local_a && myrank == 0) {
		perror("unable to allocate array local_a: ");
		exit(-1); }

	// Copy relevant strings to local process (WIP)


	// Stop clock and record copy time
	end = MPI_Wtime();
	wall_client = end - start;



	// Print wall times
	if (debug && myrank == 0) {
		fprintf(stderr, "Tot. read/copy wall time: server = %f, clients = %f\n", wall_server, wall_client);
		fprintf(stderr, "Avg. read/copy wall time: server = %f, clients = %f\n", wall_server / (double)num_strings, wall_client / (double)num_strings); }


	// First test: find similar strings to merge
	// Choose the first string in the list, compare it to all the other strings in the list

	// In the real program, finding the non-merged string with the highest count would go here.
	strncpy(target, &a[0], string_len);


	// Start clock
	start = MPI_Wtime();

	// Do the actual comparison (find the number of strings to merge)
	num_comps = num_matches = 0;
	for (i = string_len; i < a_char_size; i += string_len) {
		strncpy(other, &a[i], string_len);
		num_matches += is_match(target, other, string_len, threshold);
		num_comps++; }

	// Stop clock and record/print comp time
	end = MPI_Wtime();
	wall_comp = end - start;
	if (debug && myrank == 0)
		fprintf(stdout, "num_strings = %d, num_comps = %d, num_matches = %d, wall_comp = %.6f\n", num_strings, num_comps, num_matches, wall_comp);

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


// The individual character comparison
int is_match_char(char a, char b)
{
	char is_matched;

	//Check whether the twho characters match, but don't
	//use a branch. ASM is needed to do a left rotate
	//so that we can interpret the sign bit separately.

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
		: "=r" (is_matched)
		: "r" (a), "r" (b)
		: "%al");

	//is_match will be 0 when the chars are equal and will
	//be one when they are different

	return (int)is_matched;
}
