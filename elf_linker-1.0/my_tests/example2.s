
     .text
     .global main
main:
     MOV r5, #0xff
     LDR r6, [r5] 
     MOV r1, r5
     MOV r1, r6

     ADD r2, r1, r6
     SUB r3, r5, r2

condition: 
     cmp r3, r6
     bge else
if:
     mov r6, #xa0
     bal fi
else:
     ADD r6, r6, r3
fi: