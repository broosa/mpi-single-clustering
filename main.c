#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>

bool is_match(char *a, char *b, int len, int max)
{
	int diff = 0;
	for (int j = 0; j < len; j += 6)
	{
		diff += (a[j    ] != b[j    ]);
		diff += (a[j + 1] != b[j + 1]);
		diff += (a[j + 2] != b[j + 2]);
		diff += (a[j + 3] != b[j + 3]);
		diff += (a[j + 4] != b[j + 4]);
		diff += (a[j + 5] != b[j + 5]);
		if (diff > max)
			return false;
	}
	return true;
}

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
		exit(1); }
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
