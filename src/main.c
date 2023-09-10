
#include "format.h"
#include <limits.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

FormatType get_format(char *inp) {
	if (strcmp(inp, "space2") == 0) {
		return (FormatType){' ', 2};
	}
	if (strcmp(inp, "space4") == 0) {
		return (FormatType){' ', 4};
	}
	if (strcmp(inp, "tab") == 0) {
		return (FormatType){'\t', 1};
	}
	return (FormatType){'\0', 0};
}

int main(int argc, char **argv) {
	// Parameter Number Validation
	if (argc < 3 || argc > INT_MAX) {
		fprintf(stderr, "usage: %s option files\n", argv[0]);
		return 1;
	}

	// Detect Original Format
	FormatType new_fmt = get_format(argv[1]);
	if (new_fmt.ch == '\0') {
		fprintf(stderr, "error: invalid option (try tab, space2, or space4)\n");
		return 1;
	}

	// Create Arrays to Hold Thread ID's and Params
	pthread_t threads[argc - 2];
	HandleFileParams params[argc - 2];

	// Format Requested Files in Seperate Threads
	for (int i = 0; i < argc - 2; i++) {
		params[i] =
			(HandleFileParams){.new_fmt = new_fmt, .fname = argv[2 + i]};
		pthread_create(&threads[i], NULL, handle_file, params + i);
	}

	// Wait for threads to finish executing
	for (int i = 0; i < argc - 2; i++) {
		pthread_join(threads[i], NULL);
	}

	// Exit
	return 0;
}
