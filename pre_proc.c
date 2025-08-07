#include "pre_proc.h"

/*
  This function removes all spaces and tabs from the given line,
  and also handles comment lines starting with ';' by truncating them.
  The cleaned result is stored in 'dest'.
*/
void remove_white_spaces(char dest[MAX_LINE_LENGTH], char *line)
{
    int i, j = 0;
    char temp[MAX_LINE_LENGTH];

    /* Initialize the temp buffer with null characters */
    memset(temp, '\0', MAX_LINE_LENGTH);

    /* Go over the input line character by character */
    for (i = 0; i < strlen(line); i++)
    {
        /* Copy only non-whitespace characters */
        if (line[i] != ' ' && line[i] != '\t')
        {
            temp[j] = line[i];
            j++;
        }

        /* If a comment starts, terminate the line with newline and break */
        if (line[i] == ';')
        {
            temp[j] = '\n';
            j++;
            i = strlen(line); /* exit loop */
        }
    }

    temp[j] = '\0';
    /* Copy cleaned result to destination */
    strcpy(dest, temp);
}

/*
  This function extracts a token from the input 'line' until the given delimiter.
  The extracted token is copied into 'dest'. Returns the index after the delimiter.
  If the delimiter is not found, returns 0.
*/
int extract_token(char dest[MAX_LINE_LENGTH], char *line, char delimiter)
{
    int i, found = 0;

    /* Copy characters to dest until delimiter is found */
    for (i = 0; i < strlen(line) && found == 0; i++)
    {
        if (line[i] != delimiter)
        {
            dest[i] = line[i];
        }
        else
        {
            found = 1;
        }
    }

    dest[i] = '\0';

    /* If delimiter was found, return its position + 1 */
    return found ? i : 0;
}

/*
  Initializes file paths and file pointers used for pre-processing.
  Adds ".as" and ".am" suffixes to filenames and opens the files accordingly.
  Also clears the buffers for line and macro name.
*/
void files_initialize(assembler_table **assembler, FILE **fp_as, FILE **fp_am,
                      char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH])
{

    /* Generate full path for ".as" and ".am" files */
    add_suffix((*assembler)->assembly_file, (*assembler)->source_file, ".as");
    add_suffix((*assembler)->macro_expanded_file, (*assembler)->source_file, ".am");

    /* Open the input (.as) and output (.am) files */
    *fp_as = safe_fopen((*assembler)->assembly_file, "r");
    *fp_am = safe_fopen((*assembler)->macro_expanded_file, "w");

    /* Clear line and macro_name buffers */
    memset(line, '\0', MAX_LINE_LENGTH);
    memset(macro_name, '\0', MAX_LINE_LENGTH);
}

/*
  Extracts and validates a macro name from a given line.
  Returns false if invalid.
*/
bool extract_and_validate_macro_name(char *macro_name, char *line, int line_number, macro *list)
{
    /* clear the macro_name buffer */
    memset(macro_name, '\0', MAX_LINE_LENGTH);

    /* extract macro name after 'mcro' keyword */
    extract_token(macro_name, line + strlen("mcro"), '\n');

    /* validate the extracted name */
    return macro_name_examine(macro_name, line_number, list);
}

/**
 * Reads the body of a macro from the source file until 'endmcro' is found.
 * Stores each valid line into the macro content list.
 *
 */
bool read_macro_body(FILE *fp_as, macro_content **content, int *line_counter, char *line)
{
    memset(line, '\0', MAX_LINE_LENGTH); /* clear line buffer */

    while (fgets(line, MAX_LINE_LENGTH, fp_as) != NULL)
    {
        (*line_counter)++;
        remove_white_spaces(line, line); /* clean whitespace */
        printf("line %d  %s", (*line_counter), line);

        if (strncmp(line, "mcroend", strlen("mcroend")) != 0)
        {
            /* add line to macro content list */
            add_to_content_list(content, line);
        }
        else
        {
            break; /* reached 'mcroend' */
        }

        memset(line, '\0', MAX_LINE_LENGTH); /* clear for next read */
    }

    remove_white_spaces(line, line); /* final check */

    if (strncmp(line, "mcroend", strlen("mcroend")) == 0)
    {
        /* validate that there's no extra text after 'mcroend' */
        if (!examine_macroend(line + strlen("mcroend"), *line_counter))
            return false;
        return true;
    }

    return false; /* missing 'mcroend' */
}


/**
 * Handles macro declaration: extracts its name, reads its body, and stores it.
*/
bool macro_trearment(assembler_table **assembler, char line[MAX_LINE_LENGTH],
                     char macro_name[MAX_LINE_LENGTH], FILE *fp_as,
                     macro_content **content, int *line_counter)
{
    /* extract and validate macro name */
    if (!extract_and_validate_macro_name(macro_name, line, *line_counter, (*assembler)->macro_list))
    {
        return false;
    }

    printf("macro_name: -%s- \n", macro_name);

    /* read macro body and check for errors */
    if (!read_macro_body(fp_as, content, line_counter, line))
    {
        memset(line, '\0', MAX_LINE_LENGTH);
        *content = NULL;
        return false;
    }

    /* add macro to macro list */
    add_to_macro_list(&(*assembler)->macro_list, macro_name, *content);

    /* reset temporary content and name */
    *content = NULL;
    memset(macro_name, '\0', MAX_LINE_LENGTH);

    return true;
}


/*
 * Checks if the line is a macro usage or a regular line.
 * Expands macro content or writes the line to the output.
 */
void handle_macro_usage_or_regular_line(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_am)
{
    macro *macro_use = find_macro((*assembler)->macro_list, line);
    macro_content *c;

    /* If line matches a macro name, write its full content */
    if (macro_use != NULL)
    {
        for (c = macro_use->content; c != NULL; c = c->next)
        {
            fprintf(fp_am, "%s", c->content_line);
        }
    }
    /* Otherwise, write the line as-is (if not empty) */
    else if (line[0] != '\n')
    {
        fprintf(fp_am, "%s", line);
    }
}


/*
 * Checks if the line starts a macro definition ("mcro").
 * If so, handles the macro block and updates the macro list.
 * Returns true if this line was a macro definition (success or failure).
 */
bool handle_macro_definition(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_as,
                             char macro_name[MAX_LINE_LENGTH], macro_content **content,
                             int *line_counter, bool *final_error)
{
    /* check if line starts a macro definition (but not 'mcroend') */
    if (strncmp(line, "mcro", strlen("mcro")) == 0 &&
        strncmp(line, "mcroend", strlen("mcroend")) != 0)
    {
        /* handle macro definition and update error status */
        if (!macro_trearment(assembler, line, macro_name, fp_as, content, line_counter))
        {
            *final_error = false;
        }
        return true;
    }

    return false;
}


/*
 * Processes a single line: handles macro definition, usage, or regular line.
 * Returns false if an error occurred, true otherwise.
 */
bool process_line(char line[MAX_LINE_LENGTH], assembler_table **assembler, FILE *fp_as, FILE *fp_am,
                  char macro_name[MAX_LINE_LENGTH], macro_content **content,
                  int *line_counter, bool *final_error)
{
    size_t len;
    int ch;

    /* check for comment errors  */
    if (!handle_notes_error(line, *line_counter))
    {
        *final_error = false;
        return true; /* skip the line, but not a fatal error */
    }

    /* check for overly long line */
    len = strlen(line);
    if (len == MAX_LINE_LENGTH - 1 && line[len - 1] != '\n')
    {
        errors_table(LINE_LENGTH_EXCEED_MAXIMUM, *line_counter);
        *final_error = false;

        /* consume the rest of the long line */
        while ((ch = fgetc(fp_as)) != '\n' && ch != EOF)
            ;
    }

    /* remove extra white spaces */
    remove_white_spaces(line, line);

    /* handle macro definition if found */
    if (handle_macro_definition(line, assembler, fp_as, macro_name, content, line_counter, final_error))
    {
        return true;
    }

    /* handle macro usage or write regular line */
    handle_macro_usage_or_regular_line(line, assembler, fp_am);
    return true;
}

/*
  Main preprocessor function. Reads each line of the ".as" file,
  handles macro definitions and macro usages, and writes the expanded
  version to the ".am" file. In case of errors, deletes the output file.
  Returns true if successful, false otherwise.
*/
bool pre_proc(assembler_table **assembler)
{
    char line[MAX_LINE_LENGTH], macro_name[MAX_LINE_LENGTH];
    macro_content *content = NULL;
    FILE *fp_as, *fp_am;

    /* Counter for line number */
    int *line_counter = generic_malloc(sizeof(int));

    bool final_error = true;
    *line_counter = 1;
    /* Open files and initialize buffers */
    files_initialize(assembler, &fp_as, &fp_am, line, macro_name);

    /* Process lines from .as file */
    while (fgets(line, MAX_LINE_LENGTH, fp_as))
    {
        printf("line %d %s", *line_counter, line);
        process_line(line, assembler, fp_as, fp_am, macro_name, &content, line_counter, &final_error);
        memset(line, '\0', MAX_LINE_LENGTH);
        (*line_counter)++;
    }

    printf("\n\n=== ERRORS ===\n");

    /* If any error occurred, delete the generated file */
    if (final_error == false)
    {
        safe_remove((*assembler)->macro_expanded_file);
    }

    /* Clean up */
    free(line_counter);
    fclose(fp_as);
    fclose(fp_am);

    return final_error;
}
