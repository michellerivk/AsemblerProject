#include <stdio.h>
#include <stdlib.h>
#include "first_pass_functions.h"

/**
 * Executes the first pass over the processed assembly source file (file_name.am).
 *
 * This function processes the '.am' file line by line.
 * It builds the assembler table by recording label names, their memory addresses,
 * and the type of input they have, while also announcing errors.
 *
 * Key responsibilities:
 * + Find the register labels and their addresses.
 * + Store data and update the Data Counter.
 * + Update the Instruction Counter.
 *
 * Errors during this pass will be counted,
 * but will not stop the program immediately, 
 * only after the passed has finished scanning the file.
 *
 * @param file  The name of the '.am' file to read.
 * @param table The assembler_table structure.
 *
 * @return true (1) if the pass completed successfully without errors, 
 * and a false (0) if there were any errors.
 */

int first_pass(const char *file, assembler_table *table) 
{
    int error_count = 0; /* Counts the amount of errors */
    char *full_file = my_malloc(strlen(file) + 4); /* File name + .am */
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
    add_ending_to_string(full_file, file, ".am");

    /* Opens 'file.am' */
    am = fopen(full_file, "r");

    /* Checks if file was opened. Prints error if no. */
    if (!am) 
    {
        first_pass_errors(ERR_AM_FILE, -1, -1);
        return 0;
    }
    
    /* Checks every line of the file to insert into the table. */
    while (fgets(line, MAX_LINE_LENGTH, am)) 
    {
        line_number++;
        check_line(line, line_number ,table, &error_count, label_flag);
    }

    add_entry_addresses(table, &error_count);

    /* Closes files and frees memory */
    fclose(am);
    free(full_file);

    /* Checks if the memory limit was exceeded. Prints error if yes. */
    if (table->data_counter + table->instruction_counter > MAX_MEMORY)
    {
        first_pass_errors(ERR_MAX_MEMORY, -1, error_count);
        error_count++;
    }

    /* Checks if there were any errors. Prints error if yes, and stops the program. */
    if (error_count != 0)
    {
        first_pass_errors(ERR_AMOUNT_OF_ERRORS, -1, error_count);
        return(false);
    }

    /* If there were no errors returns 1. */
    return true;
}