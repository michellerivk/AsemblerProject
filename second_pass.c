#include "assembler.h"

void labels_check(assembler_table **assembler) {
    label *ptr_label1 = (*assembler)->label_list;
    label *ptr_label2;

    while (ptr_label1 != NULL) {
        ptr_label2 = (*assembler)->label_list;

        if (ptr_label2->type == ENTRY) {
            while (ptr_label2 != NULL) {
                if (strcmp(ptr_label2->name, ptr_label1->name) == 0 &&
                    ptr_label2->type != ENTRY) {
                    ptr_label1->address = ptr_label2->address;
                }
                ptr_label2 = ptr_label2->next;
            }
        }

        ptr_label1 = ptr_label1->next;
    }
}



void complement_label_word(assembler_table ** assembler,  command * ptr_cmd){

    label * ptr_label = (*assembler)->label_list;
    while(ptr_label != NULL){
        if(strcmp(ptr_cmd->referenced_label ,ptr_label->name ) == 0 ){
            ptr_cmd->word.value = move_bits(ptr_label->address ,  SHIFT_AFTER_ARE) |  R;
            
            break;
        }
          
          ptr_label = ptr_label->next;
    }
}

void complement_ext_word(assembler_table ** assembler,  command * ptr_cmd){
    external_label * ptr_label_ex = (*assembler)->external_list;
    while(ptr_label_ex != NULL){
        if(strcmp(ptr_cmd->referenced_label ,ptr_label_ex->label ) == 0 ){
            ptr_cmd->word.value = E;
            add_to_external_usage( &(ptr_label_ex->usage_list) , ptr_cmd->address);
            break;
        }
        ptr_label_ex = ptr_label_ex->next;
    }
}


int second_pass(assembler_table ** assembler){

    command * ptr_cmd = (*assembler)->code_section;
    
    labels_check( assembler );
    while(ptr_cmd != NULL){
        if(strcmp(ptr_cmd->referenced_label , "") != 0){
            
            complement_label_word ( assembler,  ptr_cmd);
            
            complement_ext_word (assembler, ptr_cmd);


        }


        ptr_cmd = ptr_cmd->next;
    }



    return 0;
}
  
    

