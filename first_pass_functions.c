#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "first_pass_functions.h"

/* ############################### Main Functions ############################### */

/**
 * Validates the given line from the '.am' file.
 *
 * Updates the assembler table and IC/DC if needed.
 *
 * @param line         The line to check.
 * @param line_number  The number of the line in the source file.
 * @param table        The assembler's table structure.
 * @param error_count  The number of errors found so far.
 * @param label_flag   Boolean variable used to check if this line starts with a label.
 */
void check_line(char *line, int line_number, assembler_table *table, int *error_count, bool label_flag)
{
    /* All possible commands */
    const char *commands[] =
        {
            "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc",
            "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
        };

    int com_len = sizeof(commands) / sizeof(commands[0]); /* The total size of the commands array */

    /* All possible directives */
    const char *directives[] =
        {
            ".data", ".string", ".mat", ".entry", ".extern"
        };

    int dir_len = sizeof(directives) / sizeof(directives[0]); /* The total size of the directives array */
    char command_name[MAX_LINE_LENGTH];
    char src[MAX_LINE_LENGTH] = "", dest[MAX_LINE_LENGTH] = "";
    int i = 0;

    remove_comment_symbol(line); /* Removes comment symbol */

    check_double_comma(line, line_number, error_count); /* Checks for double comma */

    /* The line doesn't have a label */
    if (!strchr(line, ':'))
    {
        /* Checks if there's a command */
        int len = check_word(line, i, commands, com_len, error_count, label_flag, line_number);

        if (len != -1)
        {
            char *tmp = update_ic(line, i, commands, com_len, table, error_count, label_flag, line_number);

            if (!tmp) 
            {
                return;
            }

            strcpy(command_name, tmp);

            free(tmp); /* free allocated memory caused by update_ic() */

            extract_operands(line, i, strlen(command_name), src, dest, get_instruction(command_name));

            encode_command(table, check_command_value(command_name), src, dest, NULL);
        }

        else
        {         

            /* Checks if there's a directive */
            len = check_word(line, i, directives, dir_len, error_count, label_flag, line_number);

            if (len != -1)
            {
                /* Gets the directive */
                char directive[MAX_LINE_LENGTH];
                strncpy(directive, line, len);
                directive[len] = '\0';

                /* Handles the directive */
                add_directive(table, line, error_count, directive, line_number);
            }

            /* If there is no directive nor command, it will do nothing */
            else
            {
                if (line[0] == '\n') /* Empty lines are valid */
                {
                    return;
                }
                else /* There's an error with the line */
                {
                    /* Checks if the opcode appears in the line */
                    if(contains_any_word(line, commands, com_len) || contains_any_word(line, directives, dir_len))
                    {
                        /* If opcode appears, at the middle of line, there's a problem with the label */
                        if (find_any_word_index(line, commands, com_len) != 0 && find_any_word_index(line, directives, dir_len) != 0)
                        {
                            first_pass_errors(ERR_LABEL_ENDING, line_number, -1);
                            (*error_count)++;
                            return;
                        }
                        
                        return;
                    }
                    /* Otherwise the problem is with the op code */
                    else
                    {
                        first_pass_errors(ERR_OPCODE, line_number, -1);
                        (*error_count)++;
                        return;
                    }
                }
            }
        }
    }

    /* The line has a label */
    else
    {
        /* Turns on label_flag */
        label_flag = true;

        /* Checks if the label is valid */
        if (is_label_ok(line, line_number))
        {
            bool good_label;
            int len;

            good_label = add_label(table, line, i, error_count, commands, directives, com_len, dir_len, label_flag, line_number);

            if (good_label == false)
            {
                first_pass_errors(ERR_NOT_COMMAND_OR_DIRECTIVE, line_number, -1);
                (*error_count)++;
                return;
            }

            /* Skip the label */
            while (line[i] && line[i] != ':')
            {
                i++;
            }
            if (line[i] == ':')
            {
                i++;
            }

            /* Checks if there is a directive after the label */
            len = check_word(line, i, directives, dir_len, error_count, label_flag, line_number);
            if (len != -1)
            {
                /* Gets the name of the directive */
                char directive[MAX_LINE_LENGTH];
                strncpy(directive, &line[i], len);
                directive[len] = '\0';

                /* Adds the directive to the table */
                add_directive(table, line, error_count, directive, line_number);
                return;
            }

            /* Checks if there's a command after the label */
            len = check_word(line, i, commands, com_len, error_count, label_flag, line_number);

            if (len != -1)
            {
                /* Updates the IC, and gets the name of the command */
                char *tmp = update_ic(line, i, commands, com_len, table, error_count, label_flag, line_number);

                if (!tmp) 
                {
                    return;
                }

                strcpy(command_name, tmp);

                free(tmp); /* free allocated memory caused by update_ic() */

                extract_operands(line, i, strlen(command_name), src, dest, get_instruction(command_name));

                encode_command(table, check_command_value(command_name), src, dest, NULL);

                return;
            }

            /* Prints error if no */
            first_pass_errors(ERR_NOT_COMMAND_OR_DIRECTIVE, line_number, -1);
            (*error_count)++;
            return;
        }

        /* Sends an error if the label is not valid */
        else
        {
            (*error_count)++; /* The errors are called for in the is_label_ok() function */
            return;
        }
    }
}

/**
 * Updates the instruction counter based on the number of operands in a command.
 *
 * @param line         The line to check.
 * @param i            The index after the label.
 * @param commands     List of command names.
 * @param com_len      Number of the commands.
 * @param table        The assembler's table structure.
 * @param error_count  The number of errors found so far.
 * @param label_flag   Boolean variable used to check if this line starts with a label.
 * @param line_number  The number of the line in the source file.
 */
char *update_ic(char *line, int i, const char *commands[], int com_len,
                assembler_table *table, int *error_count, bool label_flag, int line_number)
{
    int len = check_word(line, i, commands, com_len, error_count, label_flag, line_number);

    if (len != -1)
    {
        char *command_name = my_malloc(MAX_LINE_LENGTH);

        strncpy(command_name, &line[i], len);
        command_name[len] = '\0';

        return command_name;
    };

    return NULL;
}

/**
 * Gets the label from the line and adds it to the assembler's table,
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
 * @param line_number  The number of the line in the source file.
 *
 * @return true (1) if label was added successfully, false (0) otherwise.
 */
int add_label(assembler_table *table, char *line, int i, int *error_count,
              const char *commands[], const char *directives[], int commands_len,
              int directives_len, bool label_flag, int line_number)
{
    int len;
    char *lbl = get_label(line, i); /* Gets the label name out of the line */
    int type = -1;

    while (line[i] && line[i] != ':')
    {
        i++;
    }

    if (line[i] == ':')
    {
        i++;
    }

    /* Checks if the next words matches a directive */
    len = check_word(line, i, directives, directives_len, error_count, label_flag, line_number);

    if (len != -1)
    {
        if (strncmp(&line[i], ".data", len) == 0 || strncmp(&line[i], ".string", len) == 0 ||
            strncmp(&line[i], ".mat", len) == 0)
        {
            type = DATA;
        }
        else if (strncmp(&line[i], ".extern", len) == 0)
        {
            type = EXTERNAL;
        }
        else if (strncmp(&line[i], ".entry", len) == 0)
        {
            type = ENTRY;
        }
        else
        {
            first_pass_errors(ERR_UNKNOWN_DIRECTIVE, line_number, -1);
            (*error_count)++;
            free(lbl);
            return false;
        }
    }
    else
    {
        /* Checks if the next words matches a command */
        len = check_word(line, i, commands, commands_len, error_count, label_flag, line_number);

        if (len != -1)
        {
            type = CODE;
        }
    }

    /* Checks if the type was changed */
    if (type != -1)
    {
        add_label_to_table(table, lbl, type, error_count);
        free(lbl);
        return true;
    }

    else
    {
        /* If it wasnt a command nor a directive*/
        free(lbl);
        return false;
    }
}

/* ############################### Command Parsing and Encoding ############################### */

/**
 * Checks if the command or directive exists in the given list of words.
 *
 * @param line         The line to check.
 * @param start        The index in the line where the word begins.
 * @param words        Array of valid directives / commands.
 * @param amount       Number of words in the list.
 * @param error_count  The number of errors found so far.
 * @param label_flag   A flag to indicate if the line includes a label before the word.
 * @param line_number  The number of the line in the source file.
 *
 * @return The length of the word found (if it's valid), or -1 otherwise.
 */
int check_word(char *line, int start, const char *words[], int amount, int *error_count, bool label_flag, int line_number)
{
    int i;
    for (i = 0; i < amount; i++)
    {
        int len = strlen(words[i]); /* Gets the length of each word in the list */
        /* Compares the first letters of the line according to the length of the word to check if it's the same */
        if (strncmp(&line[start], words[i], len) == 0)
        {
            char command[10];
            strncpy(command, &line[start], len);
            command[len] = '\0';
            if (strchr(line, '.') != NULL)
            {
                return len;
            }
            /* Checks if the operands in the command are valid */
            if (check_command(line, len, error_count, get_instruction(command), command, label_flag, line_number))
            {
                return len;
            }
            else
            {
                return -1;
            }
        }
    }
    return -1;
}

/**
 * Returns the opcode value of the given command name.
 *
 * @param com The command name.
 * @return The opcode of the command, or -1 if the command is unknown.
 */
int check_command_value(char *com)
{
    if (strcmp(com, "mov") == 0)
        return 0;
    if (strcmp(com, "cmp") == 0)
        return 1;
    if (strcmp(com, "add") == 0)
        return 2;
    if (strcmp(com, "sub") == 0)
        return 3;
    if (strcmp(com, "not") == 0)
        return 4;
    if (strcmp(com, "clr") == 0)
        return 5;
    if (strcmp(com, "lea") == 0)
        return 6;
    if (strcmp(com, "inc") == 0)
        return 7;
    if (strcmp(com, "dec") == 0)
        return 8;
    if (strcmp(com, "jmp") == 0)
        return 9;
    if (strcmp(com, "bne") == 0)
        return 10;
    if (strcmp(com, "red") == 0)
        return 11;
    if (strcmp(com, "prn") == 0)
        return 12;
    if (strcmp(com, "jsr") == 0)
        return 13;
    if (strcmp(com, "rts") == 0)
        return 14;
    if (strcmp(com, "stop") == 0)
        return 15;

    return -1;
}

/**
 * Returns the number of operands required by the given command.
 *
 * @param com  The command name.
 *
 * @return 0, 1, or 2 decided by the command.
 */
int get_instruction(char *com)
{
    if (strcmp(com, "stop") == 0 || strcmp(com, "rts") == 0)
    {
        return 0;
    }
    else if (strcmp(com, "jmp") == 0 || strcmp(com, "bne") == 0 || strcmp(com, "jsr") == 0 || strcmp(com, "prn") == 0 ||
             strcmp(com, "not") == 0 || strcmp(com, "clr") == 0 || strcmp(com, "inc") == 0 || strcmp(com, "dec") == 0 ||
             strcmp(com, "red") == 0)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

/**
 * Validates an operand and returns its addressing-mode code.
 *
 * @param operand  The operand.
 *
 * @return The addressing-mode code of the operand.
 */
int get_addressing_mode(char *operand)
{
    if (is_immediate(operand))
        return 0;
    if (is_register(operand))
        return 3;
    if (is_matrix(operand))
        return 2;
    return 1;
}

/**
 * Extracts source and destination operands from a line after the command.
 *
 * @param line             The full assembly line.
 * @param command_start_i  The index where the command starts.
 * @param command_len      The length of the command.
 * @param src              The source operand.
 * @param dest             The destination operand.
 * @param operand_count    The number of operands.
 */
void extract_operands(char *line, int command_start_i, int command_len, char *src, char *dest, int operand_count)
{
    char *first = NULL; /* stores the src operand */
    char *second = NULL; /* stores the dst operand */
    char *after_command = &line[command_start_i + command_len];
    char copy[MAX_LINE_LENGTH]; /* stores a copy of the line variable */

    strcpy(copy, after_command);

    first = strtok(copy, ", \t\n");
    second = strtok(NULL, ", \t\n");

    if (operand_count == 1 && first)
    {
        strcpy(dest, first);
    }
    else if (operand_count == 2 && first && second)
    {
        strcpy(src, first);
        strcpy(dest, second);
    }
    else
    {
        src[0] = dest[0] = '\0';
    }
}

/**
 * Builds command words, and adds them to the code section.
 *
 * @param table     The assembler table to store commands.
 * @param opcode    The numeric opcode of the command.
 * @param src_oper  Source operand, NULL if there isnt one.
 * @param dest_oper Destination operand, NULL if there isnt one.
 * @param lbl       The name of the label
 */
void encode_command(assembler_table *table, int opcode, char *src_oper, char *dest_oper, char *lbl)
{
    /* source and destination operands */
    int src_mode;
    int dest_mode;

    /* full machine word */
    command_parts word;

    /* Checks if there is a source operand */
    if (src_oper != NULL && src_oper[0] != '\0')
    {
        /* Checks if it's a register, immediate or a matrix*/
        src_mode = get_addressing_mode(src_oper);
    }
    else
    {
        /* If none of them, -1 */
        src_mode = -1;
    }

    /* Checks if there is a destenation operand */
    if (dest_oper != NULL && dest_oper[0] != '\0')
    {
        /* Checks if it's a register, immediate or a matrix*/
        dest_mode = get_addressing_mode(dest_oper);
    }
    else
    {
        /* If none of them, -1 */
        dest_mode = -1;
    }

    /* Stores the opcode value in the struct */
    word.opcode = opcode;

    /* Stores the source address value, if there is one */
    if (src_mode == -1)
        word.source_addr = 0; /* Stores 0 if there isnt */
    else
        word.source_addr = src_mode;

    /* Stores the destination address value, if there is one */
    if (dest_mode == -1)
        word.dest_addr = 0; /* Stores 0 if there isnt */
    else
        word.dest_addr = dest_mode;

    /* ARE bits = 0 during the first pass */
    word.ARE = 0;

    create_and_add_command(table, command_to_short(&word), lbl);

    /* Checks is both operands are registers */
    if (src_mode == 3 && dest_mode == 3)
    {
        unsigned short reg_word = (atoi(src_oper + 1) << 6) | (atoi(dest_oper + 1) << 2);
        create_and_add_command(table, reg_word, NULL);
        return;
    }

    /* Handles the source operand */
    if (src_mode != -1)
    {
        switch (src_mode)
        {
        case 0: /* Immediate */
        {
            int val_src = atoi(src_oper + 1); /* Skips # */
            create_and_add_command(table, (val_src << 2), NULL);
            break;
        }
        case 1: /* Label */
        {
            create_and_add_command(table, 0, src_oper);
            break;
        }

        case 2: /* Matrix */
            create_and_add_command(table, 0, src_oper);
            create_and_add_command(table, build_matrix_reg_word(src_oper), NULL);
            break;

        case 3: /* Register */
        {
            unsigned short reg_word = (atoi(src_oper + 1) << 6);
            create_and_add_command(table, reg_word, NULL);
            break;
        }
        }
    }

    /* Handles the destination operand */
    if (dest_mode != -1)
    {
        switch (dest_mode)
        {
        case 0: /* Immediate */
        {
            int val_dst = atoi(dest_oper + 1);
            create_and_add_command(table, (val_dst << 2), NULL);
            break;
        }
        case 1: /* Lable */
            create_and_add_command(table, 0, dest_oper);
            break;

        case 2: /* Matrix */
            create_and_add_command(table, 0, dest_oper);
            create_and_add_command(table, build_matrix_reg_word(dest_oper), NULL);
            break;

        case 3: /* Register */
        {
            unsigned short reg_word = (atoi(dest_oper + 1) << 2);
            create_and_add_command(table, reg_word, NULL);
            break;
        }
        }
    }
}

/**
 * Converts a command_parts structure into a machine word.
 *
 * @param parts A command_parts struct.
 * @return The binary machine word as unsigned short.
 */
unsigned short command_to_short(command_parts *parts)
{
    unsigned short result = 0;

    result |= (parts->opcode & 0xF) << 6;      /* Bits 6 - 9 */
    result |= (parts->source_addr & 0x3) << 4; /* Bits 4 - 5 */
    result |= (parts->dest_addr & 0x3) << 2;   /* Bits 2 - 3 */
    result |= (parts->ARE & 0x3);              /* Bits 0 - 1 */

    return result;
}

/**
 * Builds the extra word that encodes the two index-registers of a matrix operand.
 *
 * @param op The operand.
 *
 * @return The 16-bit word with the two register IDs stored in their places.
 */
unsigned short build_matrix_reg_word(const char *op)
{
    int r1 = 0, r2 = 0;
    char *left = strchr(op, '[');
    char *right = strrchr(op, '[');

    if (!left || !right)
        return 0;

    if (left[1] == 'r' && isdigit(left[2]))
        r1 = left[2] - '0';

    if (right[1] == 'r' && isdigit(right[2]))
        r2 = right[2] - '0';

    return (r1 << 6) | (r2 << 2);
}

/* ############################### Directives ############################### */

/**
 * Parses a directive line, and updates the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param line         The line being parsed.
 * @param error_count  The number of errors found so far.
 * @param directive    The directive word being processed.
 * @param line_number  The number of the line in the source file.
 */
void add_directive(assembler_table *table, char *line, int *error_count, char *directive, int line_number)
{
    if (strcmp(directive, ".data") == 0)
    {
        char *input = strstr(line, ".data");

        /* Checks .data exists */
        if (input != NULL)
        {
            char copy_of_line[MAX_LINE_LENGTH];
            char *arg;

            int value;
            if (input != NULL)
            {
                int i = (int)(input - line);
                i += 5;
                input = &line[i];
            }

            /* Copies the line to not ruin it accidentally */
            strcpy(copy_of_line, input);

            /* Splits the part with the , into mutiple arguments */
            arg = strtok(copy_of_line, ",");
            while (arg != NULL)
            {
                data *new_directive = (data *)my_malloc(sizeof(data));

                /* Checks if the number is valid */
                if (!is_number_ok(arg))
                {
                    free(new_directive);
                    first_pass_errors(ERR_NOT_A_NUMBER, line_number, -1);
                    (*error_count)++;
                    return;
                }

                value = atoi(arg);

                new_directive->word.value = value;
                new_directive->address = table->data_counter;
                new_directive->next = NULL;

                /* Add the node to the list */
                add_data_node(table, new_directive);

                arg = strtok(NULL, ",");

            }
        }
    }
    else if (strcmp(directive, ".string") == 0)
    {
        char *input = strstr(line, ".string");

        if (input != NULL)
        {
            data *null_node;
            int i = (int)(input - line);
            i += 7;

            if (line[i] != '\"')
            {
                first_pass_errors(ERR_NO_QUOTATION_MARKS, line_number, -1);
                (*error_count)++;
                return;
            }

            i++; /* Skips the first " */

            while (line[i] != '\0' && line[i] != '\"')
            {
                data *new_directive = (data *)my_malloc(sizeof(data));

                new_directive->word.value = (int)line[i];
                new_directive->address = table->data_counter;
                new_directive->next = NULL;

                add_data_node(table, new_directive);

                i++;
            }

            /* Checks if the string ends with '"' */
            if (line[i] != '\"')
            {
                first_pass_errors(ERR_CLOSING_QUOTATION_MARK, line_number, -1);
                (*error_count)++;
                return;
            }

            /* Adds 0 at the end of the string list */
            null_node = (data *)my_malloc(sizeof(data));

            null_node->word.value = 0;
            null_node->address = table->data_counter;
            null_node->next = NULL;

            /* Adds the node to the list */
            add_data_node(table, null_node);
        }
    }
    else if (strcmp(directive, ".mat") == 0)
    {
        int rows, cols, j;
        int count = 0; /* Counts the amount of numbers in the matrix that were given */

        char *input = strstr(line, ".mat"); /* The start of .mat */
        int i = (int)(input - line);

        char copy_of_line[MAX_LINE_LENGTH]; /* Holds a copy of our line, so that it won't be ruined */

        char *arg;

        i += 4; /* Skips .mat */

        input = &line[i];

        /* Checks if the input of the matrix is valid */
        if (sscanf(input, "[%d][%d]", &rows, &cols) == 0)
        {
            first_pass_errors(ERR_INVALID_MATRIX, line_number, -1);
            (*error_count)++;
            return;
        }

        /* Gets to the arguments part */
        for (j = 0; j < 2; j++)
        {
            while (line[i] != '\0' && line[i] != ']')
            {
                i++;
            }

            if (line[i] == ']')
            {
                i++;
            }
        }

        /* Makes copy of the line */
        strncpy(copy_of_line, &line[i], MAX_LINE_LENGTH - 1);

        /* Ends the line */
        copy_of_line[MAX_LINE_LENGTH - 1] = '\0';

        /* Seperates arguments */
        arg = strtok(copy_of_line, ",");

        while (arg != NULL)
        {
            data *mat_node = (data *)my_malloc(sizeof(data)); /* The node we're adding to the table */


            /* Skips white spaces */
            while (*arg == ' ' || *arg == '\t')
            {
                arg++;
            }

            /* Checks if valid arguments were given */
            if (!is_number_ok(arg))
            {
                free(mat_node);
                first_pass_errors(ERR_INVALID_MAT_ARGUMENT, line_number, -1);
                (*error_count)++;
                return;
            }

            /* Fills the table */
            mat_node->word.value = atoi(arg);
            mat_node->address = table->data_counter;
            mat_node->next = NULL;

            /* Add the directive to the table */
            add_data_node(table, mat_node);

            count++; /* Adds an argument to the counter */

            arg = strtok(NULL, ",");
        }

        /* Checks if the amount of numbers is exactly the amount neede to fill the given matrix */
        if (count != rows * cols)
        {
            first_pass_errors(ERR_MAT_WRONG_AMOUNT_OF_VALUES, line_number, -1);
            (*error_count)++;
        }
    }
    else if (strcmp(directive, ".entry") == 0)
    {
        int type = ENTRY;

        if (strstr(line, ".entry"))
        {
            char *input = strstr(line, ".entry");

            if (input != NULL)
            {
                char lbl[MAX_LABEL_LENGTH];
                int j = 0;
                int i = (int)(input - line);
                i += 6;

                while (line[i] != '\0' && line[i] != ' ' && line[i] != '\n')
                {
                    lbl[j++] = line[i++];
                }

                lbl[j] = '\0';

                add_label_to_table(table, lbl, type, error_count);
            }
        }
        /* ###############Second Pass################### */
    }
    else if (strcmp(directive, ".extern") == 0)
    {
        /*int type = EXTERNAL;*/
        char *input = strstr(line, ".extern");

        if (input != NULL)
        {
            char lbl[MAX_LABEL_LENGTH];
            int j = 0;
            int i = (int)(input - line);
            i += 7;

            while (line[i] != '\0' && line[i] != ' ' && line[i] != '\n')
            {
                lbl[j++] = line[i++];
            }

            lbl[j] = '\0';

            add_external_label_to_table(table, lbl, error_count, line_number);
        }
    }
}

/* ############################### Labels ############################### */

/**
 * Adds a new label with address and type to the assembler table.
 *
 * @param table        The assembler's table.
 * @param lbl          The label name.
 * @param type         The label type.
 * @param error_count  The number of errors found so far.
 */
void add_label_to_table(assembler_table *table, char *lbl, int type, int *error_count)
{
    label *new_label = (label *)my_malloc(sizeof(label));

    /* Inserts the name of the label into the table */
    strcpy(new_label->name, lbl);

    if (type == CODE)
    {
        new_label->address = table->instruction_counter;
    }

    else if (type == DATA)
    {
        new_label->address = table->data_counter + table->instruction_counter;
    }

    new_label->type = type;
    new_label->next = NULL;

    /* add the lable to the list */
    add_label_node(table, new_label);
}

/**
 * Extracts the label name from a line.
 *
 * @param line  TThe line to check.
 * @param i     The starting index of the line.
 *
 * @return The label name, or NULL if not found.
 */
char *get_label(char *line, int i)
{
    int len = 0;
    char *label;

    /* Goes to the end of label or to the end of the line */
    while (line[i + len] != ':' && line[i + len] != '\0' && len < MAX_LABEL_LENGTH - 1)
    {
        len++;
    }

    /* Checks if there are ':'. If no, there's no label -> returns NULL */
    if (line[i + len] != ':')
    {
        return NULL;
    }

    label = (char *)my_malloc(len + 1);

    /* Copies the label name into 'label' */
    memcpy(label, &line[i], len);

    /* Ends the label */
    label[len] = '\0';

    return label;
}

/* ############################### External/Entry labels ############################### */

/**
 * Adds an external label to the external labels table
 *
 * @param table        The assembler's table.
 * @param name         The label name.
 * @param error_count  The number of errors found so far.
 * @param line_number  The number of the line in the source file.
 */
void add_external_label_to_table(assembler_table *table, char *name, int *error_count, int line_number)
{
    external_label *lbl = table->external_list;

    /* Checks if there is already a label with this name in the list */
    while (lbl)
    {
        if (strcmp(lbl->label, name) == 0)
        {
            first_pass_errors(ERR_EXTERNAL_LABEL_EXISTS, line_number, -1);
            (*error_count)++;
            return;
        }
        lbl = lbl->next;
    }

    /* Adds a node to the list */
    lbl = my_malloc(sizeof(external_label));

    strcpy(lbl->label, name);
    lbl->label[strlen(name)] = '\0';
    lbl->usage_list = NULL;
    lbl->next = NULL;

    /* Puts it in the head */
    lbl->next = table->external_list;
    table->external_list = lbl;
}

/**
 * Checks if there's a CODE\DATA lable that matches the entry one, and changes the entry's address
 *
 * @param table        The assembler's table.
 * @param error_count  The number of errors found so far.
 */
void add_entry_addresses(assembler_table *table, int *error_count)
{
    label *entry = table->label_list;

    while (entry != NULL)
    {
        if (entry->type == ENTRY)
        {
            label *defined = table->label_list;
            while (defined != NULL)
            {
                if (strcmp(entry->name, defined->name) == 0 && defined != entry && defined->type != ENTRY)
                {
                    entry->address = defined->address;
                    break;
                }
                defined = defined->next;
            }
        }

        entry = entry->next;
    }
}

/* ############################### Adding Nodes ############################### */

/**
 * Stores a new command node to the command list.
 *
 * @param table            The assembler table to store commands.
 * @param word_value       The binary word representing the command.
 * @param referenced_label The name of the label referenced by this command. Null if there is no label.
 */
void create_and_add_command(assembler_table *table, unsigned short word_value, char *lbl)
{
    /*external_label *ext = NULL;*/
    command *new_command = (command *)my_malloc(sizeof(command));

    new_command->word.value = word_value;
    new_command->address = table->instruction_counter;

    /* Gets base label */
    if (lbl != NULL && lbl[0] != '\0')
    {
        /* Checks if it's a matrix operand */
        if (strchr(lbl, '['))
        {
            char copy_of_label[MAX_LABEL_LENGTH];
            char matrix_label[MAX_LABEL_LENGTH];
            int i = 0;

            strcpy(copy_of_label, lbl);
            while (copy_of_label[i] != '[')
            {
                matrix_label[i] = copy_of_label[i];
                i++;
            }
            matrix_label[i] = '\0';

            strcpy(new_command->referenced_label, matrix_label);
        }

        else
        {
            strncpy(new_command->referenced_label, lbl, MAX_LABEL_LENGTH - 1);
            new_command->referenced_label[MAX_LABEL_LENGTH - 1] = '\0';
        }
    }
    else
    {
        /* no label reference for this word */
        new_command->referenced_label[0] = '\0';
    }

    new_command->next = NULL;

    add_command_node(table, new_command);

    table->instruction_counter++;
}

/**
 * Adds a new data node to the data list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The data node to add to the list.
 */
void add_data_node(assembler_table *table, data *new_node)
{
    /* Adds the first node if there were none before */
    if (table->data_section == NULL)
    {
        table->data_section = new_node;
    }

    /* Adds the node to the end of the list */
    else
    {
        data *temp = table->data_section;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }

    table->data_counter++;
}

/**
 * Adds a new label node to the label list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The label node to add to the list.
 */
void add_label_node(assembler_table *table, label *new_node)
{
    /* Adds the first node if there were none before */
    if (table->label_list == NULL)
    {
        table->label_list = new_node;
    }

    /* Adds the node to the end of the list */
    else
    {
        label *temp = table->label_list;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}

/**
 * Adds a new command node to the code list in the assembler table.
 *
 * @param table     The assembler's table structure.
 * @param new_node  The command node to add to the list.
 */
void add_command_node(assembler_table *table, command *new_node)
{
    /* Adds the first node if there were none before */
    if (table->code_section == NULL)
    {
        table->code_section = new_node;
    }
    /* Adds the node to the end of the list */
    else
    {
        command *temp = table->code_section;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
}
