#include "assembler.h"



int main(int argc , char ** argv){
    assembler_table * assembler = NULL;
    int i;
    for(i = 1 ; i < argc ; i++){

        assembler = initialize_assembler_table(argv[i]);
        

        pre_proc(&assembler);

        print_assembler_table(assembler);

        print_macros(assembler->macro_list);

        if (!first_pass(argv[i], assembler)) 
        {
            printf("First pass failed.\n");
            return 1;
        }

        second_pass(&assembler);

        translation_unit(assembler);
        
    }



    return 0;
}