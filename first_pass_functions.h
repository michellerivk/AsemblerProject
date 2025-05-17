#include "assembler.h"

/* ############################### Functions declarations ############################### */

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
 * @brief Extracts the label name from a line.
 *
 * @param line  TThe line to check.
 * @param i     The starting index of the line.
 *
 * @return The label name, or NULL if not found.
 */
char *get_label(char *line, int i);

/**
 * @brief Checks if the given word is a valid command.
 *
 * @param word  The word to check.
 *
 * @return true (1) if it's a known command, false (0) otherwise.
 */
int is_command_ok(char *word);

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
 * @brief Checks if the given label already exists in the label list.
 *
 * @param list   The head of the label list.
 * @param label  The label name to check.
 *
 * @return true (1) if the label exists, false (0) otherwise.
 */
int check_for_label(label *list, char *label);

/**
 * @brief Checks if the given string is a valid number.
 *
 * @param input  The input to validate.
 *
 * @return 1 if the input is a valid number, 0 otherwise.
 */
int is_number_ok(char *input);

/**
 * @brief Returns the number of operands required by the given command.
 *
 * @param com  The command name.
 *
 * @return 0, 1, or 2 decided by the command.
 */
int get_instruction(char *com);

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
 * @brief Parses a directive line, and updates the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param line         The line being parsed.
 * @param error_count  The number of errors found so far.
 * @param directive    The directive word being processed.
 */
void add_directive(assembler_table *table, char *line, int *error_count, char *directive);

/**
 * @brief Gets the label from the line and adds it to the assembler's table,
 * deciding if it's followed by a directive or a command.
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
 * @brief Checks if a label is a reserved word in the language (command, directive, etc.).
 *
 * @param label  The label name to check.
 *
 * @return true (1) if it's reserved, false (0) otherwise.
 */
int is_reserved_word(const char *label);

/**
 * Validates the name of a label.
 *
 * @param label  The label check.
 *
 * @return true (1) if the label is legal, false (0) otherwise.
 */
int is_label_ok(char *label);

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
void update_ic(char *line, int i, const char *commands[], int com_len, 
               assembler_table *table, int *error_count, bool label_flag);


/* ############################### Error Check declarations ############################### */

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
 * Checks if the given operand is a valid matrix.
 *
 * A valid matrix operand has the form: LABEL[register1][register2]
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a valid matrix, false (0) otherwise.
 */
bool is_matrix(char *operand);

/**
 * Checks if the given operand is a valid immediate.
 *
 * The immediate must begin with '#' followed by a number. (Could be with a '-')
 *
 * @param operand The operand to check.
 *
 * @return true if the operand is a valid immediate, false (0) otherwise.
 */
bool is_immediate(char *operand);

/**
 * Checks if the given operand is a valid register. (r1 -r7)
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a register, false (0) otherwise.
 */
bool is_register(char *operand);

/**
 * Checks if the given operand is a valid label.
 *
 * A valid label must start with a letter, followed by letters or digits ONLY.
 *
 * @param operand The operand string to check.
 *
 * @return true (1) if the operand is a valid label, false (0) otherwise.
 */
bool is_label(char *operand);
