#include "assembler.h"


/* Free a linked list of data nodes */
void free_data_section(data *head) {
    data *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of command nodes */
void free_code_section(command *head) {
    command *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of label nodes */
void free_label_list(label *head) {
    label *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of external_usage nodes */
void free_external_usage_list(external_usage *head) {
    external_usage *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of external_label nodes, including their usage lists */
void free_external_list(external_label *head) {
    external_label *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free_external_usage_list(tmp->usage_list);
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of macro_content nodes */
void free_macro_content_list(macro_content *head) {
    macro_content *tmp;
    while (head) {
        tmp = head;
        head = head->next;
        free(tmp);
        tmp = NULL;
    }
}

/* Free a linked list of macro nodes, including their content */
void free_macro_list(macro *head) {
    macro *tmp;
    while (head) {
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
void free_assembler_table(assembler_table *table) {
    if (!table) return;

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
unsigned short move_bits(unsigned short word , unsigned short move){
    return word << move;
}

/* 
 * Allocates memory safely. If allocation fails, an error is printed and program exits.
 */
void * generic_malloc(long size){
    void * ptr = malloc(size);
    /* Check if memory allocation failed */
    if(ptr == NULL){ 
        errors_table(MALLOC_FAILED , -1);
        exit(1); /* Exit the program if memory allocation fails */
    }    
    return ptr; /* Return pointer to allocated memory */
}

/* 
 * Opens a file safely. Exits the program if file cannot be opened.
 */
FILE * safe_fopen(char * name , char * mode){
    FILE *fp = fopen(name, mode);
    /* If fopen failed, report and exit */
    if(fp == NULL){
        errors_table(FAILED_TO_OPEN_FILE , -1);
        exit(1);
    }
    return fp;
}

/* 
 * Appends a suffix to a file name to create a new file name.
 */
void add_suffix(char *dest, const char *file_name, const char *ending) {

    /* Copy base file name to destination */
    strcpy(dest, file_name);

    /* Append desired ending to destination */
    strcat(dest, ending);
}

/* 
 * Searches for a macro by name in a linked list of macros.
 */
macro * find_macro(macro * head , char * name){
    char temp[MAX_LINE_LENGTH];
    memset(temp , '\0' , MAX_LINE_LENGTH); /* Clear temp buffer */

    extract_token(temp , name , '\n'); /* Remove trailing newline */
    while(head != NULL){
        /* Compare macro name */
        if(strcmp(head->macro_name , temp) == 0){
            return head;
        }
        head = head->next;
    }

    return NULL; /* Not found */
}

/* 
 * Prints an error message corresponding to a given error code.
 */
void errors_table(ERRORS error_code, int line_counter) {
    switch (error_code) {
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
            printf("Line length is over then 80 chars .\n");
            break;
            
        default:
            printf("Error on line %d: Unknown error code.\n", line_counter);
            break;
    }
}

/* 
 * Safely removes a file by name. If failed, prints an error and exits.
 */
void safe_remove(const char *filename) {
    if (filename == NULL || strlen(filename) == 0) {
        return; /* Do nothing if filename is invalid */
    }

    if (remove(filename) != 0) {
        errors_table(FAILED_TO_REMOVE_FILE, -1);
        exit(1);
    }
}

/* 
 * Prints all macros and their contents.
 */
void print_macros(const macro *macro_list) {
    const macro *current_macro = macro_list;
    const macro_content *content = NULL;
    int macro_index = 1, line_num = 0;

    while (current_macro != NULL) {
        printf("=== Macro %d ===\n", macro_index++);
        printf("Macro name: %s\n", current_macro->macro_name);

        content = current_macro->content;
        line_num = 1;

        while (content != NULL) {
            printf(" Line %d: %s\n", line_num++, content->content_line);
            content = content->next;
        }

        current_macro = current_macro->next;
    }
}

/* 
 * Prints a 10-bit binary representation of a value.
 */
void print_binary_10bit(unsigned int value) {
    int i;
    value &= 0x3FF; /* Mask value to 10 bits */
    for (i = 9; i >= 0; i--) {
        putchar((value & (1 << i)) ? '1' : '0');
    }
}

/* 
 * Prints the full contents of an assembler table.
 */
void print_assembler_table(const assembler_table *table) {
    const label *lbl;
    const external_label *ext;
    const external_usage *usage;
    const data *d;
    const command *cmd;

    if (table == NULL) {
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
    while (lbl != NULL) {
        printf("Label: %s | Address: %d | Type: %d\n", lbl->name, lbl->address, lbl->type);
        lbl = lbl->next;
    }

    /* Print externals */
    printf("\n--- Externals ---\n");
    ext = table->external_list;
    while (ext != NULL) {
        printf("Extern label: %s\n", ext->label);
        usage = ext->usage_list;
        while (usage != NULL) {
            printf("  Used at address: %d\n", usage->address);
            usage = usage->next;
        }
        ext = ext->next;
    }

    /* Print data section */
    printf("\n--- Data Section ---\n");
    d = table->data_section;
    while (d != NULL) {
        printf("Address: %d | Word (bin): ", d->address);
        print_binary_10bit(d->word.value);
        printf("\n");
        d = d->next;
    }

    /* Print code section */
    printf("\n--- Code Section ---\n");
    cmd = table->code_section;
    while (cmd != NULL) {
        printf("Address: %d | Word (bin): ", cmd->address);
        print_binary_10bit(cmd->word.value);
        if (cmd->referenced_label[0] != '\0') {
            printf(" | Refers to: %s", cmd->referenced_label);
        }
        printf("\n");
        cmd = cmd->next;
    }

    /* Print macros list */
    printf("\n--- Macros ---\n");
    print_macros(table->macro_list);
}
