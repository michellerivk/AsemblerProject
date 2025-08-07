#include "first_pass_functions.h"
#include <string.h>

/* ############################### Error checks for the given line ############################### */

/**
 * Checks if the operations in the given command in the line are correct,
 * according to the operation type.
 *
 * @param line            The line to check.
 * @param i               The starting index of the line.
 * @param error_count     The number of errors found so far.
 * @param operands_amount The number of operands for the current command.
 * @param command_name    The name of the command.
 * @param label_flag      Boolean variable used to check if this line starts with a label.
 * @param line_number     The number of the line being checked
 *
 * @return true (1) if the command is valid, false (0) otherwise.
 */
bool check_command(char *line, int i, int *error_count, int operands_amount, char *command_name, bool label_flag, int line_number)
{
    char operand[MAX_LINE_LENGTH];
    char copy_of_line[MAX_LINE_LENGTH];
    int j = 0;

    strcpy(copy_of_line, line);

    /* Skips the label - if exists */
    if (label_flag == true)
    {
       while (copy_of_line[j] != ':')
        {
            j++;
        }
        if (copy_of_line[j] == ':')
        {
            j++;
        }

        i = j;

        for (j = 0; j < strlen(command_name); j++)
        {
            i++;
        }
    }
    
    strcpy(operand, &line[i]);
    

    
    switch (operands_amount) 
    {
        case 0:
            return check_zero_operands(line, i, error_count, line_number);
        case 1:
            return check_one_operands(line, i, error_count, command_name, operand, line_number);
        case 2:
            return check_two_operands(line, i, error_count, command_name, line_number);
        default:
            first_pass_errors(ERR_INVALID_OP, line_number, -1);
            (*error_count)++;
            return false;
    }
}

/**
 * Checks if a command that requires zero operands has an extra argument.
 * If any argument is found after the command, it reports an error and add 1 to the error count.
 *
 * @param line        The line to check.
 * @param i           The starting index of operands in the line.
 * @param error_count The number of errors found so far.
 * @param line_number The number of the line being checked
 *
 * @return true (1) if the command line is valid, false (0) otherwise.
 */
bool check_zero_operands(char *line, int i, int *error_count, int line_number) 
{    
    if (line[i] != '\0' && line[i] != '\n') 
    {
        first_pass_errors(ERR_SHOULD_NOT_HAVE_OP, line_number, -1);
        (*error_count)++;
        return false;
    }

    return true;
}

/**
 * Checks if the line contains only one valid operand.
 * It verifies that the operand type matches the rules for the specific command.
 *
 * @param line        The full line of the assembly code.
 * @param i           The starting index of the operands in the line.
 * @param error_count The number of errors found so far.
 * @param name        The name of the command.
 * @param operand     The operand to validate.
 * @param line_number The number of the line being checked
 *
 * @return true (1) if the operand is valid for the given command, false (0) otherwise.
 */

bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand, int line_number) 
{
    int comma_count = 0;
    bool ok = false;

    /* Checks if there are enough operands */
    if (line[i] == '\0' || line[i] == '\n') 
    {
        first_pass_errors(ERR_MISSING_OPERAND, line_number, -1);
        (*error_count)++;
        return ok;
    }

    for (; line[i] != '\0' && line[i] != '\n'; i++) 
    {
        if (line[i] == ',') 
        {
            comma_count++;
        }
    }

    /* Checks if there are too many operands */
    if (comma_count != 0) 
    {
        first_pass_errors(ERR_TOO_MANY_OPERANDS, line_number, -1);
        (*error_count)++;
        return ok;
    }

    if (strcmp(name, "red") == 0 || strcmp(name, "not") == 0 || strcmp(name, "clr") == 0 || 
        strcmp(name, "inc") == 0 || strcmp(name, "dec") == 0) 
    {
        if (is_register(operand) || is_matrix(operand) || is_label(operand)) 
        {
            ok = true;
        }
    }
    if (strcmp(name, "prn") == 0)
    {
        if (is_register(operand) || is_immediate(operand) || is_label(operand) || is_matrix(operand)) 
        {
            ok = true;
        }
    }
    if (strcmp(name, "jsr") == 0 || strcmp(name, "jmp") == 0 || strcmp(name, "bne") == 0)
    {
        if (is_label(operand) || is_matrix(operand) || is_register(operand)) 
        {
            ok = true;
        }
    }

    if (!ok)
    {
        first_pass_errors(ERR_INVALID_OP, line_number, -1);
        (*error_count)++;
    }
    

    return ok;
}

/**
 * Checks if the line contains only two operands, separated by a comma,
 * and validates them according to the command.
 *
 * @param line        The line to check.
 * @param i           The starting index of the operands in the line.
 * @param error_count The number of errors found so far.
 * @param name        The name of the command.
 * @param line_number The number of the line being checked
 *
 * @return true (1) if both operands are valid, false (0) otherwise.
 */

bool check_two_operands(char *line, int i, int *error_count, char *name, int line_number) 
{
    char temp_line[MAX_LINE_LENGTH];
    char *src = NULL, *dest = NULL;
    int count = 0; /* Counts the number of operands */
    bool src_ok = false, dest_ok = false;

    strcpy(temp_line, &line[i]);

    /* Gets first operand */
    src = strtok(temp_line, ", \t\n");
    if (src) 
    {
        count++;
        
        /* Gets second operand */
        dest = strtok(NULL, ", \t\n");
        if (dest) 
        {
            count++;
        }
    }

    /* Checks if there is a missing bracket */
    if (strchr(src, '[') && strchr(src, ']') == NULL) 
    {
        first_pass_errors(ERR_MISSING_BRACKET, line_number, -1);
        (*error_count)++;
        return false;
    }

    /* Checks if there were more than 2 operands */
    if (strtok(NULL, ", \t\n") != NULL) 
    {
        count++; 
    }

    /* Sends an error if yes */
    if (count != 2) 
    {
        first_pass_errors(ERR_SHOULD_HAVE_TWO_OP, line_number, -1);
        (*error_count)++;
        return false;
    }


    if (strcmp(name, "cmp") == 0)
    {
        if (is_register(src) || is_immediate(src) || is_label(src) || is_matrix(src)) 
        {
            src_ok = true;
        }
        if (is_register(dest) || is_immediate(dest) || is_label(dest) || is_matrix(dest)) 
        {
            dest_ok = true;
        }
    }
    if (strcmp(name, "sub") == 0 || strcmp(name, "mov") == 0 || strcmp(name, "add") == 0)
    {
        if (is_register(src) || is_immediate(src) || is_label(src) || is_matrix(src)) 
        {
            src_ok = true;
        }
        
        if (is_register(dest) || is_label(dest) || is_matrix(dest)) 
        {
           dest_ok = true;
        }
    }
    if (strcmp(name, "lea") == 0)
    {
        if (is_label(src))
        {
            src_ok = true;
        }
        
        if (is_register(dest) || is_label(dest) || is_matrix(dest)) 
        {
            dest_ok = true;
        }
    }

    if (!src_ok) {
        first_pass_errors(ERR_INVALID_SRC_OP, line_number, -1);
        (*error_count)++;
    }

    if (!dest_ok) {
        first_pass_errors(ERR_INVALID_DEST_OP, line_number, -1);
        (*error_count)++;
    }

    return src_ok && dest_ok;
}

/**
 * Checks if the given operand is a valid register. (r1 -r7)
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a register, false (0) otherwise.
 */
bool is_register(char *operand) 
{
    return (strlen(operand) == 2 && operand[0] == 'r' &&
            operand[1] >= '0' && operand[1] <= '7');
}

/**
 * Checks if the given operand is a valid immediate.
 *
 * The immediate must begin with '#' followed by a number. (Could be with a '-')
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a valid immediate, false (0) otherwise.
 */
bool is_immediate(char *operand) 
{
    return (operand[0] == '#' && isdigit(operand[1])) || 
        (operand[0] == '#' && operand[1] == '-' && isdigit(operand[2]));
}

/**
 * Checks if the given operand is a valid matrix.
 *
 * A valid matrix operand has the form: LABEL[register1][register2]
 *
 * @param operand The operand to check.
 *
 * @return true (1) if the operand is a valid matrix, false (0) otherwise.
 */
bool is_matrix(char *operand)
{
    char *first_open, *first_close, *second_open, *second_close;
    char index1[MAX_LINE_LENGTH], index2[MAX_LINE_LENGTH];

    first_open = strchr(operand, '[');
    if (!first_open) return false;

    first_close = strchr(first_open, ']');
    if (!first_close) return false;

    second_open = strchr(first_close + 1, '[');
    if (!second_open) return false;

    second_close = strchr(second_open, ']');
    if (!second_close) return false;

    /* Copy the content inside the first brackets */
    strncpy(index1, first_open + 1, first_close - first_open - 1);
    index1[first_close - first_open - 1] = '\0';

    /* Copy the content inside the second brackets */
    strncpy(index2, second_open + 1, second_close - second_open - 1);
    index2[second_close - second_open - 1] = '\0';

    /* Check if both are valid registers, or have no value */
    if ((is_register(index1) && is_register(index2)) ||
        (is_register(index1) && index2[0] == '\0') ||
        (index1[0] == '\0' && is_register(index2)) ||
        (index1[0] == '\0' && index2 [0]== '\0')) 
    {
        return true;
    }    

    return false;
}

/**
 * Checks if the given operand is a valid label.
 *
 * A valid label must start with a letter, followed by letters or digits ONLY.
 *
 * @param operand The operand string to check.
 *
 * @return true (1) if the operand is a valid label, false (0) otherwise.
 */
bool is_label(char *operand)
{
    int i;
    if (!isalpha(operand[0])) 
    {
        return false;
    }

    for (i = 1; i < strlen(operand) - 1; i++) 
    {
        if (!isalnum(operand[i])) return false;
    }
    return true;
}



/**
 * Validates the name of a label.
 *
 * @param label  The label check.
 * @param line_number  The number of the line in the source file.
 *
 * @return true (1) if the label is legal, false (0) otherwise.
 */
int is_label_ok(char *label, int line_number)
{
    char label_name[MAX_LABEL_LENGTH];
    int i = 0;

    /* Checks if the label starts with a letter. Prints an error if no. */
    if (isalpha(label[i]))
    {
        /* Checks if the label ends with ':'. Prints an error if no. */
        if (strchr(label, ':') != NULL)
        {
            while (label[i] != ':' && i < MAX_LABEL_LENGTH - 1 && label[i] != '\0')
            {
                /* Checks if all the characters in the label are letters/digits. 
                Prints an error if no, and returns false (0) */
                if (isalpha(label[i]) || isdigit(label[i]))
                {
                    label_name[i] = label[i];
                    i++;
                }
                else
                {
                    first_pass_errors(ERR_LABEL_IS_NOT_ALPHANUMERIC, line_number, -1);
                    return false;
                }
            }

            /* Checks if the label ends with a ':' */
            if (label[i] != ':')
            {
                first_pass_errors(ERR_LABEL_ENDING, line_number, -1);
                return false;
            }

            /* End the label name */
            label_name[i] = '\0';
        }

        /* Checks if the label is a reserved word. Prints an error if yes. */
        if (is_reserved_word(label_name))
        {
            first_pass_errors(ERR_LABEL_RESERVED, line_number, -1);
            return false;
        }
    }

    else
    {
        first_pass_errors(ERR_LABEL_START, line_number, -1);
        return false;
    }

    /* Returns true (1) if the label is valid */
    return true;
}

/**
 * Checks if the given string is a valid number.
 *
 * @param input  The input to validate.
 *
 * @return true (1) if the input is a valid number, false (0) otherwise.
 */
int is_number_ok(char *input)
{
    int i = 0;

    /* Skips the number sign */
    if (input[i] == '-' || input[i] == '+')
    {
        i++;
    }

    /* Checks if the first digit is a digit */
    if (!isdigit(input[i]))
    {
        return false;
    }

    /* Checks the rest of the input */
    while (input[i] != '\0' && input[i] != '\n')
    {
        if (!isdigit(input[i]))
        {
            return false;
        }
        i++;
    }

    return true;
}

/**
 * Checks if the given word is a valid command.
 *
 * @param word  The word to check.
 *
 * @return true (1) if it's a known command, false (0) otherwise.
 */
int is_command_ok(char *word)
{
    char *commands[] =
        {
            "mov", "cmp", "add", "sub", 
            "not", "clr", "lea", "inc", 
            "dec", "jmp", "bne", "red",
            "prn", "jsr", "rts","stop"
        };

    int amount_of_commands = 16;

    int i;

    /* Checks if the given word is a command */
    for (i = 0; i < amount_of_commands; i++)
    {
        if (strcmp(word, commands[i]) == 0)
            return true;
    }
    return false;
}

/**
 * Checks if a label is a reserved word in the language (command, directive, etc.).
 *
 * @param label  The label name to check.
 *
 * @return true (1) if it's reserved, false (0) otherwise.
 */
int is_reserved_word(const char *label)
{
    const char *reserved[] = 
    {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        ".data", ".string", ".mat", ".entry", ".extern",
        "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
        "mcro", "mcroend"
    };

    int reserved_words = sizeof(reserved) / sizeof(reserved[0]);
    int i;
    for (i = 0; i < reserved_words; i++)
    {
        if (strcmp(label, reserved[i]) == 0)
        {
            return true;
        }
    }
    return false;
}


