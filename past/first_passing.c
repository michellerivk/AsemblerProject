#include <stdio.h>
#include <stdlib.h>
#include "data_structs.h"
#include "help_functions.h"

/* a function to check how many lines there are in the file */
int count_lines_in_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    int count = 0;
    char c;

    if (!fp) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }

    fclose(fp);
    return count;
}

int main(int argc, char *argv[]) {
    while (--argc > 0) {
        char *filename = argv[argc];

        char *as_file = add_ending(filename, ".as");

        if (!macro(as_file)) {
            continue;
        }

        char *am_file = add_new_file(filename, ".am");
        if (exe_first_pass(am_file)) {
            continue;
        }

        free(as_file);
        free(am_file);
    }

    return 0;
}
