#include "first_pass_functions.h"
#include <string.h>



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
            printf("ERROR: Invalid Operator in line:%s\n", line);
            (*error_count)++;
            return false;
    }
}

bool check_zero_operands(char *line, int i, int *error_count) 
{    
    if (line[i] != '\0' && line[i] != '\n') 
    {
        printf("ERROR: Command should not have operands.\n");
        (*error_count)++;
        return false;
    }

    return true;
}

bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand) 
{
    int comma_count = 0;
    bool ok = false;

    /* Checks if there are enough operands */
    if (line[i] == '\0' || line[i] == '\n') 
    {
        printf("ERROR: Command is missing its operand in line:%s\n", line);
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
        printf("ERROR: Command has too many operands in line:%s\n", line);
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
        printf("ERROR: Invalid operand in line:%s\n", line);
        (*error_count)++;
    }
    

    return ok;
}

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
        printf("ERROR: Command should have two operands in line:%s\n", line);
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
        if (is_register(src) || is_immediate(src) || is_label(src)) 
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
        printf("ERROR: Invalid source operand in line:%s\n", line);
        (*error_count)++;
    }

    if (!dest_ok) {
        printf("ERROR: Invalid destination operand in line:%s\n", line);
        (*error_count)++;
    }

    return src_ok && dest_ok;
}

/* Checks if the register is ok */
bool is_register(char *operand) 
{
    return (strlen(operand) == 2 && operand[0] == 'r' &&
            operand[1] >= '0' && operand[1] <= '7');
}

/* Checks if the immediate is ok */
bool is_immediate(char *operand) 
{
    return (operand[0] == '#' && isdigit(operand[1])) || 
        (operand[0] == '#' && operand[1] == '-' && isdigit(operand[2]));
}

/* Checks if the matrix is ok */
bool is_matrix(char *operand)
{
    char *bracket1, *bracket2, *bracket3, *bracket4;
    char reg1[5], reg2[5];
    int len1, len2;


    bracket1 = strchr(operand, '[');
    if (!bracket1) return false;

    bracket2 = strchr(bracket1 + 1, ']');
    if (!bracket2 || bracket2 <= bracket1 + 1) return false;

    bracket3 = strchr(bracket2 + 1, '[');
    if (!bracket3) return false;

    bracket4 = strchr(bracket3 + 1, ']');
    if (!bracket4 || bracket4 <= bracket3 + 1) return false;

    if (*(bracket4 + 1) != '\0') return false;

    len1 = bracket2 - bracket1 - 1;
    len2 = bracket4 - bracket3 - 1;

    if (len1 >= sizeof(reg1) || len2 >= sizeof(reg2)) return false;

    strncpy(reg1, bracket1 + 1, len1);
    reg1[len1] = '\0';

    strncpy(reg2, bracket3 + 1, len2);
    reg2[len2] = '\0';

    return is_register(reg1) && is_register(reg2);
}

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