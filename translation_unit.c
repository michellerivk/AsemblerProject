#include "assembler.h"

/*
  Converts an integer to a base-4 representation using characters 'a' to 'd',
  and stores the result as a 5-character string in 'dest'.

  This encoding is used to represent binary values compactly in output files.
*/
void trans_base_four(int number, char dest[]) {
    int i;                  /* Bit index: used to iterate through the bits from left to right */
    int curr_two_bits;      /* Holds the current 2-bit chunk extracted from 'number' */
    int j;                  /* Index for writing characters into 'dest' */

    /*
      The number is assumed to be up to 10 bits long (since 2 bits * 5 = 10).
      We extract 2 bits at a time, starting from the most significant bits (bit 8).
    */
    for (i = 8, j = 0; i >= 0; i -= 2, j++) {
        curr_two_bits = number >> i;                  /* Shift desired bits to the right */
        curr_two_bits = curr_two_bits & MASK_TWO_BITS;/* Keep only the last two bits */
        dest[j] = (char)(curr_two_bits + UP_TO_CHARACTER); /* Map 0–3 to 'a'–'d' */
    }

    dest[5] = '\0'; /* Null-terminate the string */
}


/*
  Generates the object file (.ob), which contains:
  - A header line with instruction and data counts.
  - A section for machine code (commands).
  - A section for data (constants, strings).

  All values are written in base-4 format with characters 'a' to 'd'.
*/
void object_file(assembler_table *assembler) {
    char file_object[MAX_LABEL_LENGTH] = {0};    /* Holds the full filename with ".ob" suffix */
    char dest_addr[MAX_LABEL_LENGTH] = {0};      /* Holds the current address (in base-4) */
    char dest_word[MAX_LABEL_LENGTH] = {0};      /* Holds the current value (in base-4) */
    char ic_dest[MAX_LABEL_LENGTH] = {0};        /* Holds instruction count in base-4 */
    char dc_dest[MAX_LABEL_LENGTH] = {0};        /* Holds data count in base-4 */
    int i;                                       /* Variable for loop */
    command *temp_command = NULL;                /* Pointer for iterating code section */
    data *temp_data = NULL;                      /* Pointer for iterating data section */

    FILE *fp_ob = NULL;                          /* File pointer for output object file */

    /* Add ".ob" suffix to the source file name and open the file */
    add_suffix(file_object, assembler->source_file, ".ob");
    fp_ob = fopen(file_object, "w");

    /* Translate instruction and data count to base-4 and print header line */
    trans_base_four(assembler->instruction_counter - 100, ic_dest);
    trans_base_four(assembler->data_counter, dc_dest);
    puts("\n===================trans unite==============\n");
    for(i = 0; i < strlen(ic_dest) - 2; i++){ 
    
        ic_dest[i] = ic_dest[i+2];
        
    }
    ic_dest[i]= '\0';
    for(i = 0; i < strlen( dc_dest) - 3; i++){ 
    
        dc_dest[i] =  dc_dest[i+3];
        
    }
     dc_dest[i]= '\0';
    fprintf(fp_ob, "\t%s\t%s\n", ic_dest, dc_dest);

    /* Write the machine code section line by line */
    temp_command = assembler->code_section;
    while (temp_command != NULL) {
        /* Convert address and word value to base-4 */
        trans_base_four(temp_command->address, dest_addr);
        trans_base_four(temp_command->word.value, dest_word);

        for(i = 0; i < strlen(dest_addr) - 1; i++){ 
        dest_addr[i] = dest_addr[i+1];
    }
    dest_addr[i]= '\0';

        fprintf(fp_ob, "%s\t%s\n", dest_addr, dest_word);
        temp_command = temp_command->next;
    }

    /* Write the data section, addresses continue after the last instruction */
    temp_data = assembler->data_section;
    while (temp_data != NULL) {
        /* The address is offset by instruction count */
        trans_base_four(temp_data->address + assembler->instruction_counter, dest_addr);
        trans_base_four(temp_data->word.value, dest_word);
          for(i = 0; i < strlen(dest_addr) - 1; i++){ 
                dest_addr[i] = dest_addr[i+1];
        }
        dest_addr[i]= '\0';

        fprintf(fp_ob, "%s\t%s\n", dest_addr, dest_word);
        temp_data = temp_data->next;
    }

    fclose(fp_ob);
}


/*
  Generates the extern file (.ext) which lists all usages of external labels.
  Each line contains the label name and the address where it is used.

  If no usages exist, the file is deleted to avoid empty output.
*/
void ext_file(assembler_table *assembler) {
    char file_external[MAX_LABEL_LENGTH] = {0};      /* Filename for .ext file */
    char dest_extern_usage[MAX_LABEL_LENGTH] = {0};  /* Holds each usage address in base-4 */

    external_label *temp_external_label = NULL;      /* Iterates over external label list */
    external_usage *usage_ptr = NULL;                /* Iterates over usage list of a label */

    FILE *fp_ext = NULL;                             /* Output file pointer */
    bool file_empty = true;                          /* Used to check if file remains unused */
    int i;                                           /* Variable for loop */

    /* Create and open the .ext file */
    add_suffix(file_external, assembler->source_file, ".ext");
    fp_ext = fopen(file_external, "w");

    /* Loop through each external label */
    temp_external_label = assembler->external_list;
    while (temp_external_label != NULL) {
        usage_ptr = temp_external_label->usage_list;

        /* Loop through all usages of this label */
        while (usage_ptr != NULL) {
            file_empty = false; /* File will contain content, so keep it */
            trans_base_four(usage_ptr->address, dest_extern_usage);
              for(i = 0; i < strlen( dest_extern_usage) - 1; i++){ 
                   dest_extern_usage[i] =  dest_extern_usage[i+1];
                 }
              dest_extern_usage[i]= '\0';

            fprintf(fp_ext, "%s\t%s\n", temp_external_label->label, dest_extern_usage);
            usage_ptr = usage_ptr->next;
        }

        temp_external_label = temp_external_label->next;
    }

    /* If no extern usages were recorded, delete the file */
    if (file_empty) {
        safe_remove(file_external);
    }

    fclose(fp_ext);
}


/*
  Generates the entry file (.ent) listing all labels declared as ENTRY.
  Each line contains the label name and its resolved memory address.

  If there are no ENTRY labels, the file is removed to avoid unnecessary output.
*/
void ent_file(assembler_table *assembler) {
    char file_entry[MAX_LABEL_LENGTH] = {0};      /* Holds the filename with ".ent" */
    char dest_label_addr[MAX_LABEL_LENGTH] = {0}; /* Holds label address in base-4 */

    label *temp_label = NULL;                     /* Iterates through label list */
    FILE *fp_ent = NULL;                          /* File pointer for .ent */
    bool file_empty = true;                       /* Flag for whether any ENTRY was written */
    int i;                                        /* Variable for loop */

    /* Generate filename and open file */
    add_suffix(file_entry, assembler->source_file, ".ent");
    fp_ent = fopen(file_entry, "w");

    /* Iterate through all labels and write only ENTRY types */
    temp_label = assembler->label_list;
    while (temp_label != NULL) {
        if (temp_label->type == ENTRY) {
            file_empty = false;
            trans_base_four(temp_label->address, dest_label_addr);
             for(i = 0; i < strlen( dest_label_addr) - 1; i++){ 
                   dest_label_addr[i] =  dest_label_addr[i+1];
                 }
              dest_label_addr[i]= '\0';

            fprintf(fp_ent, "%s\t%s\n", temp_label->name, dest_label_addr);
        }
        temp_label = temp_label->next;
    }

    /* Delete the file if no ENTRY labels were written */
    if (file_empty) {
        safe_remove(file_entry);
    }

    fclose(fp_ent);
}


/*
  High-level wrapper function that creates all three output files:
  Object file (.ob): machine code and data
  Entry file (.ent): all ENTRY labels
  Extern file (.ext): external label usages

  This function should be called after second_pass.
*/
void translation_unit(assembler_table *assembler) {
    object_file(assembler);
    ent_file(assembler);
    ext_file(assembler);
}

