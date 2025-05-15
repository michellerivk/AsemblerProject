#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_functions.h"

/* Checks if the given word exists in the language */
int check_word(char *line, int start, const char *words[], int amount) {
    int i;
    for (i = 0; i < amount; i++) 
    {
        int len = strlen(words[i]);
        if (strncmp(&line[start], words[i], len) == 0) 
        {
            char next_char = line[start + len];

            /* Checks if the next character is good */
            if (next_char == '\0' || next_char == ',' || next_char == '\n' ||
                next_char == '@' || next_char == '"' || next_char == '[' || next_char == '#' ||
                !isalpha(next_char) || !isdigit(next_char)) 
            {
                return len;
            }
        }
    }
    return -1;
}

/* Adds a data node to a list */
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

/* Adds a label node to a list */
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

/* Deletes the following white spaces */
int delete_white_spaces(char input[], int i)
{
    while (input[i] == ' ' || input[i] == '\t')
    {
        i++;
    }

    return i;
}

/* Imports the necessary data for the table, and adds it to the table */
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
                i = delete_white_spaces(line, i);
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

                /* Removes white spaces */
                while (*arg == ' ' || *arg == '\t')
                {
                    arg++;
                }
                
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
            i = delete_white_spaces(line, i);

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
        i = delete_white_spaces(line, i);

        input = &line[i];

        /* Checks if the input of the matrix is ok */
        if (sscanf(input, "[%d][%d]", &rows, &cols) != 2) 
        {
            printf("ERROR: The matrix was entered wrongly\n");
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

            i = delete_white_spaces(line,i);
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
            mat_node->address = table->data_counter++;
            mat_node->next = NULL;

            /* Add the directive to the table */
            add_data_node(table, mat_node);
        
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
            i = delete_white_spaces(line, i);

            while (line[i] != '\0' && line[i] != ' ' && line[i] != '\n') 
            {
                lbl[j++] = line[i++];
            }

            lbl[j] = '\0';

            add_label_to_table(table, lbl, type, error_count);
        }
    }    
}

/* Checks if the given input is a number */
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

/* Imports the necessary data for the table, and calls add_label_to_table function */
void add_label(assembler_table *table, char *line, int i, int *error_count, const char *commands[], const char *directives[],
               int commands_len, int directives_len)
{
    int len;
    char *lbl = get_label(line, i); /* Gets the label name out of the line */
    int type;

    while (line[i] && line[i] != ':') 
    {
        (i)++;
    }

    if (line[i] == ':') 
    {
        (i)++;
    }

    /* Deletes whitespaces after the label */
    i = delete_white_spaces(line, i);

    /* Checks if the next words matches a directive */
    len = check_word(line, i, directives, directives_len);

    if (len != -1)
    {
        if (strncmp(&line[i], ".data", len) == 0 || strncmp(&line[i], ".string", len) == 0 ||
            strncmp(&line[i], ".mat", len) == 0) 
        {
            type = DATA;

            /* ################FOR DEBUGGiNG################ */
            printf("type = DATA\n");
        } 
        else if (strncmp(&line[i], ".extern", len) == 0) 
        {
            type = EXTERNAL;

            /* ################FOR DEBUGGiNG################ */
            printf("type = EXTERNAL\n");
        } 
        else if (strncmp(&line[i], ".entry", len) == 0) 
        {
            type = ENTRY;

            /* ################FOR DEBUGGiNG################ */
            printf("type = ENTRY\n");
        } 
        else 
        {
            printf("ERROR: The directive: %s after label is not known\n", &line[i]);
            (*error_count)++;
            free(lbl);
            return;
        }
    }

    /* Checks if the next words matches a command */
    len = check_word(line, i, commands, commands_len);

    if (len != -1)
    {
        type = CODE;
        add_label_to_table(table, lbl, type, error_count);
        free(lbl);

        /* ################FOR DEBUGGiNG################ */
            printf("type = CODE");

        return;
    }

    /* If it was none of them */
    printf("ERROR: The command or directive after label is not known: %s\n", &line[i]);
    (*error_count)++;
    free(lbl);

}

/* Adds label to the table */
void add_label_to_table(assembler_table *table, char *lbl, int type, int *error_count)
{
    label *new_label = (label *)malloc(sizeof(label));
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

    strcpy(new_label->name, lbl);

    /* Checks what counter to assign to the address */
    if (type == DATA)
    {
        new_label->address = table->data_counter;
    }
    else
    {
        new_label->address = table->instruction_counter;
    }

    new_label->type = type;
    new_label->next = NULL;

    /* add the lable to the list */
    add_label_node(table, new_label);
}

/* Checks if label exists already. Returns an true if yes, false otherwise */
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

/* Checks if the command name is ok */
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

/* Checks if the label is a reserved word. Returns true if yes. */
int is_reserved_word(const char *label) {
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


/* Checks if the name of the label is good. */
int is_label_ok(char *label)
{
    char label_name[MAX_LABEL_LENGTH];
    int i = 0;

    i = delete_white_spaces(label, i);

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


/* Checks if the name of the label is good. */

 /*************************CHECK IT************************************* */
char *get_label(char *line, int i)
{
    int start = delete_white_spaces(line, i);
    int len = 0;
    char *label;

    /* measure up to the colon or end of buffer */
    while (line[start + len] != ':' &&
           line[start + len] != '\0' &&
           len < MAX_LABEL_LENGTH - 1) {
        len++;
    }

    if (line[start + len] != ':') {
        /* no colon found */
        return NULL;
    }

    /* allocate and copy */
    label = (char *)malloc(len + 1);
    if (!label) {
        fprintf(stderr, "ERROR: Out of memory in get_label()\n");
        exit(1);
    }
    memcpy(label, &line[start], len);
    label[len] = '\0';
    return label;

}


/* a function to check how many lines there are in the file */
int count_lines_in_file(const char *filename) 
{
    FILE *fp = fopen(filename, "r");
    int count = 0;
    char c;

    if (!fp) 
    {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    while ((c = fgetc(fp)) != EOF) 
    {
        if (c == '\n') 
        {
            count++;
        }
    }

    fclose(fp);
    return count;
}

int get_instruction(char *com) 
{
    if (strcmp(com, "stop") == 0 || strcmp(com, "rts") == 0)
    {
        return 1;
    }
    else if (strcmp(com, "jmp") == 0 || strcmp(com, "bne") == 0 || strcmp(com, "jsr") == 0 || strcmp(com, "prn") == 0 ||
             strcmp(com, "not") == 0 || strcmp(com, "clr") == 0 || strcmp(com, "inc") == 0 || strcmp(com, "dec") == 0 ||
             strcmp(com, "red") == 0)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

/* Checks if line has a label, data section or a command.*/
void check_line(char *line, int line_number, assembler_table *table, int *error_count)
{
    const char *commands[] = 
    {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    };

    const char *directives[] = 
    {
        ".data", ".string", ".mat", ".entry", ".extern"
    };

    char str[MAX_LINE_LENGTH];
    int i = 0;
    int j = 0;
    int count = 0;

    i = delete_white_spaces(line, i);

    /* Checks if there's a directive */
    if (strchr(line, ':') == NULL && strchr(line, '.') != NULL)
    {
        if (line[i] == '.')
        {
            while (line[i] != ' ' && line[i] != '\t')
            {
                str[j++] = line[i++];
            }
            
            /* Checks if the directive exists, Returns error if no. */
            for (j = 0; j < sizeof(directives) / sizeof(directives[0]); j++) 
            {
                if (strcmp(str, directives[j]) == 0) 
                {
                    count++;
                    break;
                }
            }
            if(count == 0)
            {
                printf("ERROR: The directive doesn't exist.\n");
                (*error_count)++;
            }
            else
            {
                /*###############3 JUST FOR ME ############### */
                printf("FOR DEBUGGING: The name of the dericetive is: %s\n", str);

                add_directive(table, line, error_count, str);
            }
        }

        else
        {
            printf("ERROR: There's a directive in the wrong place.\n ");
            (*error_count)++;
        }
    }
    else
    {
        int instruction; 

        /* Checks if there's a label in the line */
        if (strchr(line, ':') != NULL)
        {
            if (is_label_ok(line) == 1)
            {
                /* Adds the label to the table */
                add_label(table, line, i, error_count, commands, directives, sizeof(commands) / sizeof(commands[0]) ,
                          sizeof(directives) / sizeof(directives[0]));

                /* Checks if there's a directive after the label */
                if (strchr(line, '.') != NULL)
                {
                    /* Skips the label */
                    while (line[i] != ':')
                    {
                        i++;
                    }
                    
                    if (line[i] == ':') 
                    {
                        i++;
                    }

                    i = delete_white_spaces(line, i);

                    if (line[i] != '.')
                    {
                        printf("ERROR: There's a directive in the wrong place.");
                        (*error_count)++;
                    }

                    else
                    {
                        printf("FOR DEBUGGING: There is a directive after the label: %s\n", str);
                    }
                }
                
            }
            
        }

        /* Checks if there's a command */
        else
        {
            while (line[i] != ' ' && line[i] != '\n' )
            {
                str[j++] = line[i++];
            }
            
            /* Checks if the command exists, Returns error if no. */
            for (j = 0; j < sizeof(commands) / sizeof(commands[0]); j++) 
            {
                if (strcmp(str, commands[j]) == 0) 
                {
                    count++;
                    break;
                }
            }

            if(count == 0)
            {
                printf("ERROR: The directive doesn't exist.\n");
                (*error_count)++;
            }

            else
            {
                printf("FOR DEBUGGING: The line starts with the command: %s", str);
            }

            instruction = get_instruction(str);
            table->instruction_counter += instruction;
        }
    }    
}
