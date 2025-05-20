#include "assembler.h"



int main(int argc , char ** argv){
    assembler_table * assembler = NULL;
    int i;
    for(i = 1 ; i < argc ; i++){

        if(strlen(argv[i]) > MAX_LABEL_LENGTH){
            errors_table(FILE_NAME_EXCEED_MAXIMUM, -1);
            return false;
        }

        assembler = initialize_assembler_table(argv[i]);
        
        if(pre_proc(&assembler) == false){
            return false;
        }


        /*print_macros(assembler->macro_list);*/

        /*print_assembler_table(assembler);*/

        if (!first_pass(argv[i], assembler)) 
        {
            printf("First pass failed.\n");
            return 1;
        }

        puts("=============First Pass Table:==================");
        print_assembler_table(assembler); /* ########debugging####### */

        puts("==========Start second pass============");

        if(second_pass(&assembler) == false){
            return false;
        }

        puts("=============Second Pass Table:==================");
        print_assembler_table(assembler);

        translation_unit(assembler);
        
    }



    return 0;
}