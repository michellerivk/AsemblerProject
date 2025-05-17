#include "assembler.h"

/* First pass functions declarations */
void check_line(char *line, int line_number, assembler_table *table, int *error_count, bool label_flag);

char *get_label(char *line, int i);

int is_command_ok(char *word);

void add_label_to_table(assembler_table *table, char *lbl, int type, int *error_count);

int check_for_label(label *list, char *label);

int is_number_ok(char *input);

int get_instruction(char *com);

/* Error check declarations */
bool check_one_operands(char *line, int i, int *error_count, char *name, char *operand);
bool check_two_operands(char *line, int i, int *error_count, char *name);
bool check_zero_operands(char *line, int i, int *error_count);
bool check_command(char *line, int i, int *error_count, int operands_amount, char *command, bool label_flag);
bool is_matrix(char *operand);
bool is_immediate(char *operand);
bool is_register(char *operand);
bool is_label(char *operand);
