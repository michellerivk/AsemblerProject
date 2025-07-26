; line 2 - Not a command or directive after label
LABEL1: some_invalid_text

; line 4 - Unknown directive
.nonsense_directive 123 

; line 6 - "nine" is not a number
.data 5, nine, 3

; line 8 - Missing quotation marks after .string
.string Hello World 

; line 10 - Label contains non-alphanumeric character
INVALID LABEL: mov  r1,  r2 ; 

; line 12 - Label does not end with a ':'
MissingColon mov  r3,  r4 

; line 14 - Label is a reserved assembler name
mov: mov  r1,  r2 

; line 16 - Label starts with a digit
9InvalidStart: add  r2,  r3 

; line 19 - External label re-declared
.extern SHARED_LABEL
.extern SHARED_LABEL

; line 21 - Invalid matrix declaration syntax
.mat 4,5,6,7,8,9

; line 23 - Invalid matrix argument
.mat [5a][2] ; 

; line 25 - Not enough values for 2x2 matrix
.mat [2][2] 1,2,3

; line 27 - Missing closing quotation mark
.string "Hello, world  

; line 29 - 'fly' is not a valid opcode
fly  r2,  r3 

; line 31 - Invalid source operand
mov 3Hello,  r3  

; line 33 - Invalid destination operand
mov  r3, "badDest"  

; line 35 - 'stop' shouldn't have operands
stop  r2 

; line 37 - 'prn' missing operand
prn 

; line 39 - 
inc  r1,  r2  

; line 41 - 'add' needs two operands
add  r3 