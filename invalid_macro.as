;
      ;
MAIN: mov M1[r2][r7],LENGTH 
add r2,STR 
LOOP: jmp END 
prn #-5 

mcro  a_mcXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend
XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx
           ;


mcro  1XXXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  XXX?XXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

;Not error delete whitespace
mcro  XXX      XXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  a_mcXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  stop
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  a                                                 
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend
                                                                                               
mcro  a_mc
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  mov
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend
mcro  r2
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  mcroend
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  prn
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  .mat
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  .entry
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend




mcro  jmp
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  mcro
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend


mcro  a_mcXXXXXXXXXXXX&XXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend

mcro  
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend 

sub r1, r4 
inc K 
 mcro  1hadar  
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend  
                      
mcro  &XXXX
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend  

mcro  hadar  
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend  

mcro  valid_macro  
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend   XXX


mcro  hadar 
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend  

mcro  a_mc 
    mov M1[r3][r3],r3 
    bne LOOP 
mcroend  

a_mc 


hadar
M1:  .mat  [2][2]  1,2,3,4 