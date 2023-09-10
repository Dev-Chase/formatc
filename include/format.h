#ifndef FORMAT_H
#define FORMAT_H
#include <stdio.h>

#define MAX_TAB_SIZE 4
typedef struct {
    char ch;
    int n;
} FormatType;

typedef struct {
    char *fname;
    FormatType new_fmt;
} HandleFileParams;

void *handle_file(void *params);

#endif
