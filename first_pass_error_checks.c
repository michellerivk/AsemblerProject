#include "first_pass_functions.h"
#include <string.h>


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
 *
 * @return true if the command is valid, false otherwise.
 */
bool check_command(char *line, int i, int *error_count, int operands_amount, char *command_name, bool label_flag)
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
            return check_zero_operands(line, i, error_count);
        case 1:
            return check_one_operands(line, i, error_count, command_name, operand);
        case 2:
            return check_two_operands(line, i, error_count, command_name);
        default:
            printf("ERROR: Invalid Operator in line: %s\n", line);
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
 *
 * @return true if the command line is valid, false otherwise.
 */
bool check_zero_operands(char *line, int i, int *error_count) 
{    
    if (line[i] != '\0' && line[i] != '\n') 
    {
        printf("ERROR: Command should not have operands in line: %s\n", line);
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
 *
 * @return true if the operand is valid for the given command, false otherwise.
 */

bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand) 
{
    int comma_count = 0;
    bool ok = false;

    /* Checks if there are enough operands */
    if (line[i] == '\0' || line[i] == '\n') 
    {
        printf("ERROR: Command is missing its operand in line: %s\n", line);
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
        printf("ERROR: Command has too many operands in line: %s\n", line);
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
        if (is_register(operand) || is_immediate(operand) || is_label(operand)) 
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
        printf("ERROR: Invalid operand in line: %s\n", line);
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
 *
 * @return true if both operands are valid, false otherwise.
 */

bool check_two_operands(char *line, int i, int *error_count, char *name) 
{
    char temp_line[MAX_LINE_LENGTH];
    char *src = NULL, *dest = NULL;
    int count = 0;
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

    /* Checks if there were more than 2 operands */
    if (strtok(NULL, ", \t\n") != NULL) 
    {
        count++; 
    }

    /* Sends an error if yes */
    if (count != 2) 
    {
        printf("ERROR: Command should have two operands in line: %s\n", line);
        (*error_count)++;
        return false;
    }


    if (strcmp(name, "cmp") == 0)
    {
        if (is_register(src) || is_immediate(src) || is_label(src)) 
        {
            src_ok = true;
        }
        if (is_register(dest) || is_immediate(dest) || is_label(dest)) 
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
        
        if (is_register(dest) || is_label(dest)) 
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
        
        if (is_register(dest) || is_label(dest)) 
        {
            dest_ok = true;
        }
    }

    if (!src_ok) {
        printf("ERROR: Invalid source operand in line: %s\n", line);
        (*error_count)++;
    }

    if (!dest_ok) {
        printf("ERROR: Invalid destination operand in line: %s\n", line);
        (*error_count)++;
    }

    return src_ok && dest_ok;
}

/**
 * Checks if the given operand is a valid register. (r1 -r7)
 *
 * @param operand The operand to check.
 *
 * @return true if the operand is a register, false otherwise.
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
 * @return true if the operand is a valid immediate, false otherwise.
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
 * @return true if the operand is a valid matrix, false otherwise.
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

    /* Check if both are valid registers */
    if (is_register(index1) && is_register(index2)) {
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
 * @return true if the operand is a valid label, false otherwise.
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