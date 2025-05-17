#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "first_pass_functions.h"

/**
 * Checks if the command or directive exists in the given list of words.
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
int check_word(char *line, int start, const char *words[], int amount, int *error_count, bool label_flag) 
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
            
            /* Checks if the operands in the command are ok */
            if (check_command(line, len, error_count, get_instruction(command), command, label_flag))
            {
                return len;
            }            
        }
    }
    return -1;
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
 * Parses a directive line, and updates the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param line         The line being parsed.
 * @param error_count  The number of errors found so far.
 * @param directive    The directive word being processed.
 */
void add_directive(assembler_table *table, char *line, int *error_count, char *directive)
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
                data *new_directive = (data *)malloc(sizeof(data)); 

                /* Returns error if memory allocation failed */
                if (!new_directive) {
                    printf("ERROR: Memory allocation failed.\n");
                    exit(1);
                }
                
                /* Checks if the number is ok */
                if (!is_number_ok(arg)) 
                {
                    printf("ERROR: The argument %s is not a number!\n", arg);
                    (*error_count)++;
                    arg = strtok(NULL, ",");
                    continue;
                }   
                
                value = atoi(arg); 

                new_directive->word.value = value;
                new_directive->address = table->data_counter;
                new_directive->next = NULL;

                /* Add the node to the list */
                add_data_node(table, new_directive);

                table->data_counter++;

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
                printf("ERROR: There are no quotation marks straightly after .string\n");
                (*error_count)++;
                return;
            }

            i++; /* Skips the first " */

            while (line[i] != '\0' && line[i] != '\"') 
            {
                data *new_directive = (data *)malloc(sizeof(data));

                /* Checks if memory allocation failed */
                if (!new_directive) 
                {
                    printf("ERROR: Memory allocation failed.\n");
                    exit(1);
                }
    
                new_directive->word.value = (int)line[i];
                new_directive->address = table->data_counter;
                new_directive->next = NULL;
    
                add_data_node(table, new_directive);
    
                table->data_counter++;
                i++;
            }

            /* Checks if the string ends with " */
            if (line[i] != '\"') 
            {
                printf("ERROR: Missing closing quotation mark in .string directive\n");
                (*error_count)++;
                return;
            }

            /* Adds 0 at the end of the string list */
            null_node = (data *)malloc(sizeof(data));

            /* Checks if memory allocation failed */
            if (!null_node) {
                printf("ERROR: Memory allocation failed.\n");
                exit(1);
            }

            null_node->word.value = 0;
            null_node->address = table->data_counter;
            null_node->next = NULL;

            /* Adds the node to the list */
            add_data_node(table, null_node);

            table->data_counter++;

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

        /* Checks if the input of the matrix is ok */
        if (sscanf(input, "[%d][%d]", &rows, &cols) != 2) 
        {
            printf("ERROR: The matrix was entered wrongly on line: %s\n", line);
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
            data *mat_node = (data *)malloc(sizeof(data)); /* The node we're adding to the table */

            /* Checks if memory allocation failed */
            if (!mat_node) 
            {
                printf("ERROR: Memory allocation failed.\n");
                exit(1);
            }
            
            /* Skips white spaces */
            while (*arg == ' ' || *arg == '\t')
            {
                arg++;
            }
        
            /* Checks if valid arguments were given */
            if (!is_number_ok(arg)) 
            {
                printf("ERROR: '%s' is not a valid number in .mat\n", arg);
                (*error_count)++;
                arg = strtok(NULL, ",");
                continue;
            }
        
            /* Fills the table */
            mat_node->word.value = atoi(arg);
            mat_node->address = table->data_counter;
            mat_node->next = NULL;

            /* Add the directive to the table */
            add_data_node(table, mat_node);
        
            table->data_counter++;
            count++; /* Adds an argument to the counter */

            arg = strtok(NULL, ",");
        }
        
        /* Checks if the amount of numbers is exactly the amount neede to fill the given matrix */
        if (count != rows * cols) 
        {
            printf("ERROR: Got %d values instead of %d\n", count, rows * cols);
            (*error_count)++;
        }
    }    
    else if (strcmp(directive, ".entry") == 0)
    {
        return;
        /* ###############Second Pass################### */
    }    
    else if (strcmp(directive, ".extern") == 0)
    {
        int type = EXTERNAL;
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

            add_label_to_table(table, lbl, type, error_count);
        }
    }    
}

/**
 * Checks if the given string is a valid number.
 *
 * @param input  The input to validate.
 *
 * @return 1 if the input is a valid number, 0 otherwise.
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
 *
 * @return true (1) if label was added successfully, false (0) otherwise.
 */
int add_label(assembler_table *table, char *line, int i, int *error_count, 
              const char *commands[], const char *directives[], int commands_len, 
              int directives_len, bool label_flag)
{
    int len;
    char *lbl = get_label(line, i); /* Gets the label name out of the line */
    int type = -1;

    while (line[i] && line[i] != ':') 
    {
        (i)++;
    }

    if (line[i] == ':') 
    {
        (i)++;
    }

    /* Checks if the next words matches a directive */
    len = check_word(line, i, directives, directives_len, error_count, label_flag);

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
            printf("ERROR: The directive: %s after label is not known\n", &line[i]);
            (*error_count)++;
            free(lbl);
            return false;
        }
    }
    else
    {
        /* Checks if the next words matches a command */
        len = check_word(line, i, commands, commands_len, error_count, label_flag);

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

/**
 * Adds a new label with address and type to the assembler table.
 *
 * @param table        The assembler's table structure.
 * @param lbl          The label name.
 * @param type         The label type.
 * @param error_count  The number of errors found so far.
 */
void add_label_to_table(assembler_table *table, char *lbl, int type, int *error_count)
{
    label *new_label = (label *)malloc(sizeof(label));

    /* Checks if memory allocation failed */
    if (!new_label) 
    {
        printf("ERROR: Memory allocation failed\n");
        exit(1);
    }

    /* Checks if the label exists already in the table */
    if (check_for_label(table->label_list, lbl)) 
    {
        printf("ERROR: The label: %s exists already in the table.", lbl);
        (*error_count)++;
        return; 
    }

    /* Inserts the name of the label into the table */
    strcpy(new_label->name, lbl);

    /* Adds 100 to the IC on the first incounter */
    if (type == CODE && table->instruction_counter == 0)
    {
        table->instruction_counter += 100;
        new_label->address = table->instruction_counter;
    }
    
    else 
    {
        /* Checks what counter to assign to the address */
        if (type == DATA)
        {
            new_label->address = table->data_counter + table->instruction_counter + 1;
        }
        if (type == CODE)
        {
            new_label->address = table->instruction_counter + 1;
        }
    }

    new_label->type = type;
    new_label->next = NULL;

    /* add the lable to the list */
    add_label_node(table, new_label);
}

/**
 * Checks if the given label already exists in the label list.
 *
 * @param list   The head of the label list.
 * @param label  The label name to check.
 *
 * @return true (1) if the label exists, false (0) otherwise.
 */
int check_for_label(label *list, char *label)
{
    while (list != NULL) 
    {
        if (strcmp(list->name, label) == 0) 
        {

            return true;
        }
        list = list->next;
    }

    return false;
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
        "mov", "cmp", "add", "sub", "not", "clr", "lea", 
        "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    };

    int amount_of_commands = 16;

    int i;

    /* Checks if the given word is a command */
    for (i = 0; i < amount_of_commands; i++) {
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
    char *reserved[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        ".data", ".string", ".mat", ".entry", ".extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };

    int reserved_words = 29;
    int i;
    for (i = 0; i < sizeof(reserved_words)/ sizeof(reserved[0]); i++) 
    {
        if (strcmp(label, reserved[i]) == 0) 
        {
            return true;
        }
    }
    return false;
}

/**
 * Validates the name of a label.
 *
 * @param label  The label check.
 *
 * @return true (1) if the label is legal, false (0) otherwise.
 */
int is_label_ok(char *label)
{
    char label_name[MAX_LABEL_LENGTH];
    int i = 0;

    /* Checks if the label starts with a letter. Returns an error if no using else. */
    if (isalpha(label[i]))
    {
        /* Checks if the label ends with ':'. Returns an error if no. */
        if (strchr(label, ':') != NULL)
        {    
            while (label[i] != ':' && i < MAX_LABEL_LENGTH - 1 && label[i] != '\0')
            {
                /* Checks if all the characters in the label are letters/digits. Returns an error if no. */
                if (isalpha(label[i]) || isdigit(label[i]))
                {              
                    label_name[i] = label[i];
                    i++;
                }
                else
                {
                    printf("ERROR: The label includes a charcter other than a digit or a letter.\n");
                    return false;
                }
            }

            /* Checks if the label ends with a ':' */
            if (label[i] != ':') 
            {
                printf("ERROR: The label cannot end with a character other than a ':'\n");
                return false;
            }

            /* End the label name */
            label_name[i] = '\0';
        }
        

        /* Checks if the label is a reserved word. Returns an error if yes. */
        if (is_reserved_word(label_name))
        {
            printf("ERROR: The label cannot be a reserved word of the essembler.\n");
            return false;
        }
    }

    else
    {
        printf("ERROR: The label has to start with a letter.\n");
        return false;
    }

    /* Returns true if the label is ok */
    return true;
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

    /* Checks if there are ':'. If no there's no label, and returns NULL */
    if (line[i + len] != ':') 
    {
        return NULL;
    }

    label = (char *)malloc(len + 1);

    /* Returns an error if memory allocation fails */
    if (!label) 
    {
        fprintf(stderr, "ERROR: Out of memory in get_label()\n");
        exit(1);
    }

    /* Copies the label name into 'label' */
    memcpy(label, &line[i], len);

    /* Ends the label */
    label[len] = '\0';


    return label;

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
 * Updates the instruction counter based on the number of operands in a command.
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
               assembler_table *table, int *error_count, bool label_flag)
{
    int len = check_word(line, i, commands, com_len, error_count, label_flag);

    if (len != -1)
    {
        char command_name[MAX_LINE_LENGTH];
        int L; /* The number of operends */

        strncpy(command_name, &line[i], len);
        command_name[len] = '\0';

        /* Checks the amount of arguments needed for the command*/
        L = get_instruction(command_name);

        table->instruction_counter += 1 + L; /* Updates the IC to IC + L */
    };

    
}

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

    int i = 0;

    /* The line doesn't have a label */
    if (!strchr(line, ':')) 
    {
        /* Checks if there's a command */
        int len = check_word(line, i, commands, com_len, error_count, label_flag);

        if (len != -1)
        {
            update_ic(line, i, commands, com_len, table, error_count, label_flag);
        }
        else
        {
            /* Checks if there's a directive */
            len = check_word(line, i, directives, dir_len, error_count, label_flag);

            if (len != -1)
            {
                /* Gets the directive */
                char directive[MAX_LINE_LENGTH];
                strncpy(directive, line, len);
                directive[len] = '\0'; 

                /* Handles the directive */
                add_directive(table, line, error_count, directive);
            }

            /* If there is no directive nor command, it will do nothing */
            else
            {
                printf("Nothing to do in line: %s\n", line);
                return;
            }
        }
    }

    /* The line has a label */
    else
    {
        /* Turns on label_flag */
        label_flag = true;

        /* Cheks if label is ok */
        if (is_label_ok(line))
        {
            bool good_label;
            int len;
            good_label = add_label(table, line, i, error_count, commands, directives, com_len, dir_len, label_flag);

            if (good_label == false)
            {
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
            len = check_word(line, i, directives, dir_len, error_count, label_flag);
            if (len != -1)
            {
                /* Gets the name of the directive */
                char directive[MAX_LINE_LENGTH];
                strncpy(directive, &line[i], len);
                directive[len] = '\0';

                /* Adds the directive to the table */
                add_directive(table, line, error_count, directive);
                return;
            }

            /* Checks if there's a command after the label */
            len = check_word(line, i, commands, com_len, error_count, label_flag);

            if (len != -1)
            {
                update_ic(line, i, commands, com_len, table, error_count, label_flag); /* Updates the IC */
                return;
            }

            /* Returns error if no */
            printf("ERROR: Something other than a command or a directive was entered after the label: %s\n", &line[i]);
            (*error_count)++;
            return;
        }

        /* Sends an error if the label is not ok */
        else
        {
            printf("ERROR: No label was found in the line: %s\n", line);
            (*error_count)++;
            return;
        }
    }
}
