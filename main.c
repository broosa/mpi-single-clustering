// File: main.c
// Authors: Edward Ly, Byron Roosa(, and possibly George Crowson)
// Last Updated: 22 April 2016
// Single-linkage clustering program on strings using MPI implementation.
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>

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

// The actual comparison (working in chunks with early exit)
bool is_match(char *a, char *b, int len, int max)
{
	int diff = 0;
	for (int i = 0; i < len; i += 6)
	{
		diff += is_match_char(*(a + i    ), *(b + i    ));
		diff += is_match_char(*(a + i + 1), *(b + i + 1));
		diff += is_match_char(*(a + i + 2), *(b + i + 2));
		diff += is_match_char(*(a + i + 3), *(b + i + 3));
		diff += is_match_char(*(a + i + 4), *(b + i + 4));
		diff += is_match_char(*(a + i + 5), *(b + i + 5));
		if (diff > max)
			return false;
	}
	return true;
}

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
	size_t size = sizeof(char) * max_num_strings * array_len;
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
	if (!a) {
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
	
	
	
	// Start clock
	start = MPI_Wtime();
	
	// Malloc local copy of a (WIP)
	int num_local_strings = num_strings / numprocs;
	

	// Copy relevant strings to local process (WIP)
	
	
	// Stop clock and record copy time
	end = MPI_Wtime();
	wall_client = end - start;
	
	
	
	// Print wall times
	if (debug) {
		fprintf(stderr, "Tot. read/copy wall time: server = %f, clients = %f\n", wall_server, wall_client);
		fprintf(stderr, "Avg. read/copy wall time: server = %f, clients = %f\n", wall_server / (double)num_strings, wall_client / (double)num_strings); }
	
	// Choose the first string in the list, compare it to all the other strings in the list
	strncpy(target, &a[0], string_len);
	int a_char_size = num_strings * string_len;
	
	
	
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
	if (debug) 
		fprintf(stdout, "num_strings = %d, num_comps = %d, num_matches = %d, wall_comp = %.6f\n", num_strings, num_comps, num_matches, wall_comp);	
	
	// Cleanup
	free(a); free(local_a);
	MPI_Finalize();
	exit(0);
}
