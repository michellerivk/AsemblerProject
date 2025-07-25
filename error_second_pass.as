.entry MISSINGDEFINITION
.entry DEFINEEXTERN   
.extern DEFINEEXTERN

DEFINEEXTERN: .data 5
.entry VALID
VALID: mov r2,r1
mov r2 , DIDNTDEFINElabelAsOperand


jmp DIDNTDEFINElabelAsOperand

mov M1[r3][r3],r3

jmp M1[r3][r3]
 mov r2 , r1




