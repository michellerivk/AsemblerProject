#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stdio.h>
#include "assembler.h"  /* for assembler_table, macro, macro_content, bool, etc. */


/* ============================ Functions of macro procces ================================== */

/**
 * Processes a single line from the source file during preprocessing.
 * Handles comments, line length validation, macro definitions, and macro usage.
 *
 * @param line The line to process.
 * @param assembler Pointer to the assembler table.
 * @param fp_as Source file pointer (.as).
 * @param fp_am Output file pointer (.am).
 * @param macro_name Buffer for storing the current macro name.
 * @param content Pointer to store macro body content.
 * @param line_counter Current line number (for error reporting).
 * @param final_error Indicates if a fatal error occurred during processing.
 * @return true if the line was processed, false if skipped.
 */

bool process_line(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_as, FILE *fp_am,
                  char macro_name[MAX_LINE_LENGTH], macro_content **content,
                  int *line_counter, bool *final_error);
                  

/**
 * Checks if the current line starts a macro definition and handles it.
 *
 * @param line The current line being processed.
 * @param assembler Pointer to the assembler table.
 * @param fp_as Source file pointer (.as).
 * @param macro_name Buffer to store the macro name.
 * @param content Pointer to store the macro body content.
 * @param line_counter Current line number (for error reporting).
 * @param final_error Indicator for fatal errors during macro processing.
 * @return true if the line is a macro definition (handled or not), false otherwise.
 */

bool handle_macro_definition(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_as,
                             char macro_name[MAX_LINE_LENGTH], macro_content **content,
                             int *line_counter, bool *final_error);

/**
 * Writes a line to the output file, expanding macros if used.
 *
 * @param line The current line to process.
 * @param assembler Pointer to the assembler table (includes macro list).
 * @param fp_am Output file pointer (.am).
 */

void handle_macro_usage_or_regular_line(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_am);

/**
 * Handles macro declaration: extracts its name, reads its body, and stores it.
 *
 * @param assembler Pointer to the assembler table.
 * @param line The current line being read.
 * @param macro_name Buffer to store the macro's name.
 * @param fp_as Pointer to the input file.
 * @param content Pointer to hold the macro's body content.
 * @param line_counter Current line number in the file.
 * @return true if macro was processed successfully, false otherwise.
 */

bool macro_trearment(assembler_table **assembler, char line[MAX_LINE_LENGTH],
                     char macro_name[MAX_LINE_LENGTH], FILE *fp_as,
                     macro_content **content, int *line_counter);

/**
 * Reads the body of a macro from the source file until 'endmcro' is found.
 * Stores each valid line into the macro content list.
 *
 * @param fp_as Pointer to the source file.
 * @param content Pointer to the macro content list to fill.
 * @param line_counter Pointer to the current line number.
 * @param line Buffer to read and process each line.
 * @return true if the macro ended correctly with 'endmcro', false otherwise.
 */

bool read_macro_body(FILE *fp_as, macro_content **content, int *line_counter, char *line);

/**
 * Searches a linked list of macros for one with the given name.
 * @return Pointer to matching macro, or NULL if not found.
 */
macro *find_macro(macro *head, char *name);

/**
 * Add a new content line to the macro content linked list.
 */
void add_to_content_list(macro_content **head, char *content);

/**
 * Add a new macro with given name and content to the macro linked list.
 */
void add_to_macro_list(macro **head, char *macro_name, macro_content *content);


/* ============================ Functions of error handling ================================== */

/**
 * Extracts the macro name from a line and validates it.
 *
 * @param macro_name Buffer to store the extracted macro name.
 * @param line The input line containing the macro definition.
 * @param line_number The current line number (used for error reporting).
 * @param list Pointer to the existing macro list.
 * @return true if the macro name is valid, false otherwise.
 */

bool extract_and_validate_macro_name(char *macro_name, char *line, int line_number, macro *list);

/**
 * Validates a macro name according to assembler rules.
 * Checks length, emptiness, duplicates, reserved words, and character legality.
 *
 * @param macro_name The macro name to validate.
 * @param line_counter Line number (used for error reporting).
 * @param macro_list List of already defined macros.
 * @return true if the macro name is valid, false otherwise.
 */

bool macro_name_examine(char macro_name[], int line_counter, macro *macro_list);

/**
 * Checks that there is no extra text after the 'mcroend' keyword.
 *
 * @param line The remaining part of the line after 'mcroend'.
 * @param line_counter The current line number (used for error reporting).
 * @return true if the line is clean, false if extra text is found.
 */

bool examine_macroend(char line[], int line_counter);

#endif /* PREPROCESSOR_H */
