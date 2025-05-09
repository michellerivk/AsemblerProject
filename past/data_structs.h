#define MAX_LINE_LENGTH 81
#define MAX_LABEL_LENGTH 31

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
    external_usage *usage_list;
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
