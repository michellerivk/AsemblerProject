#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "help_functions.h"
#include "data_structs.h"

/**
 * The function removes all the white spaces at the start of the string.
 *
 * @param input The given string.
 * @param i The start of the string.
 */
int remove_white_spaces(char input[], int i)
{
    while (input[i] == ' ' || input[i] == '\t')
    {
        i++;
    }

    return i;
}

/**
 * The function adds an ending to the given file name.
 *
 * @param filename The given name of the file.
 * @param ending The file ending.
 * @return A pointer to the new name.
 *         If memory allocation fails, the function returns an errors.
 */
char *add_ending(const char *file_name, const char *ending) 
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
    const char *reserved[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        ".data", ".string", ".mat", ".entry", ".extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"
    };

    int reserved_words = 29;
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

    i = remove_white_spaces(label, i);

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
                    printf("ERROR: The label includes a charcter other than a digit or a letter");
                    return false;
                }
            }

            /* Checks if the label ends with a ':' */
            if (label[i] != ':') 
            {
                printf("ERROR: Label cannot end with a charcter other than a ':'\n");
                return false;
            }

            /* End the label name */
            label_name[i] = '\0';
        }
        

        /* Checks if the label is a reserved word. Returns an error if yes. */
        if (is_reserved_word(label_name))
        {
            printf("ERROR: The label cannot be a reserved word of the essembler.");
            return false;
        }
    }

    else
    {
        printf("ERROR: The label has to start with a letter");
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
char* get_label(char *line)
{
    char label_name[MAX_LABEL_LENGTH];
    int i = 0;

    i = remove_white_spaces(line, i);

    if (label_name )
    {
        /* code */
    }
    
}