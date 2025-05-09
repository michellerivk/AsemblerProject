#include "assembler.h"

assembler_table * initialize_assembler_table(char * argv){
    assembler_table * assembler =  generic_malloc(sizeof(assembler_table));
    assembler->data_section = NULL;
    assembler->code_section = NULL;
    assembler->label_list = NULL;
    assembler->external_list = NULL;
    assembler->macro_list = NULL;
    strcpy(assembler->source_file , argv); /*no extend*/
    memset( assembler->macro_expanded_file, 0, sizeof( assembler->macro_expanded_file));
    memset( assembler->assembly_file, 0, sizeof( assembler->assembly_file));
    assembler->instruction_counter = 0 ;
    assembler->data_counter = 0;
    return assembler;
}

void add_to_external_usage(external_usage ** usage_list , int address){
    external_usage * temp = NULL;
    external_usage  * new_node = generic_malloc(sizeof(external_usage));
    new_node->address = address;
    new_node->next = NULL;

    if(*usage_list == NULL){
        *usage_list = new_node;
        return;
    }

    temp = *usage_list;

    while( temp->next != NULL){
       temp = temp->next; 
    }
    temp->next = new_node;
    

}


void add_to_macro_list(macro ** head , char * macro_name , macro_content * content){
    macro * temp;
    macro * new_macro = generic_malloc(sizeof(macro));
    strcpy(new_macro->macro_name , macro_name);
    new_macro->content = content;
    new_macro->next = NULL;

  

    if( *head == NULL){
        *head = new_macro;
        return;
    } 

    temp = *head;

    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = new_macro;

}

void add_to_content_list(macro_content ** head ,char * content ){
    macro_content * temp;
    macro_content * new_content_node = generic_malloc(sizeof(macro_content));
    strcpy(new_content_node->content_line , content);
    new_content_node->next = NULL;

  
    if( *head == NULL){
        *head = new_content_node;
        return;
    } 

    temp = *head;

    while(temp->next != NULL){
        temp = temp->next;
    }

    temp->next = new_content_node;

}