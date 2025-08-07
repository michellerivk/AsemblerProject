#include "assembler.h"

/* Free a linked list of data nodes */
void free_data_section(data *head)
{
    data *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/*
* Checks for invalid comment positioning in a line.
* A comment (';') must appear only at the beginning of the line.
*/
bool handle_notes_error(char line[], int line_counter)
{
    int i;

    /* scan for comment ';' that appears after whitespace */
    for (i = 1; i < strlen(line) - 1; i++)
    {
        if (isspace(line[i - 1]) && line[i] == ';')
        {
            /* report error*/
            errors_table(ERROR_NOTE_WITH_SPACE , line_counter);
            return false;
        }
    }

    return true;
}


/* Free a linked list of command nodes */
void free_code_section(command *head)
{
    command *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of label nodes */
void free_label_list(label *head)
{
    label *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of external_usage nodes */
void free_external_usage_list(external_usage *head)
{
    external_usage *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of external_label nodes, including their usage lists */
void free_external_list(external_label *head)
{
    external_label *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free_external_usage_list(tmp->usage_list);
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of macro_content nodes */
void free_macro_content_list(macro_content *head)
{
    macro_content *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of macro nodes, including their content */
void free_macro_list(macro *head)
{
    macro *tmp;
    while (head)
    {
        tmp = head;
        head = head->next;
        free_macro_content_list(tmp->content);
        free(tmp);
        tmp = NULL;
    }
}

/*
 * Frees all dynamically allocated memory used by the assembler table.
 */
void free_assembler_table(assembler_table *table)
{
    if (!table)
        return;

    /* Free each section of the table */
    free_data_section(table->data_section);
    free_code_section(table->code_section);
    free_label_list(table->label_list);
    free_external_list(table->external_list);
    free_macro_list(table->macro_list);

    /* Free the table struct itself */
    free(table);
    table = NULL;
}

/*
 * Shifts the bits of a given word to the left by a specified amount.
 */
unsigned short move_bits(unsigned short word, unsigned short move)
{
    return word << move;
}

/*
 * Allocates memory safely. If allocation fails, an error is printed and program exits.
 */
void *generic_malloc(long size)
{
    void *ptr = malloc(size);
    /* Check if memory allocation failed */
    if (ptr == NULL)
    {
        errors_table(MALLOC_FAILED, -1);
        exit(1); /* Exit the program if memory allocation fails */
    }
    return ptr; /* Return pointer to allocated memory */
}

/*
 * Opens a file safely. Exits the program if file cannot be opened.
 */
FILE *safe_fopen(char *name, char *mode)
{
    FILE *fp = fopen(name, mode);
    /* If fopen failed, report and exit */
    if (fp == NULL)
    {
        errors_table(FAILED_TO_OPEN_FILE, -1);
        exit(1);
    }
    return fp;
}

/*
 * Appends a suffix to a file name to create a new file name.
 */
void add_suffix(char *dest, const char *file_name, const char *ending)
{

    /* Copy base file name to destination */
    strcpy(dest, file_name);

    /* Append desired ending to destination */
    strcat(dest, ending);
}

/*
 * Searches for a macro by name in a linked list of macros.
 */
macro *find_macro(macro *head, char *name)
{
    char temp[MAX_LINE_LENGTH];
    memset(temp, '\0', MAX_LINE_LENGTH); /* Clear temp buffer */

    extract_token(temp, name, '\n'); /* Remove trailing newline */
    while (head != NULL)
    {
        /* Compare macro name */
        if (strcmp(head->macro_name, temp) == 0)
        {
            return head;
        }
        head = head->next;
    }

    return NULL; /* Not found */
}

/*
 * Prints an error message corresponding to a given error code.
 */
void errors_table(ERRORS error_code, int line_counter)
{
    switch (error_code)
    {
    case FILE_NAME_EXCEED_MAXIMUM:
        printf("Error: File name exceed the maximum length.\n");
        break;
    case ERROR_RESERVED_WORD:
        printf("Error on line %d: The name used is a reserved word and cannot be used.\n", line_counter);
        break;
    case ERROR_INVALID_MACRO_NAME:
        printf("Error on line %d: Invalid macro name.\n", line_counter);
        break;
    case ERROR_TEXT_AFTER_MACROEND:
        printf("Error on line %d: Unexpected text after 'macroend'.\n", line_counter);
        break;
    case MISSING_MACRO_NAME:
        printf("Error on line %d: Missing macro name.\n", line_counter);
        break;
    case EXCEED_MAXIMUM_MACRO_LENGTH:
        printf("Error on line %d: Macro name exceed maximum length.\n", line_counter);
        break;
    case MACRO_ALREADY_DEFINED:
        printf("Error on line %d: Macro name already defined.\n", line_counter);
        break;
    case FAILED_TO_REMOVE_FILE:
        printf("Failed to remove file.\n");
        break;
    case FAILED_TO_OPEN_FILE:
        printf("Failed to open file.\n");
        break;
    case MALLOC_FAILED:
        printf("Malloc fail.\n");
        break;
    case LINE_LENGTH_EXCEED_MAXIMUM:
        printf("\nLine length is over then 80 chars .\n");
        break;
    case ERROR_NOTE_WITH_SPACE:
        printf("Error on line %d: Invalid Note cannot have whitespaces before .\n" , line_counter);
        break;
    default:
        printf("Error on line %d: Unknown error code.\n", line_counter);
        break;
    }
}

/*
 * Prints an error message of the first pass corresponding to a given error code.
 */
void first_pass_errors(FIRST_PASS_ERRORS error_code, int line, int error_counter)
{
    switch (error_code)
    {
    case ERR_AM_FILE:
        printf("ERROR: Could not open the .am file\n");
        break;

    case ERR_NOT_COMMAND_OR_DIRECTIVE:
        printf("ERROR on line %d: Something other than a command or a directive was entered after the label. \n", line);
        break;

    case ERR_FIRST_PASS:
        printf("First pass failed.\n");
        break;

    case ERR_AMOUNT_OF_ERRORS:
        printf("%d ERRORS has been detected on the first pass.\n", error_counter);
        break;

    case ERR_LABEL_INVALID:
        printf("ERROR on line %d: The label is invalid / doesn't exist.\n", line);
        break;

    case ERR_UNKNOWN_DIRECTIVE:
        printf("ERROR on line %d: The directive after label is not known\n", line);
        break;

    case ERR_NOT_A_NUMBER:
        printf("ERROR on line %d: The argument has to be a number!\n", line);
        break;

    case ERR_NO_QUOTATION_MARKS:
        printf("ERROR on line %d: There are no quotation marks straightly after .string\n", line);
        break;

    case ERR_LABEL_IS_NOT_ALPHANUMERIC:
        printf("ERROR on line %d: The label includes a character other than a digit or a letter.\n", line);
        break;

    case ERR_LABEL_ENDING:
        printf("ERROR on line %d: The label cannot end with a character other than a ':'\n", line);
        break;

    case ERR_OPCODE:
        printf("ERROR on line %d: The given opcode is invalid\n", line);
        break;

    case ERR_LABEL_RESERVED:
        printf("ERROR on line %d: The label cannot be a reserved word of the assembler.\n", line);
        break;

    case ERR_LABEL_START:
        printf("ERROR on line %d: The label has to start with a letter.\n", line);
        break;

    case ERR_EXTERNAL_LABEL_EXISTS:
        printf("ERROR on line %d: External label already declared.\n", line);
        break;

    case ERR_LABEL_EXISTS:
        printf("ERROR on line %d: The label already exists with the same type.\n", line);
        break;

    case ERR_INVALID_MATRIX:
        printf("ERROR on line %d: The matrix is invalid\n", line);
        break;

    case ERR_INVALID_MAT_ARGUMENT:
        printf("ERROR on line %d: .mat recieved an invalid argument\n", line);
        break;

    case ERR_MAT_WRONG_AMOUNT_OF_VALUES:
        printf("ERROR on line %d: The matrix didn't get the right amount of values\n", line);
        break;

    case ERR_CLOSING_QUOTATION_MARK:
        printf("ERROR on line %d: Missing closing quotation mark in .string directive\n", line);
        break;

    case ERR_INVALID_OP:
        printf("ERROR in line %d: Invalid operator\n", line);
        break;

    case ERR_INVALID_SRC_OP:
        printf("ERROR in line %d: Invalid source operand\n", line);
        break;

    case ERR_INVALID_DEST_OP:
        printf("ERROR in line %d: Invalid destination operand\n", line);
        break;

    case ERR_SHOULD_NOT_HAVE_OP:
        printf("ERROR in line %d: The command should not have operands\n", line);
        break;

    case ERR_MISSING_OPERAND:
        printf("ERROR in line %d: The command is missing its operand\n", line);
        break;

    case ERR_TOO_MANY_OPERANDS:
        printf("ERROR in line %d: The command has too many operands\n", line);
        break;

    case ERR_SHOULD_HAVE_TWO_OP:
        printf("ERROR in line %d: The command should have two operands\n", line);
        break;

    case ERR_MAX_MEMORY:
        printf("ERROR: The program exceeds the maximum memory limit\n");
        break;

    case ERR_MISSING_BRACKET:
        printf("ERROR in line %d: There is a missing bracket\n", line);
        break;
    
    case ERR_DOUBLE_COMMA:
        printf("ERROR in line %d: There is a double comma\n", line);
        break;

    default:
        printf("Error on line %d: Unknown error code.\n", line);
        break;
    }
}

/*
 * Safely removes a file by name. If failed, prints an error and exits.
 */
void safe_remove(const char *filename)
{
    if (filename == NULL || strlen(filename) == 0)
    {
        return; /* Do nothing if filename is invalid */
    }

    if (remove(filename) != 0)
    {
        errors_table(FAILED_TO_REMOVE_FILE, -1);
        exit(1);
    }
}

/*
 * Prints all macros and their contents.
 */
void print_macros(const macro *macro_list)
{
    const macro *current_macro = macro_list;
    const macro_content *content = NULL;
    int macro_index = 1, line_num = 0;

    while (current_macro != NULL)
    {
        printf("=== Macro %d ===\n", macro_index++);
        printf("Macro name: %s\n", current_macro->macro_name);

        content = current_macro->content;
        line_num = 1;

        while (content != NULL)
        {
            printf(" Line %d: %s\n", line_num++, content->content_line);
            content = content->next;
        }

        current_macro = current_macro->next;
    }
}

/*
 * Prints a 10-bit binary representation of a value.
 */
void print_binary_10bit(unsigned int value)
{
    int i;
    value &= 0x3FF; /* Mask value to 10 bits */
    for (i = 9; i >= 0; i--)
    {
        putchar((value & (1 << i)) ? '1' : '0');
    }
}

/*
 * Prints the full contents of an assembler table.
 */
void print_assembler_table(const assembler_table *table)
{
    const label *lbl;
    const external_label *ext;
    const external_usage *usage;
    const data *d;
    const command *cmd;

    if (table == NULL)
    {
        printf("Assembler Table is NULL.\n");
        return;
    }

    printf("=== Assembler Table ===\n");
    printf("Source file: %s\n", table->source_file);
    printf("Macro expanded file: %s\n", table->macro_expanded_file);
    printf("Assembly file: %s\n", table->assembly_file);
    printf("Instruction Counter (IC): %d\n", table->instruction_counter);
    printf("Data Counter (DC): %d\n\n", table->data_counter);

    /* Print labels list */
    printf("--- Labels ---\n");
    lbl = table->label_list;
    while (lbl != NULL)
    {
        printf("Label: %s | Address: %d | Type: %d\n", lbl->name, lbl->address, lbl->type);
        lbl = lbl->next;
    }

    /* Print externals */
    printf("\n--- Externals ---\n");
    ext = table->external_list;
    while (ext != NULL)
    {
        printf("Extern label: %s\n", ext->label);
        usage = ext->usage_list;
        while (usage != NULL)
        {
            printf("  Used at address: %d\n", usage->address);
            usage = usage->next;
        }
        ext = ext->next;
    }

    /* Print data section */
    printf("\n--- Data Section ---\n");
    d = table->data_section;
    while (d != NULL)
    {
        printf("Address: %d | Word (bin): ", d->address);
        print_binary_10bit(d->word.value);
        printf("\n");
        d = d->next;
    }

    /* Print code section */
    printf("\n--- Code Section ---\n");
    cmd = table->code_section;
    while (cmd != NULL)
    {
        printf("Address: %d | Word (bin): ", cmd->address);
        print_binary_10bit(cmd->word.value);
        if (cmd->referenced_label[0] != '\0')
        {
            printf(" | Refers to: %s", cmd->referenced_label);
        }
        printf("\n");
        cmd = cmd->next;
    }

    /* Print macros list */
    printf("\n--- Macros ---\n");
    print_macros(table->macro_list);
}
