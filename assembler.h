#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Maximum length of a line in source code file */
#define MAX_LINE_LENGTH 81

/* Maximum length of a label */
#define MAX_LABEL_LENGTH 31

/* Internal constants for assembler usage */
#define ENTRY 5
#define R 2
#define E 1
#define SHIFT_AFTER_ARE 2
#define MAX_MEMORY 255

#define MASK_TWO_BITS 3
#define UP_TO_CHARACTER 0x61

/* Label types used during first pass */
#define CODE 0
#define DATA 1
#define STRING 2
#define MATRIX 3
#define EXTERNAL 4

/* Boolean enum type */
typedef enum
{
    false,
    true
} bool;

/* Possible error codes in the assembler */
typedef enum ERRORS
{
    FILE_NAME_EXCEED_MAXIMUM,    /* File name too long */
    ERROR_RESERVED_WORD,         /* Use of reserved word */
    ERROR_INVALID_MACRO_NAME,    /* Invalid macro name */
    ERROR_TEXT_AFTER_MACROEND,   /* Unexpected text after 'macroend' */
    EXCEED_MAXIMUM_MACRO_LENGTH, /* Macro name too long */
    MISSING_MACRO_NAME,          /* Macro name missing */
    MACRO_ALREADY_DEFINED,       /* Macro already defined */
    FAILED_TO_OPEN_FILE,         /* Failed to open file */
    FAILED_TO_REMOVE_FILE,       /* Failed to remove file */
    MALLOC_FAILED,               /* Memory allocation failed */
    ERROR_NOTE_WITH_SPACE,       /* A comment starting with ';', must appear only at the beginning of the line */
    LINE_LENGTH_EXCEED_MAXIMUM   /* Line length too long */
} ERRORS;

/* Possible errors for the first pass */
typedef enum FIRST_PASS_ERRORS
{
    ERR_AM_FILE = 11,               /* Failed to open .am file */
    ERR_NOT_COMMAND_OR_DIRECTIVE,   /* Something that is not a command or directive was entered after the label */
    ERR_FIRST_PASS,                 /* The pass finished with errors */
    ERR_AMOUNT_OF_ERRORS,           /* The amount of errors that been detected */
    ERR_LABEL_INVALID,              /* Line doesn't have a label, and needs one */
    ERR_UNKNOWN_DIRECTIVE,          /* The directive is unknown */
    ERR_NOT_A_NUMBER,               /* The argument is not numeric */
    ERR_NO_QUOTATION_MARKS,         /* There are no quotation marks */
    ERR_LABEL_IS_NOT_ALPHANUMERIC,  /* The label is not alphanumeric */
    ERR_LABEL_ENDING,               /* The label ends with something other than ':' */
    ERR_LABEL_RESERVED,             /* The assembler has a reserved word with the name of the label */
    ERR_LABEL_START,                /* The label doesn't start with a label */
    ERR_EXTERNAL_LABEL_EXISTS,      /* An external label with the same name exists */
    ERR_LABEL_EXISTS,               /* A label with the same name exists */
    ERR_INVALID_MATRIX,             /* The matrix is invalid */
    ERR_INVALID_MAT_ARGUMENT,       /* The given argument is invalid */
    ERR_MAT_WRONG_AMOUNT_OF_VALUES, /* The matrix has too many values */
    ERR_CLOSING_QUOTATION_MARK,     /* Missing a closing quotation mark on string */
    ERR_INVALID_OP,                 /* An invalid operator was entered */
    ERR_INVALID_SRC_OP,             /* An invalid source operator was entered */
    ERR_INVALID_DEST_OP,            /* An invalid destination operator was entered */
    ERR_SHOULD_NOT_HAVE_OP,         /* The command shouldn't have operands */
    ERR_MISSING_OPERAND,            /* An operand is missing */
    ERR_TOO_MANY_OPERANDS,          /* The command has too many operands */
    ERR_SHOULD_HAVE_TWO_OP,         /* The command should have exactly two operands */
    ERR_OPCODE,                     /* The command has the wrong opcode */
    ERR_MAX_MEMORY,                 /* IC + DC > 255 (Max Memory) */
    ERR_MISSING_BRACKET,            /* There's a missing bracket */
    ERR_DOUBLE_COMMA                /* There're double commas */
} FIRST_PASS_ERRORS;

/*************************************************************************/

/* Linked list node for storing macro content lines */
typedef struct macro_content
{
    char content_line[MAX_LINE_LENGTH]; /* One line of macro content */
    struct macro_content *next;         /* Pointer to next content line */
} macro_content;

/* Macro struct: macro name, linked list of content lines, pointer to next macro */
typedef struct macro
{
    char macro_name[MAX_LINE_LENGTH]; /* Macro name */
    struct macro_content *content;    /* List of content lines */
    struct macro *next;               /* Next macro in list */
} macro;

/* External usage struct: list of addresses where external label is used */
typedef struct external_usage
{
    int address;                 /* Usage address */
    struct external_usage *next; /* Next usage in list */
} external_usage;

/* External label struct: label name, usage addresses list, pointer to next external label */
typedef struct external_label
{
    char label[MAX_LABEL_LENGTH];      /* External label name */
    struct external_usage *usage_list; /* List of usage addresses */
    struct external_label *next;       /* Next external label in list */
} external_label;

/* Label struct: name, address, type, pointer to next label */
typedef struct label
{
    char name[MAX_LABEL_LENGTH]; /* Label name */
    int address;                 /* Label address */
    int type;                    /* Label type (code, data, etc.) */
    struct label *next;          /* Next label in list */
} label;

/* Code word struct: 10-bit value */
typedef struct code_word
{
    unsigned short value; /* The word will use only 10 bits */
} code_word;

/* Command parts struct: opcode, source/dest addressing, ARE */
typedef struct command_parts
{
    int opcode;      /* Opcode */
    int source_addr; /* Source addressing mode */
    int dest_addr;   /* Destination addressing mode */
    int ARE;         /* ARE field */
} command_parts;

/* Command struct: code word, address, referenced label, pointer to next command */
typedef struct command
{
    code_word word;                          /* Code word */
    int address;                             /* Command address */
    char referenced_label[MAX_LABEL_LENGTH]; /* Referenced label name */
    struct command *next;                    /* Next command in list */
} command;

/* Data struct: code word, address, pointer to next data */
typedef struct data
{
    code_word word;    /* Data word */
    int address;       /* Data address */
    struct data *next; /* Next data element */
} data;

/* Main assembler table struct holding all sections and metadata */
typedef struct assembler_table
{
    struct data *data_section;                  /* Pointer to data section list */
    struct command *code_section;               /* Pointer to code section list */
    struct label *label_list;                   /* Pointer to label list */
    struct external_label *external_list;       /* Pointer to external labels list */
    struct macro *macro_list;                   /* Pointer to macros list */
    char source_file[MAX_LABEL_LENGTH];         /* Source file name */
    char macro_expanded_file[MAX_LABEL_LENGTH]; /* Macro expanded file name */
    char assembly_file[MAX_LABEL_LENGTH];       /* Output assembly file name */
    int instruction_counter;                    /* Instruction counter */
    int data_counter;                           /* Data counter */
} assembler_table;

/**
 * Runs the full assembly process on a given file.
 * Performs preprocessing, first pass, second pass, and generates output files.
 * Frees all allocated memory and reports errors if encountered during any stage.
 *
 * @param filename The name of the input file (without extension).
 * @return true if the file was processed successfully, false if an error occurred.
 */
bool process_file(char *filename);

/**
 * Extracts a token from the line up to the given delimiter.
 * Copies the token into dest and returns the position after the delimiter.
 * Returns 0 if the delimiter is not found.
 *
 * @param dest Buffer to store the extracted token.
 * @param line The input line to extract from.
 * @param delimeter Character that ends the token.
 * @return Position after the delimiter or 0 if not found.
 */
int extract_token(char dest[MAX_LINE_LENGTH], char *line, char delimeter);
/**
 * Removes spaces and tabs from the input line until a comment (';') is reached.
 * The cleaned line is copied into dest.
 *
 * @param dest Buffer to store the cleaned line.
 * @param line The input line to process.
 */
void remove_white_spaces(char dest[MAX_LINE_LENGTH], char *line);
/**
 * Allocates memory of the given size and ensures it succeeded.
 * @param size The number of bytes to allocate.
 * @return Pointer to allocated memory (void*).
 */
void *generic_malloc(long size);
/**
 * Opens a file with the given name and mode, exits on failure.
 * @param name Name of the file.
 * @param mode fopen mode (e.g., "r", "w").
 * @return Pointer to opened file (FILE*).
 */
FILE *safe_fopen(char *name, char *mode);
/**
 * Copies the file name to the destination and appends a suffix.
 * @param dest Destination buffer.
 * @param file_name The base file name.
 * @param ending The suffix to append.
 */
void add_suffix(char *dest, const char *file_name, const char *ending);

/**
 * Checks for invalid comment positioning in a line.
 * A comment (';') must appear only at the beginning of the line.
 *
 * @param line The input line to check.
 * @param line_counter The current line number (for error reporting).
 * @return false if the comment is in an invalid position, true otherwise.
 */


bool handle_notes_error(char line[], int line_counter);

/**
 * Performs the preprocessing stage on the input file.
 * Expands macros, handles errors, and creates the .am file.
 *
 * @param assembler Pointer to the assembler table.
 * @return true if preprocessing succeeded, false if an error occurred.
 */

bool pre_proc(assembler_table **assembler);
/**
 * Initializes file pointers for assembly and macro-expanded files.
 * Also prepares buffers for lines and macro names.
 *
 * @param assembler Pointer to the assembler table structure.
 * @param fp_as Pointer to assembly file pointer (to be opened).
 * @param fp_am Pointer to macro-expanded file pointer (to be opened).
 * @param line Buffer for reading lines.
 * @param macro_name Buffer for storing macro names.
 */
void files_initialize(assembler_table **assembler, FILE **fp_as, FILE **fp_am, char line[MAX_LINE_LENGTH], char macro_name[MAX_LINE_LENGTH]);
/**
 * Shifts a 16-bit word to the left by a given number of bits.
 * @param word The word to be shifted.
 * @param move Number of bits to shift left.
 * @return The result after shifting.
 */
unsigned short move_bits(unsigned short word, unsigned short move);

/**
 * Initialize an assembler_table structure with empty lists and counters.
 * Copies the source filename provided.
 * @param argv - source file name string.
 * @return Pointer to newly allocated assembler_table.
 */
assembler_table *initialize_assembler_table(char *argv);

/**
 * Add a new usage address to the external usage linked list.
 * Creates a new external_usage node for the given address.
 * @param usage_list - pointer to the pointer to the head of the list.
 * @param address - address where external label is used.
 * No return value.
 */
void add_to_external_usage(external_usage **usage_list, int address);
void print_macros(const macro *macro);
void print_assembler_table(const assembler_table *table);

/**
 * Executes the second pass of the assembler:
 * - Validates label consistency (ENTRY, EXTERN)
 * - Complements referenced labels in command list
 * - Reports unresolved labels
 *
 * @param assembler Pointer to the assembler table structure.
 * @return True if second pass completed successfully, false otherwise.
 */
bool second_pass(assembler_table **assembler);

/**
 * High-level function that generates all output files:
 * - .ob file (object)
 * - .ent file (entries)
 * - .ext file (externals)
 *
 * @param assembler Pointer to the assembler table.
 */
void translation_unit(assembler_table *assembler);





/**
 * Prints an error message based on an error code and optional line number.
 * @param error_code The type of error.
 * @param line_counter Line number for context (-1 if not relevant).
 */
void errors_table(ERRORS error_code, int line_counter);

/**
 * Prints an error message based on an error code and optional line number of the first pass.
 * @param error_code The type of error.
 * @param line_counter Line number for context (-1 if not relevant).
 */
void first_pass_errors(FIRST_PASS_ERRORS error_code, int line, int error_counter);

/**
 * Removes a file if the name is valid. Exits if removal fails.
 * @param filename The file to remove.
 */
void safe_remove(const char *filename);

int first_pass(const char *file, assembler_table *table);

/**
 * Frees a linked list of data nodes.
 *
 * @param head Pointer to the first data node to free.
 */
void free_data_section(data *head);
/**
 * Frees a linked list of command nodes.
 *
 * @param head Pointer to the first command node to free.
 */
void free_code_section(command *head);
/**
 * Frees a linked list of label nodes.
 *
 * @param head Pointer to the first label node to free.
 */
void free_label_list(label *head);
/**
 * Frees a linked list of external usage address nodes.
 *
 * @param head Pointer to the head of the external usage list.
 */
void free_external_usage_list(external_usage *head);
/**
 * Frees a linked list of external labels, including their usage address lists.
 *
 * @param head Pointer to the head of the external label list.
 */
void free_external_list(external_label *head);
/**
 * Frees a linked list of macro content lines.
 *
 * @param head Pointer to the head of the macro content list.
 */
void free_macro_content_list(macro_content *head);
/**
 * Frees a linked list of macro nodes, including their content lines.
 *
 * @param head Pointer to the head of the macro list.
 */
void free_macro_list(macro *head);
/**
 * Frees all memory used by the assembler table and its internal components.
 *
 * @param table Pointer to the assembler_table to free.
 *
 * Frees: data section, code section, label list, externals, macros, and finally the table itself.
 */
void free_assembler_table(assembler_table *table);


int is_reserved_word(const char *label); /* Checks if the label is a reserved word of the assembler */

#endif /* ASSEMBLER_H */
