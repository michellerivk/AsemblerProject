#include "assembler.h"


bool macro_name_examine(char macro_name[] , int line_counter , macro * macro_list){
    int i;

    if(strlen(macro_name) > MAX_LABEL_LENGTH){
        errors_table(EXCEED_MAXIMUM_MACRO_LENGTH, line_counter);
        return false;
    }

    if(macro_name[0] == '\0'){
        errors_table(MISSING_MACRO_NAME, line_counter);
        return false;
    }

    if(find_macro(macro_list , macro_name) != NULL){
        errors_table(MACRO_ALREADY_DEFINED, line_counter);
        return false;
    }

    if (is_reserved_word(macro_name) == true){
        errors_table(ERROR_RESERVED_WORD, line_counter);
        return false;
    }

    if( !islower(macro_name[0]) && !isupper(macro_name[0]) && macro_name[0] != '_' ){
        errors_table(ERROR_INVALID_MACRO_NAME, line_counter);
        return false;
    }

    for(i = 1 ; i < strlen(macro_name) ; i++){
        if(!islower(macro_name[i]) && !isupper(macro_name[i]) && !isdigit(macro_name[i]) && macro_name[i] != '_'){
            errors_table(ERROR_INVALID_MACRO_NAME, line_counter);
            return false;
        }

    }

    return true;
    
}

bool examine_macroend(char line[] , int line_counter){
    if(line[0] != '\n'){
        errors_table(ERROR_TEXT_AFTER_MACROEND, line_counter);
        return false;
    }

    return true;
}


