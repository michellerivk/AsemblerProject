#ifndef FIRST_PASS_HELPERS_H
#define FIRST_PASS_HELPERS_H

#include "assembler.h"
#include "first_pass_error_checks.h"

/**
 * @file first_pass_functions.h
 * @brief Header file for fucntions of the first pass of the assembler.
 *
 * This file contains function declarations from the file:
 * + first_pass_helpers.c
 *
 * This header declares functions used during the assembler’s first pass
 * to preprocess lines, such as:
 * + Removing comment symbols 
 * + Searching for command or directive keywords within a line
 * + Checking for a double comma 
 */

/**
 * Removes the symbol ';' from the line.
 *
 * @param line The line we want to remove the ; symbol from.
 *
 */
void remove_comment_symbol(char *line);

/**
 * Returns true (1) if any of the given words appears in the line.
 * Otherwise returns false (0).
 * 
 * @param line      The line we're looking for our word in.
 * @param words     The list of words we're looking for (Either commands or directives)
 * @param amount    The amount of words in the list.
 */
int contains_any_word(const char *line, const char *words[], int amount);

/** 
 * Returns the index of the word in the line
 * 
 * @param line      The line we're looking for our word in.
 * @param words     The list of words we're looking for (Either commands or directives)
 * @param amount    The amount of words in the list.
 */
int find_any_word_index(const char *line, const char *words[], int amount);

/**
 * Checks if there's a double comma in the line.
 *
 * @param line The line to check.
 *
 * Prints an error if there is a double comma.
 */
void check_double_comma(char *line, int line_number, int *error_count);

#endif /* FIRST_PASS_HELPERS_H */