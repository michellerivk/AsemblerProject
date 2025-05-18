#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 31

#define ENTRY 0
#define R 2
#define E 1
#define SHIFT_AFTER_ARE 2

#define MASK_TWO_BITS 3
#define UP_TO_CHARACTER 65

/* Label deffinitions for the first_pass() */
#define CODE 0
#define DATA 1
#define STRING 2
#define MATRIX 3
#define EXTERNAL 4

/* Create enum for boolean expressions */
typedef enum { false, true } bool;


typedef enum ERRORS{
    FILE_NAME_EXCEED_MAXIMUM,
    ERROR_RESERVED_WORD,
    ERROR_INVALID_MACRO_NAME,
    ERROR_TEXT_AFTER_MACROEND,
    EXCEED_MAXIMUM_MACRO_LENGTH,
    MISSING_MACRO_NAME,
    MACRO_ALREADY_DEFINED,
    FAILED_TO_OPEN_FILE,
    FAILED_TO_REMOVE_FILE,
    MALLOC_FAILED
}ERRORS;


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

typedef struct command_parts {
    int opcode;
    int source_addr;
    int dest_addr;
    int ARE;
} command_parts;

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
bool pre_proc(assembler_table ** assembler);
void files_initialize(assembler_table **assembler, FILE **fp_as, FILE **fp_am, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH]);
unsigned short move_bits(unsigned short word , unsigned short move);

bool complement_label_word(assembler_table ** assembler,  command * ptr_cmd);
bool complement_ext_word(assembler_table ** assembler,  command * ptr_cmd);
bool second_pass(assembler_table ** assembler);
bool labels_check(assembler_table **assembler);
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
bool macro_trearment(assembler_table **assembler, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH], FILE *fp_as, macro_content **content , int * line_couner);
bool macro_name_examine(char macro_name[] , int line_counter , macro * macro_list);
bool examine_macroend(char line[] , int line_counter);
void errors_table(ERRORS error_code, int line_counter);
void safe_remove(const char *filename);



int first_pass(const char *file, assembler_table *table);




/*=========Michelle header: ==================*/



/* Call helping functions so that the main function can use them*/
/*
int first_pass(const char *am_file);
int macro(const char *as_file);
char *add_ending(const char *file_name, const char *ending);
*/

void check_line(char *line, int line_number, assembler_table *table, int *error_count, bool label_flag);

char *get_label(char *line, int i);

int is_command_ok(char *word);

void add_label_to_table();

int check_for_label(label *list, char *label);

int is_number_ok(char *input);

int get_instruction(char *com);
int is_reserved_word(const char *label);


#endif /* ASSEMBLER_H */


