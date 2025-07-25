; --- ERR_NOT_COMMAND_OR_DIRECTIVE ---
LABEL1: some_invalid_text ; Not a command or directive after label

; --- ERR_UNKNOWN_DIRECTIVE ---
        .nonsense_directive 123 ; Unknown directive

; --- ERR_NOT_A_NUMBER ---
        .data 5, nine, 3 ; "nine" is not a number

; --- ERR_NO_QUOTATION_MARKS ---
        .string Hello World ; Missing quotation marks after .string

; --- ERR_LABEL_IS_NOT_ALPHANUMERIC ---
INVALID@LABEL: mov @r1, @r2 ; Label contains non-alphanumeric character

; --- ERR_LABEL_ENDING ---
MissingColon mov @r3, @r4 ; Label does not end with a ':'

; --- ERR_LABEL_RESERVED ---
.data: mov @r1, @r2 ; Label is a reserved directive name

; --- ERR_LABEL_START ---
9InvalidStart: add @r2, @r3 ; Label starts with a digit

; --- ERR_EXTERNAL_LABEL_EXISTS ---
        .extern SHARED_LABEL
        .extern SHARED_LABEL ; External label re-declared

; --- ERR_LABEL_EXISTS ---
EXISTING: .data 1,2,3
EXISTING: .string "duplicate" ; Label redefined

; --- ERR_INVALID_MATRIX ---
        .mat 4,5,6,7,8,9 ; Invalid matrix declaration syntax

; --- ERR_INVALID_MAT_ARGUMENT ---
        .mat [5a][2] ; Invalid matrix argument

; --- ERR_MAT_WRONG_AMOUNT_OF_VALUES ---
        .mat [2][2] 1,2,3 ; Not enough values for 2x2 matrix

; --- ERR_CLOSING_QUOTATION_MARK ---
        .string "Hello, world ; Missing closing quotation mark

; --- ERR_INVALID_OP ---
        fly @r2, @r3 ; 'fly' is not a valid opcode

; --- ERR_INVALID_SRC_OP ---
        mov 3Hello, @r3 ; Invalid source operand

; --- ERR_INVALID_DEST_OP ---
        mov @r3, "badDest" ; Invalid destination operand

; --- ERR_SHOULD_NOT_HAVE_OP ---
        stop @r2 ; 'stop' shouldn't have operands

; --- ERR_MISSING_OPERAND ---
        prn ; 'prn' missing operand

; --- ERR_TOO_MANY_OPERANDS ---
        inc @r1, @r2 ; 'inc' only accepts one operand

; --- ERR_SHOULD_HAVE_TWO_OP ---
        add @r3 ; 'add' needs two operands