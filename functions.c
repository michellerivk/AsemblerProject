#include "assembler.h"


unsigned short move_bits(unsigned short word , unsigned short move){
    return word << move;
}

void * generic_malloc(long size){
    void * ptr = malloc(size);
    /*Check if memory allocation failed*/
    if(ptr == NULL){ 

        exit(1);/* Exit the program if memory allocation fails */
    }    
    return ptr; /* Return void pointer to allocated memory */
}

FILE * safe_fopen(char * name , char * mode){
    FILE *fp = fopen(name, mode);
    if(fp == NULL){
    
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
        printf("Address: %d | Word: 0x%04X\n", d->address, d->word.value);
        d = d->next;
    }

    /* Print code section */
    printf("\n--- Code Section ---\n");
    cmd = table->code_section;
    while (cmd != NULL) {
        printf("Address: %d | Word: 0x%04X", cmd->address, cmd->word.value);
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



