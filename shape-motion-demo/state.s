	.file "stateMachine.c"
	.text
	.globl buzzerState
buzzerState:
	cmp.b #0, r12
	jz iszero
	cmp.b #1, r12
	jz isone
	cmp.b #2, r12
	jz two
	jmp exit
iszero:
	call buzzer_set_period
	ret
isone:
	
	
