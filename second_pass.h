#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include <stdio.h>
#include "assembler.h"  /* for assembler_table, command, label, etc. */


/* ======================= Functions of proccesing second pass========================================= */

/**
 * Iterates over all commands and resolves any referenced labels:
 * - First tries to find them in the internal symbol table
 * - Then tries to resolve as external (adds to .ext if needed)
 * - Reports errors for unresolved references
 *
 * @param assembler Pointer to the assembler table.
 * @return True if all labels resolved, false otherwise.
 */
bool resolve_label_references(assembler_table **assembler);

/**
 * Fills a command word with the address of the referenced internal label.
 * Sets ARE bits to relocatable.
 *
 * @param assembler Pointer to the assembler table.
 * @param ptr_cmd Pointer to the current command to update.
 * @return True if label was found and updated, false otherwise.
 */
bool complement_label_word(assembler_table **assembler, command *ptr_cmd);

/**
 * If the referenced label is external, sets the ARE bits to external,
 * and adds a usage entry to the external list.
 *
 * @param assembler Pointer to the assembler table.
 * @param ptr_cmd Pointer to the current command to update.
 * @return True if label was external and added, false otherwise.
 */
bool complement_ext_word(assembler_table **assembler, command *ptr_cmd);


/* ======================= Functions of error handling========================================= */

/**
 * Ensures that every ENTRY label has a corresponding real definition.
 * Updates the address of the ENTRY label to match the defined one.
 *
 * @param assembler Pointer to the assembler table.
 * @return True if all ENTRY labels were matched, false if any are undefined.
 */
bool validate_entry_labels(assembler_table **assembler);

/**
 * Ensures that each non-ENTRY label is defined only once.
 *
 * @param assembler Pointer to the assembler table.
 * @return True if no duplicates found, false otherwise.
 */
bool validate_duplicate_labels(assembler_table **assembler);

/**
 * Verifies both ENTRY references and duplicate label definitions.
 *
 * @param assembler Pointer to the assembler table.
 * @return True if all checks passed, false otherwise.
 */
bool labels_check(assembler_table **assembler);

/**
 * Validates that no label is defined both as EXTERNAL and internal.
 *
 * @param assembler Pointer to the assembler table.
 * @return True if no conflicts found, false otherwise.
 */
bool check_if_label_defined_as_extern(assembler_table **assembler);

#endif /* SECOND_PASS_H */
