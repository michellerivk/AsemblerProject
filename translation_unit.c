#include "assembler.h"


void trans_base_four(int number , char  dest[]){
    int i , curr_two_bits ,j;
   
    for( i = 8 , j = 0 ; i >= 0  ;i-=2 , j++){
        curr_two_bits = number >> i;
        curr_two_bits = curr_two_bits & MASK_TWO_BITS;
        
        dest[j] =(char)(curr_two_bits + UP_TO_CHARACTER);
    }
    dest[5] = '\0';
}

void object_file (assembler_table * assembler){
    char  file_object[MAX_LABEL_LENGTH] = {0} , dest_addr[MAX_LABEL_LENGTH] = {0} , dest_word[MAX_LABEL_LENGTH] = {0}; 
    command * temp_command = NULL;
    data * temp_data = NULL;
    FILE * fp_ob = NULL;
    char ic_dest [MAX_LABEL_LENGTH] = {0}, dc_dest [MAX_LABEL_LENGTH] = {0};

    add_suffix(file_object, assembler-> source_file, ".ob");
    fp_ob = fopen(file_object, "w"); 

    trans_base_four(assembler->instruction_counter - 100 , ic_dest );
    trans_base_four(assembler->data_counter ,dc_dest );
    fprintf( fp_ob, "\t%s\t%s\n" , ic_dest , dc_dest);
    temp_command = assembler->code_section;
    while(temp_command != NULL){
        trans_base_four(temp_command->address , dest_addr);
        trans_base_four(temp_command->word.value , dest_word);
        fprintf( fp_ob, "%s\t%s\n" , dest_addr , dest_word );
         
        temp_command = temp_command->next;
    }

    temp_data = assembler->data_section;
    while(temp_data != NULL){
        trans_base_four(temp_data->address + assembler->instruction_counter  , dest_addr);
        trans_base_four(temp_data->word.value , dest_word);
        fprintf( fp_ob, "%s\t%s\n" , dest_addr , dest_word );
        temp_data = temp_data->next;
    }
    fclose(fp_ob);

}

void ext_file(assembler_table * assembler){

    char  file_external[MAX_LABEL_LENGTH] = {0};
    char dest_extern_usage[MAX_LABEL_LENGTH] = {0};
    external_label * temp_external_label = NULL;
    FILE * fp_ext = NULL;
    bool file_empty = true;

    add_suffix(file_external, assembler-> source_file, ".ext");
    fp_ext = fopen(file_external, "w");

    temp_external_label = assembler->external_list;
    while( temp_external_label != NULL){
        file_empty = false;
        while(temp_external_label->usage_list != NULL){

            trans_base_four(temp_external_label->usage_list->address, dest_extern_usage);
            fprintf( fp_ext, "%s\t%s\n" , temp_external_label->label , dest_extern_usage );
            

            temp_external_label->usage_list = temp_external_label->usage_list->next;
        }
        
        temp_external_label = temp_external_label->next;
    }

    if(file_empty == true){
        safe_remove(file_external);
    }

    fclose(fp_ext);
}

void ent_file(assembler_table * assembler){
    char  file_entery[MAX_LABEL_LENGTH] = {0} ;
    char dest_label_addr[MAX_LABEL_LENGTH] = {0};
    label * temp_label = NULL;
    FILE * fp_ent = NULL;
    bool file_empty = true;

    add_suffix(file_entery, assembler-> source_file, ".ent");
    fp_ent = fopen(file_entery, "w");

    temp_label = assembler->label_list;
    while(temp_label != NULL){
        
        if(temp_label->type ==  ENTRY){
            file_empty = false;
            trans_base_four(temp_label->address , dest_label_addr);
            fprintf( fp_ent, "%s\t%s\n" , temp_label->name , dest_label_addr );
           
        }
        temp_label = temp_label->next;
    }
  
    if(file_empty == true){
        safe_remove(file_entery);
    }
    
    fclose(fp_ent);
}

void translation_unit(assembler_table * assembler){

    object_file (assembler);
    ent_file(assembler);
    ext_file(assembler);
}