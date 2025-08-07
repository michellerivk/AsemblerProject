; ERROR_INVALID_SRC_OP: Register r8 doesn’t exist
mov r8, r2
; ERROR_MISSING_OPERAND: prn needs one operand
prn
; ERROR_INVALID_OP: immediate must be a number
prn #-a
; ERROR_INVALID_DEST_OP: cannot write to an immediate
add r1, #5
; ERROR_INVALID_OP: malformed matrix (missing ‘]’)
mov LABEL[ r1, r2