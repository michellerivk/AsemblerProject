; Register r8 doesn’t exist
mov r8, r2
; prn needs one operand
prn
; wrong operand
prn #-a
; cannot write to an immediate
add r1, #5
; missing ']'
mov LABEL[ r1, r2
; missing '['
mov LABEL r1, r2]
