#include "format.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FMT_STR_LEN 6

FormatType detect_format(FILE *fp);
int format_file(FILE *fp, char *fname, FormatType old_fmt, FormatType new_fmt);

void *handle_file(void *params) {
	// Deconstruct Params
	char *fname = ((HandleFileParams *)params)->fname;
	FormatType new_fmt = ((HandleFileParams *)params)->new_fmt;

	// File Extension Check
	if (strcmp(fname + strlen(fname) - 2, ".c") != 0 &&
		strcmp(fname + strlen(fname) - 2, ".h") != 0) {
		fprintf(stderr, "error: \"%s\" has invalid file type (not .c or .h)\n",
				fname);
		return NULL;
	}

	// Open File and Check for Errors
	FILE *fp = fopen(fname, "r");
	if (fp == NULL) {
		fprintf(stderr, "error opening file: \"%s\"\n", fname);
		return NULL;
	}

	// Find File's Format
	FormatType old_fmt = detect_format(fp);

	// Check for Format Detection Errors
	if (old_fmt.ch == '\0') {
		fprintf(stderr, "couldn't detect old formatting for: \"%s\"\n", fname);
		return NULL;
	}

	// Existing vs. New Format Checks
	if (old_fmt.n == new_fmt.n) {
		fprintf(stderr, "file \"%s\" is already formatted.\n", fname);
		return NULL;
	}

	// Format File
	format_file(fp, fname, old_fmt, new_fmt);

	return NULL;
}

FormatType detect_format(FILE *fp) {
	// Initialize Beginning Buffer (+1 for NULL termination)
	char beg[MAX_TAB_SIZE + 1];

	// set second character to NULL for space detection
	beg[1] = '\0';
	int space_len = 0;
	while ((beg[0] = fgetc(fp)) != EOF) {
		if (beg[0] == '\t') {
			return (FormatType){'\t', 1};
		}
		if (beg[0] == ' ') {
			fscanf(fp, "%3[ ]%*[^\n]", beg + 1);
			space_len = strlen(beg);
			if (space_len > 1) {
				return (FormatType){' ', space_len};
			}
		}
	}

	return (FormatType){'\0', 0};
}

int format_file(FILE *fp, char *fname, FormatType old_fmt, FormatType new_fmt) {
	// Create Swap File
	char temp_file_name[strlen(fname) + strlen(".temp")];
	strcpy(temp_file_name, fname);
	strcat(temp_file_name, ".temp");
	FILE *temp = fopen(temp_file_name, "w");

	// Check if Swap File was Successfully created
	if (temp == NULL) {
		fprintf(stderr, "error: couldn't create swap file, aborting\n");
		return EXIT_FAILURE;
	}

	// Reset File Pointer
	rewind(fp);

	// Initialize Loop Variables
	char beg[old_fmt.n + 1];
	char format_str1[FMT_STR_LEN]; // %n[c] + \0
	sprintf(format_str1, "%%%d[%c]", old_fmt.n, old_fmt.ch);

	// Copy OG file to Swap, putting New Format in Place of Old
	while ((beg[0] = fgetc(fp)) != EOF) {
		if (beg[0] == old_fmt.ch) {
			ungetc(beg[0], fp);

			// SAFE -- either "%2[ ]", "%4[ ]", or "%1[\t]"
			fscanf(fp, format_str1, beg);
			if (strlen(beg) == old_fmt.n) {
				for (int i = 0; i < new_fmt.n; i++) {
					fputc(new_fmt.ch, temp);
				}
				continue;
			}
		}
		fputc(beg[0], temp);
	}

	// Close Files
	fclose(fp);
	fclose(temp);

	// Delete Original File & Check if Succesful
	if (remove(fname) != EXIT_SUCCESS) {
		fprintf(stderr,
				"error: couldn't delete file \"%s\" for swap, aborting\n",
				fname);
		remove(temp_file_name);
		return EXIT_FAILURE;
	}

	// Rename Swap File to Old Name
	rename(temp_file_name, fname);

	printf("succesfully formatted \"%s\"!\n", fname);
	return EXIT_SUCCESS;
}
