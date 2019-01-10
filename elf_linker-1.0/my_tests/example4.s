	.text
	.global main
main:

neg_image:  
    mov r0, #0x22
	sub sp,sp,#x4
	str r0,[sp]
    sub sp,sp,#x4
    str lr,[sp]                                                               				

	
	ldr r3, adr_bytes	
	add r8, r0, #0x6c					

	cond:	
		cmp r0, r8
		bge endfor
	for:
		ldrb r4,[r0]
		bl neg_octet
		add r0,r0, #x01
		bal cond
		
	endfor:
    ldr lr,[sp]
    add sp,sp,#x04
    ldr r0,[sp]s
    add sp,sp,#x04
	mov  pc,lr                                                           

neg_octet:                  
            mov r4, #0x2e                             
			mov r5, #0xff
			eor r7, r5, r4  	@r7 = negatif(r0)
			strb r7,[r0]
			
			mov pc,lr 

                                                           
fin: