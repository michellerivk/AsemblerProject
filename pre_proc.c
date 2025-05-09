#include "assembler.h"








void remove_white_spaces(char dest[MAX_LINE_LENGTH] , char * line){
    int i , j = 0;
    char temp[MAX_LINE_LENGTH];
    memset(temp , '\0' , MAX_LINE_LENGTH);

    for(i = 0 ; i < strlen(line) ; i++){
        if(line[i] != ' ' && line[i] != '\t'){
            temp[j] = line[i];
            j++;
        }

        if(line[i] == ';'){
            temp[j] = '\n';
            j++;
            i = strlen(line);
        }
        
    }

    temp[j] = '\0';

    strcpy(dest , temp);


}


int extract_token(char dest[MAX_LINE_LENGTH] , char * line , char delimeter){
    int i , found  = 0;

    for(i = 0 ; i < strlen(line) && found == 0 ; i++){
        if(line[i] != delimeter){
            dest[i] = line[i];
        }
        else{
            found = 1;
        }
    }

    dest[i] = '\0';

    if(found == 1){
        return i;
    }

    return 0;

}

void files_initialize(assembler_table **assembler, FILE **fp_as, FILE **fp_am, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH])
{    
    add_suffix( (*assembler)->assembly_file , (*assembler)->source_file , ".as");
    add_suffix((*assembler)->macro_expanded_file , (*assembler)->source_file, ".am");
    
    *fp_as = safe_fopen((*assembler)->assembly_file, "r");
    *fp_am = safe_fopen((*assembler)->macro_expanded_file, "w");
    
    memset(line, '\0', MAX_LINE_LENGTH);
    memset(macro_name, '\0', MAX_LINE_LENGTH);
    
}

void macro_trearment(assembler_table **assembler, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH], FILE *fp_as, macro_content **content)
{
    
        extract_token(macro_name ,  line + strlen("mcro") , '\n');
        printf("macro_name: -%s- \n" , macro_name);
        memset(line, '\0', MAX_LINE_LENGTH);
        while(fgets(line, MAX_LINE_LENGTH , fp_as) != NULL ){
            
            remove_white_spaces(line , line);
            printf("line %s",line);
            if(strncmp(line , "mcroend" , strlen("mcroend")) != 0){
                add_to_content_list( &(*content) , line );
            }else{
                break;

            }
            memset(line, '\0', MAX_LINE_LENGTH);
        }
        if(strncmp(line , "mcroend" , strlen("mcroend")) == 0){
            printf("Line inside %s ",line);
            add_to_macro_list(&(*assembler)->macro_list , macro_name , *content);
            *content = NULL;
        }
        memset(macro_name , '\0' , MAX_LINE_LENGTH);

    
    
}

int pre_proc(assembler_table ** assembler){
    char  line[MAX_LINE_LENGTH] , macro_name[MAX_LINE_LENGTH];
    macro  * macro_use = NULL;
    macro_content * content = NULL;
    FILE * fp_as , * fp_am;
    
    files_initialize(assembler, &fp_as, &fp_am ,  line, macro_name);
    
    while(fgets(line, MAX_LINE_LENGTH , fp_as)){
        
        remove_white_spaces(line , line);

        printf("line %s",line);

        if(strncmp(line , "mcro" , strlen("mcro")) == 0 && strncmp(line , "mcroend" , strlen("mcroend")) != 0){

        macro_trearment ( assembler, line, macro_name,  fp_as, &content);
        
        }
        else if( (macro_use = find_macro((*assembler)->macro_list , line)) != NULL){
            printf("MACRO USE NAME -%s-",macro_use->macro_name);
            content = macro_use->content;
            while(content != NULL){
                fprintf( fp_am , "%s" , content->content_line);
                content = content->next;
            }
            content = NULL;
            macro_use = NULL;
        }else if (line[0] != '\n'){
            fprintf(fp_am , "%s" , line);
        }
    
    
        memset(line, '\0', MAX_LINE_LENGTH);
        
    }

    
    fclose(fp_as);
    fclose(fp_am);
    return 0;
}