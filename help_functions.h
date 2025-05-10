#include "assembler.h"

/* Create enum for boolean expressions */
typedef enum { false, true } bool;

/* Call helping functions so that the main function can use them*/
/*
int first_pass(const char *am_file);
int macro(const char *as_file);
char *add_ending(const char *file_name, const char *ending);
*/

/**
 * The function removes all the white spaces at the start of the string.
 *
 * @param input The given string.
 * @param i The start of the string.
 */
int delete_white_spaces(char input[], int i);

void check_line(char *line, int line_number, assembler_table *table, int *error_count);