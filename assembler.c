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
int main(int argc, char **argv)
{
    int i;

    if (argc < 2)
    {
        printf("Missing file as parameter\n");
        return false;
    }

    for (i = 1; i < argc; i++)
    {
        if (!process_file(argv[i]))
        {
            printf("Error processing file: %s\n", argv[i]);
        }
    }

    return 0;
}


/*
* Handles the full assembly process for a given source file.
* Performs preprocessing, first pass, second pass, translation, and cleanup.
*/
bool process_file(char *filename)
{
    assembler_table *assembler = NULL;

    /* Check for filename length */
    if (strlen(filename) > MAX_LABEL_LENGTH)
    {
        errors_table(FILE_NAME_EXCEED_MAXIMUM, -1);
        return false;
    }

    /* Initialize main assembler table */
    assembler = initialize_assembler_table(filename);
    if (!assembler)
    {
        printf("Failed to initialize assembler table\n");
        return false;
    }

    /* Run preprocessor */
    if (!pre_proc(&assembler))
    {
        free_assembler_table(assembler);
        return false;
    }

    /* Run first pass */
    if (!first_pass(filename, assembler))
    {
        first_pass_errors(ERR_FIRST_PASS, -1, -1);
        free_assembler_table(assembler);
        return false;
    }

    puts("=============First Pass Table:==================");
    print_assembler_table(assembler);

    puts("==========Start second pass============");

    /* Run second pass */
    if (!second_pass(&assembler))
    {
        free_assembler_table(assembler);
        return false;
    }

    puts("=============Second Pass Table:==================");
    print_assembler_table(assembler);

    /* Generate final output files */
    translation_unit(assembler);

    /* Free all allocated memory */
    free_assembler_table(assembler);

    return true;
}
