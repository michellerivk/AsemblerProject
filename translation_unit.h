#ifndef TRANSLATION_UNIT_H
#define TRANSLATION_UNIT_H

#include <stdio.h>
#include "assembler.h"  /* for assembler_table, command, label, etc. */


/* ========== Functions of the translation unit ========== */


/**
 * Creates the .ob file with instructions and data (in base-4).
 *
 * @param assembler Pointer to the assembler table.
 */
void object_file(assembler_table *assembler);

/**
 * Creates the .ent file listing all ENTRY labels.
 *
 * @param assembler Pointer to the assembler table.
 */
void ent_file(assembler_table *assembler);

/**
 * Creates the .ext file listing all EXTERN label usages.
 *
 * @param assembler Pointer to the assembler table.
 */
void ext_file(assembler_table *assembler);


/* ========== Helper methods for open files ========== */

/**
 * Opens a .ob (object) file for writing.
 *
 * @param source_filename The original source filename.
 * @param file_object Output: full .ob filename.
 * @param fp_ob Output: pointer to opened file.
 * @return True if successful.
 */
bool open_object_file(const char *source_filename, char *file_object, FILE **fp_ob);

/**
 * Opens a .ent (entry) file for writing.
 *
 * @param source_filename The original source filename.
 * @param file_entry Output: full .ent filename.
 * @param fp_ent Output: pointer to opened file.
 * @return True if successful.
 */
bool open_entry_file(const char *source_filename, char *file_entry, FILE **fp_ent);

/**
 * Opens a .ext (extern) file for writing.
 *
 * @param source_filename The original source filename.
 * @param file_external Output: full .ext filename.
 * @param fp_ext Output: pointer to opened file.
 * @return True if successful.
 */
bool open_extern_file(const char *source_filename, char *file_external, FILE **fp_ext);


/* ========== Write section functions ========== */

/**
 * Writes the command section to the .ob file.
 *
 * @param code_section Pointer to the command list.
 * @param fp_ob File pointer to the .ob file.
 */
void write_code_section(command *code_section, FILE *fp_ob);

/**
 * Writes the data section to the .ob file.
 * Offsets addresses by instruction counter.
 *
 * @param data_section Pointer to the data list.
 * @param ic The instruction counter.
 * @param fp_ob File pointer to the .ob file.
 */
void write_data_section(data *data_section, int ic, FILE *fp_ob);

/**
 * Writes external usages to the .ext file.
 *
 * @param ext_list List of external labels.
 * @param fp_ext File pointer to the .ext file.
 * @return True if any were written.
 */
bool write_extern_usages(external_label *ext_list, FILE *fp_ext);

/**
 * Writes ENTRY labels to the .ent file.
 *
 * @param label_list List of labels.
 * @param fp_ent File pointer to the .ent file.
 * @return True if any were written.
 */
bool write_entry_labels(label *label_list, FILE *fp_ent);


/* ========== Translate base 4 functions ========== */

/**
 * Converts a 10-bit number to 5-character base-4 string using 'a' to 'd'.
 *
 * @param number The integer to convert.
 * @param dest Output string (length 6 including \0).
 */
void trans_base_four(int number, char dest[]);

/**
 * Converts an integer (like IC or DC) to base-4 string using 'a' to 'd'.
 *
 * @param number The integer to convert.
 * @param dest Output string.
 */
void translate_ic_dc(int number, char dest[]);

#endif /* TRANSLATION_UNIT_H */
