#include <stdio.h>
#include <stdlib.h>
#include "help_functions.h"

/* #############DEBUGGING################33 */
void print_assembler_table(assembler_table *table, int error_count)
{
    label *lbl = table->label_list;
    data *data_node = table->data_section;

    printf("\n========================\n");
    printf("ASSEMBLER TABLE SUMMARY\n");
    printf("========================\n");

    printf("Instruction Counter (IC): %d\n", table->instruction_counter);
    printf("Data Counter (DC): %d\n", table->data_counter);
    printf("Total Errors: %d\n", error_count);

    printf("\n-- LABELS --\n");
    while (lbl != NULL) 
    {
        printf("Label: %-10s | Address: %-4d | Type: %s\n", 
               lbl->name, 
               lbl->address, 
               lbl->type == CODE ? "CODE" :
               lbl->type == DATA ? "DATA" :
               lbl->type == EXTERNAL ? "EXTERNAL" : 
               lbl->type == ENTRY ? "ENTRY" : "UNKNOWN");

        lbl = lbl->next;
    }

    printf("\n-- DATA SECTION --\n");
    while (data_node != NULL)
    {
        printf("Address: %-4d | Value: %-4d\n", data_node->address, data_node->word.value);
        data_node = data_node->next;
    }

    printf("========================\n\n");
}



/* Runs the first pass */
int first_pass(const char *file, assembler_table *table) 
{
    int error_count = 0; /* Counts the amount of errors */
    char *full_file = malloc(strlen(file) + 4); /* File name + .am */
    char line[MAX_LINE_LENGTH]; /* A variable to include the lines of the am file */
    int line_number = 0; /* Counts the amount of lines in the file */
    FILE *am; /* A variable to include the opened am file */

    /* 0-ing the table */
    table->instruction_counter = 0;
    table->data_counter = 0;
    table->label_list = NULL;
    table->data_section = NULL;
    
    /* Adds .am to the file name */
    add_suffix(full_file, file, ".am");

    /* Opens 'file.am' */
    am = fopen(full_file, "r");

    /* Checks if file was opened. Returns error if no. */
    if (!am) {
        printf("ERROR: Could not open file %s\n", full_file);
        return 0;
    }

    /* Checks every line of the file to insert into the table. */
    while (fgets(line, MAX_LINE_LENGTH, am)) 
    {
        line_number++;
        check_line(line, line_number ,table, &error_count);
    }

    /* Closes files and frees memory */
    fclose(am);
    free(full_file);

    print_assembler_table(&table, error_count); /* ########debugging#####33 */

    /* Checks if there were any errors. Returns error if yes. */
    if (error_count != 0)
    {
        printf("Please fix the previous %d errors, and start over!\n", error_count);
        return(0);
    }
    
    /* If there were no errors returns 1. */
    return 1;
}