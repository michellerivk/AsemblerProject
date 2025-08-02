#include "second_pass.h"

/*
 * Ensures that each non-ENTRY label is defined only once.
 */
bool validate_duplicate_labels(assembler_table **assembler)
{
    label *current = (*assembler)->label_list;
    label *other;
    bool error = true;

    /* iterate over labels and compare each with the ones after it */
    while (current != NULL)
    {
        if (current->type != ENTRY)
        {
            other = current->next;

            /* check for duplicate (non-entry) label names */
            while (other != NULL)
            {
                if (strcmp(other->name, current->name) == 0 && other->type != ENTRY)
                {
                    printf("Error: Label '%s' is already defined.\n", current->name);
                    error = false;
                    break;
                }
                other = other->next;
            }
        }

        current = current->next;
    }

    return error;
}

/*
 * Ensures that every ENTRY label has a corresponding real definition.
 * Updates the address of the ENTRY label to match the defined one.
 */
bool validate_entry_labels(assembler_table **assembler)
{
    label *entry = (*assembler)->label_list;
    label *other;
    bool found, error = true;

    /* iterate over label list and look for ENTRY labels */
    while (entry != NULL)
    {
        if (entry->type == ENTRY)
        {
            found = false;
            other = (*assembler)->label_list;

            /* search for matching label with same name but real address */
            while (other != NULL)
            {
                if (strcmp(other->name, entry->name) == 0 && other->type != ENTRY)
                {
                    entry->address = other->address;
                    found = true;
                    break;
                }
                other = other->next;
            }

            /* if no match found, report error */
            if (!found)
            {
                printf("Error: Entry label '%s' is undefined.\n", entry->name);
                error = false;
            }
        }

        entry = entry->next;
    }

    return error;
}


/*
 * Validates label list
 * - ENTRY labels must be defined elsewhere.
 * - Non-ENTRY labels must not be defined more than once.
 */
bool labels_check(assembler_table **assembler)
{
    bool valid_entries, no_duplicates;

    /* check that all .entry labels point to existing labels */
    valid_entries = validate_entry_labels(assembler);

    /* check that no label is defined more than once */
    no_duplicates = validate_duplicate_labels(assembler);

    return valid_entries && no_duplicates;
}

/*
  Finds a label in the label list that matches the one referenced by the command.
  If found:
  If it's an external label, set the word to E (external).
  Otherwise, shift the address and mark it with R (relocatable).
*/
bool complement_label_word(assembler_table **assembler, command *ptr_cmd)
{
    label *ptr_label = (*assembler)->label_list;

    while (ptr_label != NULL)
    {
        /* Compare label name to referenced label in command */
        if (strcmp(ptr_cmd->referenced_label, ptr_label->name) == 0)
        {

            /* Label is external, mark with ARE = E */
            if (ptr_label->type == EXTERNAL)
            {
                ptr_cmd->word.value = E;
            }
            /* Label is internal, store address and set ARE = R */
            else
            {
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
bool complement_ext_word(assembler_table **assembler, command *ptr_cmd)
{
    external_label *ptr_label_ex = (*assembler)->external_list;

    /* search for the referenced label in the external label list */
    while (ptr_label_ex != NULL)
    {
        if (strcmp(ptr_cmd->referenced_label, ptr_label_ex->label) == 0)
        {
            /* set ARE field to 'E' for external label */
            ptr_cmd->word.value = E;

            /* record the address where this external label is used */
            add_to_external_usage(&(ptr_label_ex->usage_list), ptr_cmd->address);
            return true;
        }

        ptr_label_ex = ptr_label_ex->next;
    }

    /* label not found in external list */
    return false;
}


/*
  Checks that no label is defined both as 'extern' and as a regular label.
  For every external label, searches the label list to ensure no duplicate exists.
*/
bool check_if_label_defined_as_extern(assembler_table **assembler)
{
    external_label *ptr_label_ext = (*assembler)->external_list;
    label *ptr_label;
    bool error = true, duplicate_found;

    /* loop through all external labels */
    while (ptr_label_ext != NULL)
    {
        ptr_label = (*assembler)->label_list;
        duplicate_found = false;

        /* check if the extern label is also defined as regular */
        while (ptr_label != NULL)
        {
            if (strcmp(ptr_label->name, ptr_label_ext->label) == 0)
            {
                duplicate_found = true;
                break;
            }
            ptr_label = ptr_label->next;
        }

        /* report error if a label is defined both as extern and regular */
        if (duplicate_found)
        {
            printf("Error: Label '%s' is defined both as extern and regular.\n", ptr_label_ext->label);
            error = false;
        }

        ptr_label_ext = ptr_label_ext->next;
    }

    return error;
}


/**
 * Resolves label references in the code section by completing missing words.
 * Tries to match each referenced label to an internal or external definition.
 */

bool resolve_label_references(assembler_table **assembler)
{
    command *ptr_cmd = (*assembler)->code_section;
    bool success = true;

    /* go over each command and resolve its referenced label if exists */
    while (ptr_cmd != NULL)
    {
        if (ptr_cmd->referenced_label[0] != '\0')
        {
            /* try to complete internal label first */
            if (!complement_label_word(assembler, ptr_cmd))
            {
                /* if not found, try external label */
                if (!complement_ext_word(assembler, ptr_cmd))
                {
                    success = false;
                    printf("Error: Undefined label '%s'\n", ptr_cmd->referenced_label);
                }
            }
        }

        ptr_cmd = ptr_cmd->next;
    }

    return success;
}


/*
 * Executes the full second pass of the assembler:
 * Verifies label consistency, resolves label references,
 * and reports any undefined labels.
 */
bool second_pass(assembler_table **assembler)
{
    bool final_error = true;

    /* check that all labels are valid and not duplicated */
    if (!labels_check(assembler))
    {
        final_error = false;
    }

    /* check that labels marked as extern are not defined locally */
    if (!check_if_label_defined_as_extern(assembler))
    {
        final_error = false;
    }

    /* resolve label references in the code section */
    if (!resolve_label_references(assembler))
    {
        final_error = false;
    }

    return final_error;
}
