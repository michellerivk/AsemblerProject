#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_functions.h"
#include "assembler.h"

add_directive(assembler_table *table)
{

}

add_label(assembler_table *table, char *line, int index)
{

}

add_command(char *instr_str, assembler_table *table)
{
    
}

/**
 * The function adds an ending to the given file name.
 *
 * @param filename The given name of the file.
 * @param ending The file ending.
 * @return A pointer to the new name.
 *         If memory allocation fails, the function returns an errors.
 */
char *add_ending(char *file_name, const char *ending) 
{
    char *new_file = malloc(strlen(file_name) + strlen(ending) + 1);
    
    /* checks if there is enough memory to allocate */
    if (!new_file) 
    {
        /* prints error if no */
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strcpy(new_file, file_name);
    strcat(new_file, ending);
    return new_file;
}

/**
 * The function makes all string letters uppercase.
 *
 * @param label The given label.
 * @return true if the label is a reserved word of the essembler, false otherwise.
 */
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

char *get_label(char *line, int i)
{
    int start = delete_white_spaces(line, i);
    int len = 0;

    /* measure up to the colon or end of buffer */
    while (line[start + len] != ':' &&
           line[start + len] != '\0' &&
           len < MAX_LABEL_LENGTH - 1) {
        len++;
    }

    if (line[start + len] != ':') {
        /* no colon found → bad label */
        return NULL;
    }

    /* allocate and copy */
    char *label = malloc(len + 1);
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
void check_line(char *line, int line_number, assembler_table *table)
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
        }
    }
    else
    {
        /* Checks if there's a label in the line */
        if (strchr(line, ':') != NULL)
        {
            if (is_label_ok(line) == 1)
            {
                char *name = get_label(line, i);

                label *lbl = malloc(sizeof *lbl);
                strcpy(lbl->name, name);  
                lbl->address = table->instruction_counter;
                lbl->type    = R;
                lbl->next    = table->label_list;
                table->label_list = lbl;

                free(name);

               /*  table->label_list->name = get_label(line, i); */

                if (strchr(line, '.') != NULL)
                {
                    /* Skips the label */
                    while (line[i] != ':')
                    {
                        i++;
                    }
                    
                    i++;

                    i = delete_white_spaces;

                    if (line[i] != '.')
                    {
                        printf("ERROR: There's a directive in the wrong place.");
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
            while (line[i] != ' ')
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
            }

            else
            {
                printf("FOR DEBUGGING: The line starts with the command: %s", str);
            }
        }
    }    
}
