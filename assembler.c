#include "assembler.h"


/**
 * @brief Main entry point of the assembler program.
 * 
 * Processes each input file passed as a command line argument.
 * For each file, it initializes the assembler data structures,
 * runs pre-processing, performs the first and second passes,
 * and then handles the final translation.
 * 
 * @param argc Number of command line arguments.
 * @param argv Array of command line argument strings (input filenames).
 * @return int Returns 0 on success, or non-zero on failure.
 */
int main(int argc, char **argv) {
    assembler_table *assembler = NULL;
    int i;

    /* Process each input file */
    for (i = 1; i < argc; i++) {
        /* Check if filename is too long */
        if (strlen(argv[i]) > MAX_LABEL_LENGTH) {
            errors_table(FILE_NAME_EXCEED_MAXIMUM, -1);
            return false;
        }

        /* Initialize assembler table for the current file */
        assembler = initialize_assembler_table(argv[i]);

        /* Run pre-processing phase (e.g., macro expansion) */
        if (pre_proc(&assembler) == false) {
            return false;
        }

        /* First pass: parse and collect labels, instructions, and data */
        if (!first_pass(argv[i], assembler)) {
            first_pass_errors(ERR_FIRST_PASS, " ",-1);
            return 1;
        }

        puts("=============First Pass Table:==================");
        print_assembler_table(assembler); /* Debug output */

        puts("==========Start second pass============");

        /* Second pass: resolve addresses, external labels, etc. */
        if (second_pass(&assembler) == false) {
            return false;
        }

        puts("=============Second Pass Table:==================");
        print_assembler_table(assembler);

        /* Final translation of the assembly code */
        translation_unit(assembler);

        /* Clean up all dynamically allocated memory */
        free_assembler_table(assembler);


    }

    return 0;
}

