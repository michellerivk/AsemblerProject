.entry MISSINGDEFINITION
.entry UNDEF
.entry GOODLABEL
GOODLABEL: .data 5
.entry DEFINEEXTERN   
.extern VALIID2
.extern DEFINEEXTERN
DEFINEEXTERN: .data 5
.entry VALID
VALID: mov r2,r1
mov r2 , DIDNTDEFINElabelAsOperand
add DIDNTDEFINElabelAsOperand , r2
jmp DIDNTDEFINElabelAsOperand
prn DIDNTDEFINElabelAsOperand
inc DIDNTDEFINElabelAsOperand
dec DIDNTDEFINElabelAsOperand
jsr DIDNTDEFINElabelAsOperand
not DIDNTDEFINElabelAsOperand
add r2 , DIDNTDEFINElabelAsOperand
sub VALID , DIDNTDEFINElabelAsOperand
add r3 , M55[r3][r2]
mov M1[r3][r3],r3
sub M1[r3][r3],r3
sub M2[r3][r3],r3
sub VALIID2 , r2
add r3 , VALIID2
add VALIID2 , r3
M2:mov r2 ,r1
jmp M1[r3][r3]
 mov r2 , r1
labeltwice:mov r2, r1
labeltwice:mov r2, r1




