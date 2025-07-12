#include "assembler.h"

/*
  Validates the given macro name according to several rules:
  Must not exceed MAX_LABEL_LENGTH
  Must not be empty
  Must not be already defined
  Must not be a reserved word
  Must start with a letter or underscore
  Must contain only letters, digits or underscores

  Returns false and reports an error to the errors table if any rule is violated.
*/
bool macro_name_examine(char macro_name[], int line_counter, macro *macro_list) {
    int i;

    /* Check for maximum allowed length */
    if (strlen(macro_name) > MAX_LABEL_LENGTH) {
        errors_table(EXCEED_MAXIMUM_MACRO_LENGTH, line_counter);
        return false;
    }

    /* Check for empty name */
    if (macro_name[0] == '\0') {
        errors_table(MISSING_MACRO_NAME, line_counter);
        return false;
    }

    /* Check if macro name already exists */
    if (find_macro(macro_list, macro_name) != NULL) {
        errors_table(MACRO_ALREADY_DEFINED, line_counter);
        return false;
    }

    /* Check if macro name is a reserved keyword */
    if (is_reserved_word(macro_name) == true) {
        errors_table(ERROR_RESERVED_WORD, line_counter);
        return false;
    }

    /* First character must be a letter or underscore */
    if (!islower(macro_name[0]) && !isupper(macro_name[0]) && macro_name[0] != '_') {
        errors_table(ERROR_INVALID_MACRO_NAME, line_counter);
        return false;
    }

    /* Loop through the rest of the macro name to check for invalid characters */
    for (i = 1; i < strlen(macro_name); i++) {
        /* Only allow letters, digits, or underscore */
        if (!islower(macro_name[i]) && !isupper(macro_name[i]) && !isdigit(macro_name[i]) && macro_name[i] != '_') {
            errors_table(ERROR_INVALID_MACRO_NAME, line_counter);
            return false;
        }
    }

    return true;
}


/*
  Checks if there is any text after the keyword "mcroend".
  If line is not just a newline character, this means extra text exists → error.

  Returns false and reports the error if found, true otherwise.
*/
bool examine_macroend(char line[], int line_counter) {
    /* If line is not empty, text after mcroend was found */
    if (line[0] != '\n') {
        errors_table(ERROR_TEXT_AFTER_MACROEND, line_counter);
        return false;
    }

    return true;
}
