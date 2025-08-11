#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "first_pass_functions.h"

/**
 * Returns true (1) if any of the given words appears in the line.
 * Otherwise returns false (0).
 * 
 * @param line      The line we're looking for our word in.
 * @param words     The list of words we're looking for (Either commands or directives)
 * @param amount    The amount of words in the list.
 */
int contains_any_word(const char *line, const char *words[], int amount)
{
    int i;
    bool exists = false;

    for (i = 0; i < amount; i++) 
    {
        if (strstr(line, words[i]) != NULL) 
        {
            exists = true;        
        }
    }

    return exists;
}

/** 
 * Returns the index of the word in the line
 * 
 * @param line      The line we're looking for our word in.
 * @param words     The list of words we're looking for (Either commands or directives)
 * @param amount    The amount of words in the list.
 */
int find_any_word_index(const char *line, const char *words[], int amount)
{
    int i;
    const char *indx;

    for (i = 0; i < amount; i++) 
    {
        indx = strstr(line, words[i]);
        if (indx) 
        {
            return (int)(indx - line); /* Returns the number of characters between the word and the start. */
        }
    }
    return -1;
}

/**
 * Removes the comment from the line.
 *
 * @param line The line we want to remove the ; symbol from.
 *
 */
void remove_comment_symbol(char *line)
{
    char *comment = strchr(line, ';');
    if (comment)
    {
        *comment = '\n';
    }
}

/**
 * Checks if there's a double comma in the line. Prints an error if yes.
 *
 * @param line The line to check.
 * 
 */
void check_double_comma(char *line, int line_number, int *error_count)
{
    int i;
    char copy_of_line[MAX_LINE_LENGTH];

    strcpy(copy_of_line, line);

    for (i = 0; copy_of_line[i]; i++) {
        if ((copy_of_line[i] == ',' && copy_of_line[i+1] == ',') ||       
            (i==0 && copy_of_line[i]==',')           ||         
            (copy_of_line[i]==',' && copy_of_line[i+1]=='\0')) 
        {          
            first_pass_errors(ERR_DOUBLE_COMMA, line_number, -1);
            (*error_count)++;
        }
    }
}