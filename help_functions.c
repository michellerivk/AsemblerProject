#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_functions.h"
/* include assembler.h -> help_functions.h */

/* Deletes the following white spaces */
int delete_white_spaces(char input[], int i)
{
    while (input[i] == ' ' || input[i] == '\t')
    {
        i++;
    }

    return i;
}


void add_directive(assembler_table *table)
{

}

void add_label(assembler_table *table, char *line, int i, int *error_count)
{
    char next[MAX_LINE_LENGTH];
    char *lbl = get_label(line, i); /* Gets the label name out of the line */
    int type;
    int j = 0;

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

    /* Get the next word in line */
    while (line[i] && line[i] != ' ' && line[i] != '\t' && line[i] != '\n') 
    {
        next[j++] = line[i];
        i++;
    }

    next[j] = '\0';

    /* Check the type of the label */
    if (next[0] == '.') 
    {
        if (strcmp(next, ".data") == 0 || strcmp(next, ".string") == 0 || strcmp(next, ".mat") == 0)
        {
            type = DATA;
        }
        else if (strcmp(next, ".extern") == 0)
        {
            type = EXTERNAL;
        }
        else if (strcmp(next, ".entry") == 0)
        {
            type = ENTRY;
        }
        else 
        {
            printf("ERROR: The directive: %s after label is not known\n", next);
            (*error_count)++;
            free(lbl);
            return;
        }
    }

    else if (is_command_ok(next)) 
    {
        type = CODE;
    } 
    else 
    {
        printf("ERROR: The command or directive: %s after label is not known.\n", next);
        (*error_count)++;

        free(lbl);
        return;
    }

    add_label_to_table(table, lbl, type, error_count);
    free(lbl);

}

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
    new_label->address = (type == DATA) ? table->data_counter : table->instruction_counter;
    new_label->type = type;
    new_label->next = NULL;

    /* Adds the first label if there were none before */
    if (table->label_list == NULL) 
    {
        table->label_list = new_label;
    } 

    /* Adds the label to the end of the table */ 
    else 
    {
        label *temp = table->label_list;
        while (temp->next != NULL) 
        {
            temp = temp->next;
        }
        temp->next = new_label;
    }
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

/* Checks if the command name is */
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


/**
 * The function checks if the name of the label is good.
 *
 * @param label The given label.
 * @return true if the label is good, else otherwise.
 *         The true and false are part of an enum that exists in the help_functions.h file.
 */
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


/**
 * The function checks if the name of the label is good.
 *
 * @param label The given label.
 * @return true if the label is good, else otherwise.
 *         The true and false are part of an enum that exists in the help_functions.h file.
 */

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

/**
 * The function checks if line has a label, data section or a command.
 *
 * @param line The given line.
 * @param line_number The line number.
 * @param table The struct where we will save our conclusion.
 */
void check_line(char *line, int line_number, assembler_table *table, int *error_count)
{
    char *commands[] = 
    {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
    };

    char *directives[] = 
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
                printf("FOR DEBUGGING: The name of the dericetive is: %s\n", str);

                add_directive(table);
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
        /* Checks if there's a label in the line */
        if (strchr(line, ':') != NULL)
        {
            if (is_label_ok(line) == 1)
            {
                /* Adds the label to the table */
                add_label(table, line, i, error_count);

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
        }
    }    
}
