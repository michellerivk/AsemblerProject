#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include "help_functions.h"

/* Runs the first pass */
int first_pass_function(const char *file, assembler_table *table) 
{
    FILE *am = fopen(file, "r");
    char line[MAX_LINE_LENGTH];
    int line_number = 0;

    if (!am) {
        printf("ERROR: Could not open file %s\n", file);
        return 0;
    }

    while (fgets(line, MAX_LINE_LENGTH, am)) {
        line_number++;
        check_line(line, line_number ,table);
    }

    fclose(am);
    return 1;
}