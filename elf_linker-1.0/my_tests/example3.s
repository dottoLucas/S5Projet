	.text
	.global main
main:
	MOV r5, #0xfe
	MOV r6, #0x01

while:
	cmp r5,r6
	ble end
	ADD r6, r6, r6
	b while
end:
