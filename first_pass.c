#include <stdio.h>
#include <stdlib.h>
#include "first_pass_functions.h"

/**
 * Executes the first pass over the processed assembly source file (file_name.am).
 *
 * This function processes the `.am` file line by line.
 * It builds the assembler table by recording label names, their memory addresses,
 * and the type of input they have, while also announcing errors.
 *
 * Key responsibilities:
 * - Find the register labels and their addresses.
 * - Store data and update the Data Counter.
 * - Update the Instruction Counter.
 *
 * Errors during this pass will be counted 
 * But will not stop the program immediately, 
 * only after the passed has finished scanning the file.
 *
 * @param file  The name of the `.am` file to read.
 * @param table The assembler_table structure.
 *
 * @return 1 if the pass completed successfully without errors, and a 0 if there were any errors.
 */

int first_pass(const char *file, assembler_table *table) 
{
    int error_count = 0; /* Counts the amount of errors */
    char *full_file = malloc(strlen(file) + 4); /* File name + .am */
    char line[MAX_LINE_LENGTH]; /* A variable to include the lines of the am file */
    int line_number = 0; /* Counts the amount of lines in the file */
    FILE *am; /* A variable to include the opened am file */
    bool label_flag = false;

    /* 0-ing the table */
    table->instruction_counter = 100;
    table->data_counter = 0;
    table->label_list = NULL;
    table->data_section = NULL;
    
    /* Adds .am to the file name */
    add_suffix(full_file, file, ".am");

    /* Opens 'file.am' */
    am = fopen(full_file, "r");

    /* Checks if file was opened. Returns error if no. */
    if (!am) 
    {
        printf("ERROR: Could not open file %s\n", full_file);
        return 0;
    }

    /* Checks every line of the file to insert into the table. */
    while (fgets(line, MAX_LINE_LENGTH, am)) 
    {
        line_number++;
        check_line(line, line_number ,table, &error_count, label_flag);
    }

    /* Closes files and frees memory */
    fclose(am);
    free(full_file);

    /*print_assembler_table(table);*/ /* ########debugging####### */

    /* Checks if there were any errors. Returns error if yes, and stops the program. */
    if (error_count != 0)
    {
        printf("Please fix the previous %d errors, and start over!\n", error_count);
        return(0);
    }

    printf("The were no errors.\n\n");

    print_assembler_table(table);
    

    /* If there were no errors returns 1. */
    return 1;
}