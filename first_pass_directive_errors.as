; ERR_INVALID_OP: .data lists a word that’s not an integer
.data 5, two, 7
; ERR_INVALID_OP: .data lists a word that’s not an integer
.data two, 7
; ERR_INVALID_OP: .data lists a word that’s not an integer
.data two
; ERR_INVALID_OP: .string must be quoted
.string hello
; ERR_INVALID_OP: .string must be quoted
.string hello"
; ERR_INVALID_OP: .string must be quoted
.string "hello
