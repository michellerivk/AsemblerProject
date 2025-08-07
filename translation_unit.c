#include "translation_unit.h"

/*
  Converts an integer to a base-4 representation using characters 'a' to 'd',
  and stores the result as a 5-character string in 'dest'.

  This encoding is used to represent binary values compactly in output files.
*/
void trans_base_four(int number, char dest[])
{
  int i;             /* Bit index: used to iterate through the bits from left to right */
  int curr_two_bits; /* Holds the current 2-bit chunk extracted from 'number' */
  int j;             /* Index for writing characters into 'dest' */

  /*
    The number is assumed to be up to 10 bits long (since 2 bits * 5 = 10).
    We extract 2 bits at a time, starting from the most significant bits (bit 8).
  */
  for (i = 8, j = 0; i >= 0; i -= 2, j++)
  {
    curr_two_bits = number >> i;                       /* Shift desired bits to the right */
    curr_two_bits = curr_two_bits & MASK_TWO_BITS;     /* Keep only the last two bits */
    dest[j] = (char)(curr_two_bits + UP_TO_CHARACTER); /* Map 0–3 to 'a'–'d' */
  }

  dest[5] = '\0'; /* Null-terminate the string */
}

/*
 * Converts a number(IC or DC) to base-4 string with a-d chars.
 */
void translate_ic_dc(int number, char dest[])
{
  char temp_rvs[MAX_LABEL_LENGTH] = {0}; /* temporary string to hold reversed result */
  int curr_two_bits;
  int j = 0;


  if (number == 0) /* handle edge case number is 0*/
  {
    dest[0] = 'a';
    dest[1] = '\0';
    return;
  }

  /* turn the number into base-4, one digit at a time */
  while (number > 0)
  {
    curr_two_bits = number & MASK_TWO_BITS;                /* get the last 2 bits */
    temp_rvs[j] = (char)(curr_two_bits + UP_TO_CHARACTER); /* turn into letter a-d */
    number = number / 4;                                   /* move to next 2 bits */
    j++;
  }

  temp_rvs[j] = '\0'; /* end the string */

  /* reverse the string to get the correct order */
  for (j = 0; j < strlen(temp_rvs); j++)
  {
    dest[strlen(temp_rvs) - 1 - j] = temp_rvs[j];
  }
}
/*
 * Generates the .ob file with machine code and data,
 * all in base-4 format using 'a' to 'd'.
 */
void object_file(assembler_table *assembler)
{
  char file_object[MAX_LABEL_LENGTH] = {0};
  char ic_dest[MAX_LABEL_LENGTH] = {0};
  char dc_dest[MAX_LABEL_LENGTH] = {0};
  FILE *fp_ob = NULL;

  /* Open the .ob file for writing */
  if (!open_object_file(assembler->source_file, file_object, &fp_ob))
  {
    return;
  }

  /* Convert IC and DC to base-4 format */
  translate_ic_dc(assembler->instruction_counter - 100, ic_dest);
  translate_ic_dc(assembler->data_counter, dc_dest);

  puts("\n===================trans unite==============\n");

  /* Write IC and DC values to the first line */
  fprintf(fp_ob, "\t%s\t%s\n", ic_dest, dc_dest);

  /* Write machine code and data sections */
  write_code_section(assembler->code_section, fp_ob);
  write_data_section(assembler->data_section, assembler->instruction_counter, fp_ob);

  /* Close the file */
  fclose(fp_ob);
}

/*
 * Generates the full filename with ".ob" suffix and opens it for writing.
 * Returns true if successful.
 */
bool open_object_file(const char *source_filename, char *file_object, FILE **fp_ob)
{
  /* Add .ob suffix to the filename */
  add_suffix(file_object, source_filename, ".ob");

  /* Try to open the file for writing */
  *fp_ob = fopen(file_object, "w");

  /* Return success status */
  return (*fp_ob != NULL);
}

/*
 * Writes the machine code section to the .ob file.
 * Each line includes the address and encoded word, both in base-4.
 */
void write_code_section(command *code_section, FILE *fp_ob)
{
  command *cmd = code_section;
  char dest_addr[MAX_LABEL_LENGTH], dest_word[MAX_LABEL_LENGTH];
  int i;

  while (cmd != NULL)
  {
    /* Convert address and word to base-4 strings */
    trans_base_four(cmd->address, dest_addr);
    trans_base_four(cmd->word.value, dest_word);

    /* Shift address string left (remove leading digit) */
    for (i = 0; i < strlen(dest_addr) - 1; i++)
      dest_addr[i] = dest_addr[i + 1];
    dest_addr[i] = '\0';

    /* Write address and word to file */
    fprintf(fp_ob, "%s\t%s\n", dest_addr, dest_word);

    cmd = cmd->next;
  }
}

/*
 * Writes the data section to the .ob file.
 * Addresses are offset by the instruction counter.
 */
void write_data_section(data *data_section, int ic, FILE *fp_ob)
{
  data *d = data_section;
  char dest_addr[MAX_LABEL_LENGTH], dest_word[MAX_LABEL_LENGTH];
  int i;

  while (d != NULL)
  {
    /* Convert address + IC to base-4 string */
    trans_base_four(d->address + ic, dest_addr);

    /* Convert data word to base-4 string */
    trans_base_four(d->word.value, dest_word);

    /* Shift address string left (remove leading digit) */
    for (i = 0; i < strlen(dest_addr) - 1; i++)
      dest_addr[i] = dest_addr[i + 1];
    dest_addr[i] = '\0';

    /* Write address and data to the file */
    fprintf(fp_ob, "%s\t%s\n", dest_addr, dest_word);

    d = d->next;
  }
}

/*
 * Writes all recorded usages of external labels to the file.
 * Each line contains the label and the address in base-4.
 * Returns true if at least one usage was written.
 */
bool write_extern_usages(external_label *ext_list, FILE *fp_ext)
{
  external_label *ext = ext_list;
  external_usage *usage;
  char dest_extern_usage[MAX_LABEL_LENGTH];
  int i;
  bool written = false;

  while (ext != NULL)
  {
    usage = ext->usage_list;
    while (usage != NULL)
    {
      written = true;

      /* Convert address to base-4 string */
      trans_base_four(usage->address, dest_extern_usage);

      /* Shift string left to remove first digit */
      for (i = 0; i < strlen(dest_extern_usage) - 1; i++)
      {
        dest_extern_usage[i] = dest_extern_usage[i + 1];
      }
      dest_extern_usage[i] = '\0';

      /* Write label and address to file */
      fprintf(fp_ext, "%s\t%s\n", ext->label, dest_extern_usage);

      usage = usage->next;
    }

    ext = ext->next;
  }

  return written;
}
/*
 * Creates the filename with ".ext" and opens the extern file for writing.
 * Returns true if the file was opened successfully.
 */
bool open_extern_file(const char *source_filename, char *file_external, FILE **fp_ext)
{
    /* Add the .ext suffix to the filename */
    add_suffix(file_external, source_filename, ".ext");

    /* Try to open the file for writing */
    *fp_ext = fopen(file_external, "w");

    /* Return success status */
    return (*fp_ext != NULL);
}

/*
 * Generates the .ext file listing all usages of external labels.
 * Deletes the file if no usages were found.
 */
void ext_file(assembler_table *assembler)
{
  char file_external[MAX_LABEL_LENGTH] = {0};
  FILE *fp_ext = NULL;
  bool written;

  /* Try to open the .ext file for writing */
  if (!open_extern_file(assembler->source_file, file_external, &fp_ext))
  {
    return;
  }

  /* Write all external label usages to the file */
  written = write_extern_usages(assembler->external_list, fp_ext);

  /* Close the file */
  fclose(fp_ext);

  /* If nothing was written, delete the empty file */
  if (!written)
  {
    safe_remove(file_external);
  }
}

/*
 * Iterates over the label list and writes ENTRY labels to the given file.
 * Converts the address to base-4 format before writing.
 * Returns true if at least one label was written.
 */
bool write_entry_labels(label *label_list, FILE *fp_ent)
{
  label *temp = label_list;
  char dest_label_addr[MAX_LABEL_LENGTH];
  bool written = false;
  int i;

  while (temp != NULL)
  {
    if (temp->type == ENTRY)
    {
      written = true;

      /* Convert address to base-4 string */
      trans_base_four(temp->address, dest_label_addr);

      /* Shift digits one position left (removes leading a) */
      for (i = 0; i < strlen(dest_label_addr) - 1; i++)
      {
        dest_label_addr[i] = dest_label_addr[i + 1];
      }
      dest_label_addr[i] = '\0';

      /* Write label and address to the file */
      fprintf(fp_ent, "%s\t%s\n", temp->name, dest_label_addr);
    }
    temp = temp->next;
  }

  return written;
}

/*
 * Adds the ".ent" suffix to the source filename and opens the file for writing.
 * Returns true if file was opened successfully.
 */
bool open_entry_file(const char *source_filename, char *file_entry, FILE **fp_ent)
{
  /* Add .ent suffix to the filename */
  add_suffix(file_entry, source_filename, ".ent");

  /* Try to open the file for writing */
  *fp_ent = fopen(file_entry, "w");

  /* Return success status */
  return (*fp_ent != NULL);
}
/*
 * Generates the .ent file with ENTRY labels and their addresses.
 * Removes the file if no ENTRY labels exist.
 */
void ent_file(assembler_table *assembler)
{
  char file_entry[MAX_LABEL_LENGTH] = {0};
  FILE *fp_ent = NULL;
  bool written;

  /* Create .ent filename and open the file */
  if (!open_entry_file(assembler->source_file, file_entry, &fp_ent))
  {
    return; /* Failed to open file */
  }

  /* Write all ENTRY labels to the file */
  written = write_entry_labels(assembler->label_list, fp_ent);

  fclose(fp_ent);

  /* If no labels were written, delete the file */
  if (!written)
  {
    safe_remove(file_entry);
  }
}

/*
  High-level wrapper function that creates all three output files-
  Object file (.ob): machine code and data
  Entry file (.ent): all ENTRY labels
  Extern file (.ext): external label usages

*/
void translation_unit(assembler_table *assembler)
{
  object_file(assembler);  /* Write machine code and data to .ob */
  ent_file(assembler);     /* Write entry labels to .ent */
  ext_file(assembler);     /* Write external usages to .ext */
}