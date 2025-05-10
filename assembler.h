#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 31

#define ENTRY 0
#define R 2
#define E 1
#define SHIFT_AFTER_ARE 2

#define MASK_TWO_BITS 3
#define UP_TO_CHARACTER 65

typedef struct macro_content{
    char content_line[MAX_LINE_LENGTH];
    struct macro_content * next;
}macro_content;

typedef struct macro{
    char macro_name[MAX_LINE_LENGTH];
    struct macro_content * content;
    struct macro * next;
}macro;

typedef struct external_usage {
    int address;
    struct external_usage *next;
} external_usage;

typedef struct external_label {
    char label[MAX_LABEL_LENGTH];
    struct external_usage *usage_list;
    struct external_label *next;
} external_label;

typedef struct label {
    char name[MAX_LABEL_LENGTH];
    int address;
    int type;
    struct label *next;
} label;

typedef struct code_word {
    unsigned short value;
} code_word;

typedef struct command {
    code_word word;
    int address;
    char referenced_label[MAX_LABEL_LENGTH];
    struct command *next;
} command;

typedef struct data {
    code_word word;
    int address;
    struct data *next;
} data;

typedef struct assembler_table {
    struct data *data_section;
    struct command *code_section;
    struct label *label_list;
    struct external_label *external_list;
    struct macro *macro_list;
    char source_file[MAX_LABEL_LENGTH];
    char macro_expanded_file[MAX_LABEL_LENGTH];
    char assembly_file[MAX_LABEL_LENGTH];
    int instruction_counter;
    int data_counter;
} assembler_table;


macro * find_macro(macro * head , char * name);

int extract_token(char dest[MAX_LINE_LENGTH] , char * line , char delimeter);
void remove_white_spaces(char dest[MAX_LINE_LENGTH] , char * line);
void * generic_malloc(long size);
FILE * safe_fopen(char * name , char * mode);
void add_suffix(char *dest, const char *file_name, const char *ending);
int pre_proc(assembler_table ** assembler_table);
void files_initialize(assembler_table **assembler, FILE **fp_as, FILE **fp_am, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH]);
unsigned short move_bits(unsigned short word , unsigned short move);
int second_pass(assembler_table ** assembler_table);
assembler_table * initialize_assembler_table(char * argv);
void add_to_content_list(macro_content ** head ,char * content );
void add_to_macro_list(macro ** head , char * macro_name , macro_content * content);
void add_to_external_usage(external_usage ** usage_list , int address);
void print_macros(const macro *macro);
void print_assembler_table(const assembler_table *table) ;
void translation_unit(assembler_table * assembler);
void trans_base_four(int number , char  dest[]);
void object_file (assembler_table * assembler);
void ext_file(assembler_table * assembler);
void ent_file(assembler_table * assembler);

void files_treatment(assembler_table * assembler);
void macro_trearment(assembler_table **assembler, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH], FILE *fp_as, macro_content **content);


int first_pass(const char *file, assembler_table *table);


/**
  המעבר השני
  להשלים את המילה של התוויות בתוך הרשימה של הCOMMAND
  להשלים את המילה של כתובת מטריצה ברשימה של הCOMMAND
  להשלים את המילים של EXTERN בתוך הרשימה של ה COMMAND
  להשלים הופעות של EXTERN השימוש בתור אופרנד באיזה כתובת בתוך רשימה של EXTERN
*/






