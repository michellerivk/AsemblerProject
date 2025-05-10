# Target: assembler
assembler: assembler.o first_pass.o pre_proc.o structs.o functions.o translation_unit.o second_pass.o help_functions.o
	gcc -g -Wall -ansi -pedantic assembler.o first_pass.o pre_proc.o structs.o functions.o translation_unit.o second_pass.o help_functions.o -o assembler

# Compile assembler.c into assembler.o
assembler.o: assembler.c assembler.h
	gcc -g -c -Wall -ansi -pedantic assembler.c -o assembler.o

# Compile first_pass.c into first_pass.o
first_pass.o: first_pass.c assembler.h
	gcc -g -c -Wall -ansi -pedantic first_pass.c -o first_pass.o

# Compile pre_proc.c into pre_proc.o
pre_proc.o: pre_proc.c assembler.h
	gcc -g -c -Wall -ansi -pedantic pre_proc.c -o pre_proc.o

# Compile structs.c into structs.o
structs.o: structs.c assembler.h
	gcc -g -c -Wall -ansi -pedantic structs.c -o structs.o

# Compile functions.c into functions.o
functions.o: functions.c assembler.h
	gcc -g -c -Wall -ansi -pedantic functions.c -o functions.o

# Compile translation_unit.c into translation_unit.o
translation_unit.o: translation_unit.c assembler.h
	gcc -g -c -Wall -ansi -pedantic translation_unit.c -o translation_unit.o

# Compile second_pass.c into second_pass.o
second_pass.o: second_pass.c assembler.h
	gcc -g -c -Wall -ansi -pedantic second_pass.c -o second_pass.o

########## Compile functions for first pass #########
help_functions.o: help_functions.c help_functions.h assembler.h
	gcc -g -c -Wall -ansi -pedantic help_functions.c -o help_functions.o

# Clean all .o and the executable
clean:
	rm -f *.o assembler
