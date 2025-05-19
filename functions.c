#include "assembler.h"


unsigned short move_bits(unsigned short word , unsigned short move){
    return word << move;
}

void * generic_malloc(long size){
    void * ptr = malloc(size);
    /*Check if memory allocation failed*/
    if(ptr == NULL){ 
        errors_table(MALLOC_FAILED , -1);
        exit(1);/* Exit the program if memory allocation fails */
    }    
    return ptr; /* Return void pointer to allocated memory */
}

FILE * safe_fopen(char * name , char * mode){
    FILE *fp = fopen(name, mode);
    if(fp == NULL){
        errors_table(FAILED_TO_OPEN_FILE , -1);
        exit(1);
    }
    return fp;
}



void add_suffix(char *dest, const char *file_name, const char *ending) {

    /* Start by copying file_name into dest */
    strcpy(dest, file_name);

    /* Append ending */
    strcat(dest, ending);
}



macro * find_macro(macro * head , char * name){
    char temp[MAX_LINE_LENGTH];
    memset(temp , '\0' , MAX_LINE_LENGTH);
    extract_token(temp , name , '\n');
    while(head != NULL){
        if(strcmp(head->macro_name , temp) == 0){
            return head;
        }

        head = head->next;
    }

    return NULL;
}


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
        default:
            printf("Error on line %d: Unknown error code.\n", line_counter);
            break;
    }
}

/* Safe remove function */
void safe_remove(const char *filename) {
    if (filename == NULL || strlen(filename) == 0) {
        return; 
    }

    if (remove(filename) != 0) {
        errors_table(FAILED_TO_REMOVE_FILE, -1);
        exit(1);
    }
}

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


void print_binary_10bit(unsigned int value) {
    int i;
    value &= 0x3FF; /* Mask to 10 bits */
    for (i = 9; i >= 0; i--) {
        putchar((value & (1 << i)) ? '1' : '0');
    }
}

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

    /* Print labels */
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

    /* Print macros */
    printf("\n--- Macros ---\n");
    print_macros(table->macro_list);
}





