; Label cant start with a digit
1start: mov r1, r2

; Illegal character in label
bad$label: add r3, r4
; Label cant be a reserved word of the asssembler
mov: add r3, r4
; Label cant be a reserved word of the asssembler
stop: add r3, r4