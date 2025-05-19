#include "assembler.h"

bool labels_check(assembler_table **assembler) {
    label *ptr_label1 = (*assembler)->label_list;
    label *ptr_label2;
    bool find_entry_definition ,find_definition_twice ,  error = true;

    while (ptr_label1 != NULL) {
        
        if (ptr_label1->type == ENTRY) {
            ptr_label2 = (*assembler)->label_list;
            find_entry_definition = false;
            while (ptr_label2 != NULL) {
                if (strcmp(ptr_label2->name, ptr_label1->name) == 0 &&
                    ptr_label2->type != ENTRY) {
                    ptr_label1->address = ptr_label2->address;
                    find_entry_definition = true;
                    break;
                }
                ptr_label2 = ptr_label2->next;
            }

            if(find_entry_definition == false){
                printf("Error: Entry Label is undefined the label name is %s \n",ptr_label1->name);
                error = false;
            }
        }

        if (ptr_label1->type != ENTRY) {
            ptr_label2 = ptr_label1->next;
            find_definition_twice = false;
            while (ptr_label2 != NULL) {
                if (strcmp(ptr_label2->name, ptr_label1->name) == 0 &&
                    ptr_label2->type != ENTRY) {
                    find_definition_twice = true;
                    break;
                }
                ptr_label2 = ptr_label2->next;
            }

            if(find_definition_twice == true){
                printf("Error: Label is already defined , the label name is %s \n",ptr_label1->name);
                error = false;
            }
        }
        

   

        ptr_label1 = ptr_label1->next;
    }

    return error;
}



bool complement_label_word(assembler_table ** assembler,  command * ptr_cmd){
    label * ptr_label = (*assembler)->label_list;
    while(ptr_label != NULL){
        if(strcmp(ptr_cmd->referenced_label ,ptr_label->name ) == 0 ){
            if(ptr_label->type == EXTERNAL){
                ptr_cmd->word.value = E;
            }else{
                ptr_cmd->word.value = move_bits(ptr_label->address ,  SHIFT_AFTER_ARE) |  R;
            }
            return true;
        }
          
          ptr_label = ptr_label->next;
    }
    return false;
}

bool complement_ext_word(assembler_table ** assembler,  command * ptr_cmd){
    external_label * ptr_label_ex = (*assembler)->external_list;
    while(ptr_label_ex != NULL){
        printf("CMP -%s- WITH -%s- \n",ptr_cmd->referenced_label ,ptr_label_ex->label );
        if(strcmp(ptr_cmd->referenced_label ,ptr_label_ex->label ) == 0 ){
            
            ptr_cmd->word.value = E;
            
            add_to_external_usage( &(ptr_label_ex->usage_list) , ptr_cmd->address);
            return true;
        }
        
        ptr_label_ex = ptr_label_ex->next;
    }
    return false;
}

bool check_if_label_defined_as_extern(assembler_table ** assembler){
    external_label * ptr_label_ext = (*assembler)->external_list;
    label * ptr_label ;
    bool error = true , didnt_find_dou_definition;
    while(ptr_label_ext != NULL){
        ptr_label = (*assembler)->label_list;
        didnt_find_dou_definition = true;
        while(ptr_label != NULL){
            if(strcmp(ptr_label->name , ptr_label_ext->label) == 0){
                didnt_find_dou_definition = false;
                break;
            }
    
            ptr_label = ptr_label->next;
        }

        if(didnt_find_dou_definition == false){
            printf("The label %s already defined as Extern\n",ptr_label_ext->label);
            error = false;
        }

        ptr_label_ext = ptr_label_ext->next;
    }

    return error;
}


bool second_pass(assembler_table ** assembler){
    bool error = true , final_error = true;
    command * ptr_cmd = (*assembler)->code_section;
    
    error = labels_check(assembler);
    if(error == false){
        final_error = false;
        error = true;
    }

    while(ptr_cmd != NULL){
        if (ptr_cmd->referenced_label[0] != '\0'){
            
            error = complement_label_word ( assembler,  ptr_cmd);

            if(error != true){
                error = complement_ext_word(assembler, ptr_cmd);
                if(error == false){
                    final_error = false;
                }
            }

            if(error == false){
                final_error = false;
                printf("The undefined label name is: %s \n",ptr_cmd->referenced_label);
                error = true;
            }

        }

        ptr_cmd = ptr_cmd->next;
    }


    return final_error;
}
  
    

