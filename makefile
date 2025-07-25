# Target: assembler
assembler: pre_proc_errors.o assembler.o first_pass.o pre_proc.o structs.o functions.o translation_unit.o second_pass.o first_pass_functions.o first_pass_error_checks.o
	gcc -g -Wall -ansi -pedantic assembler.o first_pass.o pre_proc.o structs.o functions.o translation_unit.o second_pass.o first_pass_functions.o pre_proc_errors.o first_pass_error_checks.o  -o assembler

# Compile assembler.c
assembler.o: assembler.c assembler.h
	gcc -g -c -Wall -ansi -pedantic assembler.c -o assembler.o

# Compile first_pass.c
first_pass.o: first_pass.c assembler.h
	gcc -g -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

# Compile pre_proc.c
pre_proc.o: pre_proc.c assembler.h
	gcc -g -c -Wall -ansi -pedantic pre_proc.c -o pre_proc.o

# Compile structs.c
structs.o: structs.c assembler.h
	gcc -g -c -Wall -ansi -pedantic structs.c -o structs.o

# Compile functions.c
functions.o: functions.c assembler.h
	gcc -g -c -Wall -ansi -pedantic functions.c -o functions.o

# Compile translation_unit.c
translation_unit.o: translation_unit.c assembler.h
	gcc -g -c -Wall -ansi -pedantic translation_unit.c -o translation_unit.o

# Compile second_pass.c
second_pass.o: second_pass.c assembler.h
	gcc -g -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

# Compile first_pass_functions.c
first_pass_functions.o: first_pass_functions.c first_pass_functions.h assembler.h
	gcc -g -c -Wall -ansi -pedantic first_pass_functions.c -o first_pass_functions.o

# Compile first_pass_error_checks.c
first_pass_error_checks.o: first_pass_error_checks.c first_pass_error_checks.h assembler.h
	gcc -g -c -Wall -ansi -pedantic first_pass_error_checks.c -o first_pass_error_checks.o

# Compile pre_proc_errors.c
pre_proc_errors.o: pre_proc_errors.c assembler.h
	gcc -g -c -Wall -ansi -pedantic pre_proc_errors.c -o pre_proc_errors.o

# Clean object files and binary
clean:
	rm -f *.o assembler
