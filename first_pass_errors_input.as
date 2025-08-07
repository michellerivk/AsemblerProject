; Not a command or directive after label
LABEL1: some_invalid_text
; Unknown directive
.nonsense_directive 123 
; "nine" is not a number
.data 5, nine, 3
; Missing quotation marks after .string
.string Hello World 
; Label contains non-alphanumeric character
INVALID@LABEL: mov  r1,  r2 
; Label does not end with a ':'
MissingColon mov  r3,  r4 
; Label is a reserved assembler name
mov: mov  r1,  r2 
; Label starts with a digit
9InvalidStart: add  r2,  r3 
; External label re-declared
.extern SHARED_LABEL
.extern SHARED_LABEL
; Invalid matrix declaration syntax
.mat 4,5,6,7,8,9
; Invalid matrix argument
.mat [5a][2] 
; Not enough values for 2x2 matrix
.mat [2][2] 1,2,3
; Missing closing quotation mark
.string "Hello, world  
; 'fly' is not a valid opcode
fly  r2,  r3 
; Invalid source operand
mov 3Hello,  r3  
; Invalid destination operand
mov  r3, "badDest"  
; 'stop' shouldn't have operands
stop  r2 
; 'prn' missing operand
prn 
; 'inc' needs one operand
inc  r1,  r2  
; 'add' needs two operands
add  r3 

