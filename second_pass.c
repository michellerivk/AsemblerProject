#include "assembler.h"

/*
  Checks if any label is defined both as ENTRY and DATA, which is invalid.
  Returns false if such a conflict is found, true otherwise.
*/

bool data_entry_check(assembler_table **assembler) {
    bool error = true;
    label *label1 = (*assembler)->label_list; /* Label to go over the label list */

    /* Scan all labels looking for ENTRY labels */
    while (label1 != NULL) {
        if (label1->type == ENTRY) {
            label *label2 = (*assembler)->label_list;

            /* For each ENTRY label, scan again for a matching DATA label */
            while (label2 != NULL) {
                if (strcmp(label1->name, label2->name) == 0 && label2->type == DATA) {
                    /* label is both ENTRY and DATA */
                    printf("Error: Entry label '%s' can not be defined as Data type.\n", label1->name);
                    error = false;
                }
                label2 = label2->next;
            }
        }
        label1 = label1->next;
    }

    return error;
}



/*
  Goes over the label list and performs two validations:
   If a label is marked as ENTRY, it must also be defined elsewhere.
    Labels must not be defined more than once unless as ENTRY.

  Any violations will print an error message.
*/
bool labels_check(assembler_table **assembler) {
    label *ptr_label1 = (*assembler)->label_list; /* Used to iterate the label list */
    label *ptr_label2;                            /* Second pointer for comparisons */
    bool find_entry_definition, find_definition_twice, error = true;

    while (ptr_label1 != NULL) {

        /* Handle entry labels: look for real definitions */
        if (ptr_label1->type == ENTRY) {
            ptr_label2 = (*assembler)->label_list;
            find_entry_definition = false;

            /* Search for another label with the same name but not marked as ENTRY */
            while (ptr_label2 != NULL) {
                if (strcmp(ptr_label2->name, ptr_label1->name) == 0 &&
                 ptr_label2->type != ENTRY) {
                    ptr_label1->address = ptr_label2->address; /* Set resolved address */
                    find_entry_definition = true;
                    break;
                }
                ptr_label2 = ptr_label2->next;
            }

            if (!find_entry_definition) {
                printf("Error: Entry label '%s' is undefined.\n", ptr_label1->name);
                error = false;
            }
        }

        /* Check for duplicated non-entry labels */
        if (ptr_label1->type != ENTRY) {
            ptr_label2 = ptr_label1->next;
            find_definition_twice = false;

            while (ptr_label2 != NULL) {
                if (strcmp(ptr_label2->name, ptr_label1->name) == 0 &&
                 ptr_label2->type != ENTRY) {
                    find_definition_twice = true;
                    break;
                }
                ptr_label2 = ptr_label2->next;
            }

            if (find_definition_twice == true) {
                printf("Error: Label '%s' is already defined.\n", ptr_label1->name);
                error = false;
            }
        }

        ptr_label1 = ptr_label1->next;
    }

    return error;
}


/*
  Finds a label in the label list that matches the one referenced by the command.
  If found:
  If it's an external label, set the word to E (external).
  Otherwise, shift the address and mark it with R (relocatable).
*/
bool complement_label_word(assembler_table **assembler, command *ptr_cmd) {
    label *ptr_label = (*assembler)->label_list;

    while (ptr_label != NULL) {
        /* Compare label name to referenced label in command */
        if (strcmp(ptr_cmd->referenced_label, ptr_label->name) == 0) {

            /* Label is external, mark with ARE = E */
            if (ptr_label->type == EXTERNAL) {
                ptr_cmd->word.value = E;
            }
            /* Label is internal, store address and set ARE = R */
            else {
                ptr_cmd->word.value = move_bits(ptr_label->address, SHIFT_AFTER_ARE) | R;
            }

            return true;
        }

        ptr_label = ptr_label->next;
    }

    return false;
}


/*
  Similar to complement_label_word, but for external label list.
  If the label is found:
  Sets ARE = E in the command word.
  Adds the address where the external label is used to its usage list.
*/
bool complement_ext_word(assembler_table **assembler, command *ptr_cmd) {
    external_label *ptr_label_ex = (*assembler)->external_list;

    while (ptr_label_ex != NULL) {
        if (strcmp(ptr_cmd->referenced_label, ptr_label_ex->label) == 0) {
            ptr_cmd->word.value = E;

            /* Record the usage of this external label */
            add_to_external_usage(&(ptr_label_ex->usage_list), ptr_cmd->address);
            return true;
        }

        ptr_label_ex = ptr_label_ex->next;
    }

    return false;
}


/*
  Checks that no label is defined both as 'extern' and as a regular label.
  For every external label, searches the label list to ensure no duplicate exists.
*/
bool check_if_label_defined_as_extern(assembler_table **assembler) {
    external_label *ptr_label_ext = (*assembler)->external_list;
    label *ptr_label;
    bool error = true, duplicate_found;

    while (ptr_label_ext != NULL) {
        ptr_label = (*assembler)->label_list;
        duplicate_found = false;

        while (ptr_label != NULL) {
            if (strcmp(ptr_label->name, ptr_label_ext->label) == 0) {
                duplicate_found = true;
                break;
            }
            ptr_label = ptr_label->next;
        }

        if (duplicate_found) {
            printf("Error: Label '%s' is defined both as extern and regular.\n", ptr_label_ext->label);
            error = false;
        }

        ptr_label_ext = ptr_label_ext->next;
    }

    return error;
}


/*
  Executes the full second pass of the assembler:
   Verifies label consistency (entries and externs).
   Replaces referenced labels in command list with actual values.
   Reports undefined labels that were not resolved.

  Returns true if no errors were encountered.
*/
bool second_pass(assembler_table **assembler) {
    bool final_error = true;
    command *ptr_cmd = (*assembler)->code_section;

    /* First check all entry/duplicate issues */
    if (!labels_check(assembler)) {
        final_error = false;
    }

    /* Then check for duplicate extern/internal conflicts */
    if (!check_if_label_defined_as_extern(assembler)) {
        final_error = false;
    }
	
    /* Then check for entry label that define as data */
    if(!data_entry_check(assembler)){
        final_error = false;
    }

    /* Loop over all commands to resolve label references */
    while (ptr_cmd != NULL) {

        /* If the command references a label, try to resolve it */
        if (ptr_cmd->referenced_label[0] != '\0') {

            /* Try to find and fill internal label */
            if (!complement_label_word(assembler, ptr_cmd)) {

                /* If not found, try resolving as external */
                if (!complement_ext_word(assembler, ptr_cmd)) {
                    final_error = false;
                    printf("Error: Undefined label '%s'\n", ptr_cmd->referenced_label);
                }
            }
        }

        ptr_cmd = ptr_cmd->next;
    }

    return final_error;
}
