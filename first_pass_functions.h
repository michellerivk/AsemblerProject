#ifndef FIRST_PASS_FUNCTIONS_H
#define FIRST_PASS_FUNCTIONS_H

#include "assembler.h"

/**
 * @file first_pass_functions.h
 * @brief Header file for first pass of assembler.
 *
 * This file contains function declarations shared between:
 * - first_pass_funcitons.c
 * - first_pass_error_checks.c
 *
 * All functions operate on the assembler table structure to update the IC/DC,
 * and prepare the code for second pass processing.
 */


 /* =============== Functions for the first_pass_functions.c file =============== */


/* ############################### Main Functions Declarations ############################### */

/**
 * @brief Validates the given line from the '.am' file.
 *
 * Updates the assembler table and IC/DC if needed.
 *
 * @param line         The line to check.
 * @param line_number  The number of the line in the source file.
 * @param table        The assembler's table structure.
 * @param error_count  The number of errors found so far.
 * @param label_flag   Boolean variable used to check if this line starts with a label.
 */
void check_line(char *line, int line_number, assembler_table *table, int *error_count, bool label_flag);

/**
 * @brief Gets the label from the line and adds it to the assembler's table,
 *        deciding if it's followed by a directive or a command.
 *
 * @param table           The assembler's table structure.
 * @param line            The line with the label.
 * @param i               The starting index of the line.
 * @param error_count     The number of errors found so far.
 * @param commands        Array of valid commands.
 * @param directives      Array of valid directives.
 * @param commands_len    Number of commands in array.
 * @param directives_len  Number of directives in array.
 * @param label_flag      Boolean variable used to check if this line starts with a label.
 *
 * @return true (1) if label was added successfully, false (0) otherwise.
 */
int add_label(assembler_table *table, char *line, int i, int *error_count, 
              const char *commands[], const char *directives[], int commands_len, 
              int directives_len, bool label_flag);

/**
 * @brief Updates the instruction counter based on the number of operands in a command.
 *
 * @param line         The line to check.
 * @param i            The index after the label.
 * @param commands     List of command names.
 * @param com_len      Number of the commands.
 * @param table        The assembler's table structure.
 * @param error_count  The number of errors found so far.
 * @param label_flag   Boolean variable used to check if this line starts with a label.
 */
char * update_ic(char *line, int i, const char *commands[], int com_len, 
               assembler_table *table, int *error_count, bool label_flag);


/* ############################### Command Parsing and Encoding ############################### */

/**
 * @brief Checks if the command or directive exists in the given list of words.
 *
 * @param line         The line to check.
 * @param start        The index in the line where the word begins.
 * @param words        Array of valid directives / commands.
 * @param amount       Number of words in the list.
 * @param error_count  The number of errors found so far.
 * @param label_flag   A flag to indicate if the line includes a label before the word.
 *
 * @return The length of the word found (if it's valid), or -1 otherwise.
 */
int check_word(char *line, int start, const char *words[], int amount, int *error_count, bool label_flag);

/**
 * @brief Returns the opcode value of the given command name.
 *
 * @param word The command name (e.g. "mov", "add", "jmp").
 * @return The opcode value (0-15) if valid, or -1 if the command is unknown.
 */
 int check_command_value(char *com);

/**
 * @brief Returns the number of operands required by the given command.
 *
 * @param com  The command name.
 *
 * @return 0, 1, or 2 decided by the command.
 */
int get_instruction(char *com);

/**
 * @brief Validates an operand and returns its addressing-mode code.
 *
 * @param operand  The operand.
 *
 * @return The addressing-mode code of the operand.
 */
int get_addressing_mode(char *operand);

/**
 * @brief Extracts source and destination operands from a line after the command.
 *
 * @param line             The full assembly line.
 * @param command_start_i  The index where the command starts.
 * @param command_len      The length of the command.
 * @param src              (OUT) buffer for source operand.
 * @param dest             (OUT) buffer for destination operand.
 * @param operand_count    The number of operands (0/1/2).
 */
void extract_operands(char *line, int command_start_i, int command_len, char *src, char *dest, int operand_count);

/**
 * @brief Encodes a full assembly command line into machine words.
 *
 * Builds command words, and adds them to the code section.
 *
 * @param table     The assembler table to store commands.
 * @param opcode    The numeric opcode of the command.
 * @param src_oper  Source operand, NULL if there isnt one.
 * @param dest_oper Destination operand, NULL if there isnt one.
 * @param lbl       The name of the label
 */
void encode_command(assembler_table *table, int opcode, char *src_operand, char *dest_operand, char *lbl);

/**
 * @brief Converts a command_parts structure into a machine word.
 *
 * @param parts A command_parts struct.
 * @return The binary machine word as unsigned short.
 */
unsigned short command_to_short(command_parts *parts);

/**
 * @brief Builds the extra word that encodes the two index-registers of a matrix operand.
 *
 * @param op The operand.
 *
 * @return The 16 bit word with the two register IDs stored in their places.
 */
unsigned short build_matrix_reg_word(const char *op);



/* ############################### Directives ############################### */

/**
 * @brief Parses a directive line, and updates the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param line         The line being parsed.
 * @param error_count  The number of errors found so far.
 * @param directive    The directive word being processed.
 */
void add_directive(assembler_table *table, char *line, int *error_count, char *directive);



/* ############################### Labels ############################### */

/**
 * @brief Adds a new label with address and type to the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param lbl          The label name.
 * @param type         The label type.
 * @param error_count  The number of errors found so far.
 */
void add_label_to_table(assembler_table *table, char *lbl, int type, int *error_count);

/**
 * @brief Extracts the label name from a line.
 *
 * @param line  The line to check.
 * @param i     The starting index of the line.
 *
 * @return The label name, or NULL if not found.
 */
char *get_label(char *line, int i);

/**
 * Checks if the given label already exists in the label list.
 *
 * @param list   The head of the label list.
 * @param label  The label name to check.
 *
 * @return true (1) if the label exists, false (0) otherwise.
 */
int check_for_label(label *list, char *label);


/* ############################### External/Entry labels ############################### */

/**
 * @brief Adds external label to table.
 *
 * @param table         The assembler's table structure.
 * @param name      
 * @param error_count
 */
void add_external_label_to_table(assembler_table *table, char *name, int *error_count);


/**
 * @brief Checks if there's a CODE\DATA lable that matches the entry one, and changes the entry's address
 *
 * @param table        The assembler's table.
 * @param error_count  The number of errors found so far.
 */
void add_entry_addresses(assembler_table *table, int *error_count);



/* ############################### Adding Nodes ############################### */

/**
 * @brief Creates a new command node and inserts it into the assembler's code section.
 *
 * This function allocates memory for a new command node, assigns it a binary word,
 * sets its memory address using the current instruction counter (IC).
 * The node is then added to the end of the assembler's command list.
 *
 * @param table            The assembler's table structure.
 * @param word_value       The binary word representing the command.
 * @param referenced_label The name of the label referenced by this command (or "" if not applicable).
 */
void create_and_add_command(assembler_table *table, unsigned short word_value, char *lbl);

/**
 * @brief Adds a new data node to the data list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The data node to add to the list.
 */
void add_data_node(assembler_table *table, data *new_node);

/**
 * @brief Adds a new label node to the label list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The label node to add to the list.
 */
void add_label_node(assembler_table *table, label *new_node);

/**
 * @brief Adds a new command node to the code list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The command node to add to the list.
 */
void add_command_node(assembler_table *table, command *new_node);



/* ############################### Error Checks ############################### */

/**
 * @brief Validates the name of a label.
 *
 * @param label  The label check.
 *
 * @return true (1) if the label is legal, false (0) otherwise.
 */
int is_label_ok(char *label);

/**
 * @brief Checks if the given string is a valid number.
 *
 * @param input  The input to validate.
 *
 * @return 1 if the input is a valid number, 0 otherwise.
 */
int is_number_ok(char *input);


/**
 * @brief Checks if the given word is a valid command.
 *
 * @param word  The word to check.
 *
 * @return true (1) if it's a known command, false (0) otherwise.
 */
int is_command_ok(char *word);

/**
 * @brief Checks if a label is a reserved word in the language (command, directive, etc.).
 *
 * @param label  The label name to check.
 *
 * @return true (1) if it's reserved, false (0) otherwise.
 */
int is_reserved_word(const char *label);



/* ############################### Helpers ############################### */

/**
 * Removes the symbol ';' from the line.
 *
 * @param line The line we want to remove the ; symbol from.
 *
 */
void remove_comment_symbol(char *line);

/* =============== Functions for the first_pass_functions.c file =============== */

/* ############################### Error Checks ############################### */

/**
 * @brief Validates the command line.
 *
 * This function checks whether the operations in the given command in the line are written correctly,
 * including the number and validity of operands, proper comma usage, and correct addressing modes
 * according to the operation type.
 *
 * @param line            The line to check.
 * @param i               The starting index of the line.
 * @param error_count     The number of errors found so far.
 * @param operands_amount The number of operands for the current command.
 * @param command_name    The name of the command.
 * @param label_flag      Boolean variable used to check if this line starts with a label.
 *
 * @return true (1) if the command is valid, false (0) otherwise.
 */
bool check_command(char *line, int i, int *error_count, int operands_amount, char *command, bool label_flag);

/**
 * @brief Validates a command line that should not contain any operands.
 *
 * This function checks whether a command that requires zero operands is followed by an extra argument.
 * If any argument is found after the command, it reports an error and increments the error count.
 *
 * @param line        The line to check.
 * @param i           The starting index of operands in the line.
 * @param error_count The number of errors found so far.
 *
 * @return true (1) if the command line is valid, false (0) otherwise.
 */
bool check_zero_operands(char *line, int i, int *error_count);

 /**
 * @brief Validates a line with one operand.
 *
 * This function checks if the line contains only one valid operand.
 * It verifies that the operand type matches the rules for the specific command.
 *
 * @param line        The full line of the assembly code.
 * @param i           The starting index of the operands in the line.
 * @param error_count The number of errors found so far.
 * @param name        The name of the command.
 * @param operand     The operand to validate.
 *
 * @return true (1) if the operand is valid for the given command, false (0) otherwise.
 */
bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand);

/**
 * @brief Validates a line with two operands.
 *
 * This function checks if the line contains only two operands, separated by a comma,
 * and validates them according to the command.
 *
 * @param line        The line to check.
 * @param i           The starting index of the operands in the line.
 * @param error_count The number of errors found so far.
 * @param name        The name of the command.
 *
 * @return true (1) if both operands are valid, false (0) otherwise.
 */
bool check_two_operands(char *line, int i, int *error_count, char *name);

/**
 * @brief Checks if the given operand is a valid matrix.
 *
 * A valid matrix operand has the form: LABEL[register1][register2]
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a valid matrix, false (0) otherwise.
 */
bool is_matrix(char *operand);

/**
 * @brief Checks if the given operand is a valid immediate.
 *
 * The immediate must begin with '#' followed by a number. (Could be with a '-')
 *
 * @param operand The operand to check.
 *
 * @return true if the operand is a valid immediate, false (0) otherwise.
 */
bool is_immediate(char *operand);

/**
 * @brief Checks if the given operand is a valid register. (r1 -r7)
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a register, false (0) otherwise.
 */
bool is_register(char *operand);

/**
 * @brief Checks if the given operand is a valid label.
 *
 * A valid label must start with a letter, followed by letters or digits ONLY.
 *
 * @param operand The operand string to check.
 *
 * @return true (1) if the operand is a valid label, false (0) otherwise.
 */
bool is_label(char *operand);


#endif /* FIRST_PASS_FUNCTIONS_H */