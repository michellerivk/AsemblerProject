#include "assembler.h"

/* Initialize assembler_table struct with default values.
   Allocates memory and sets all lists and counters to NULL/0.
   Copies the source file name (argv) into the struct. */
assembler_table * initialize_assembler_table(char * argv){
    assembler_table * assembler =  generic_malloc(sizeof(assembler_table));
    assembler->data_section = NULL;          /* Initialize data section pointer */
    assembler->code_section = NULL;          /* Initialize code section pointer */
    assembler->label_list = NULL;            /* Initialize label list pointer */
    assembler->external_list = NULL;         /* Initialize external label list */
    assembler->macro_list = NULL;            /* Initialize macro list */
    strcpy(assembler->source_file , argv);  /* Copy source file name (no extension handling here) */
    memset( assembler->macro_expanded_file, 0, sizeof( assembler->macro_expanded_file)); /* Clear macro expanded file string */
    memset( assembler->assembly_file, 0, sizeof( assembler->assembly_file));              /* Clear assembly file string */
    assembler->instruction_counter = 0 ;     /* Reset instruction counter */
    assembler->data_counter = 0;              /* Reset data counter */
    return assembler;                        /* Return pointer to initialized assembler_table */
}

/* Add a new external_usage node with given address to the linked list.
   If the list is empty, create the first node.
   Otherwise, traverse to the end and append the new node. */
void add_to_external_usage(external_usage ** usage_list , int address){
    external_usage * temp = NULL;
    external_usage  * new_node = generic_malloc(sizeof(external_usage));
    new_node->address = address;  /* Set the address where the external label is used */
    new_node->next = NULL;        /* New node is last in list */

    if(*usage_list == NULL){
        *usage_list = new_node;   /* If list empty, start it */
        return;
    }

    temp = *usage_list;

    /* Traverse to the end of the list */
    while( temp->next != NULL){
       temp = temp->next; 
    }
    temp->next = new_node;  /* Append the new node */
}

/* Add a new macro node with the given name and content to the macro list.
   If list is empty, start it with the new macro.
   Otherwise, append to the end. */
void add_to_macro_list(macro ** head , char * macro_name , macro_content * content){
    macro * temp;
    macro * new_macro = generic_malloc(sizeof(macro));
    strcpy(new_macro->macro_name , macro_name); /* Copy macro name */
    new_macro->content = content;                /* Attach macro content */
    new_macro->next = NULL;                       /* New macro is last */

    if( *head == NULL){
        *head = new_macro;   /* Empty list case */
        return;
    } 

    temp = *head;

    /* Traverse to end */
    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = new_macro;  /* Append new macro */
}

/* Add a new macro_content node with the given content line to the content list.
   If the list is empty, start it with the new node.
   Otherwise, append it to the end. */
void add_to_content_list(macro_content ** head ,char * content ){
    macro_content * temp;
    macro_content * new_content_node = generic_malloc(sizeof(macro_content));
    strcpy(new_content_node->content_line , content); /* Copy content line */
    new_content_node->next = NULL;                      /* New content is last */

    if( *head == NULL){
        *head = new_content_node; /* Empty list case */
        return;
    } 

    temp = *head;

    /* Traverse to end */
    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = new_content_node; /* Append new content */
}
