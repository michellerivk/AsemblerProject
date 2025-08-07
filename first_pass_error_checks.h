#ifndef FIRST_PASS_ERROR_CHECKS_H
#define FIRST_PASS_ERROR_CHECKS_H

#include "assembler.h"

/**
 * @file first_pass_error_checks.h
 * @brief Header file for error checks of the first pass of the assembler.
 *
 * This file contains function declarations from the file:
 * + first_pass_error_checks.c
 *
 * This header declares the functions used during the assembler’s first pass
 * to validate instruction syntax and operand usage. 
 * Validations such as:
 *  + Checksing that each command has the correct number of operands
 *  + Verifing the operand types (register, immediate, label, matrix)
 *  + Updating the instruction (IC) and data (DC) counters
 */

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
bool check_command(char *line, int i, int *error_count, int operands_amount, char *command, bool label_flag, int line_number);

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
bool check_zero_operands(char *line, int i, int *error_count, int line_number);

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
bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand, int line_number);

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
bool check_two_operands(char *line, int i, int *error_count, char *name, int line_number);

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

/**
 * @brief Validates the name of a label.
 *
 * @param label  The label check.
 *
 * @return true (1) if the label is legal, false (0) otherwise.
 */
int is_label_ok(char *label, int line_number);

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

#endif /* FIRST_PASS_ERROR_CHECK_H */