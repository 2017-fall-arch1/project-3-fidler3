	.file	"stateMachine.c"
.text
	.balign 2
	.global	buzzerState
	.type	buzzerState, @function
buzzerState:
; start of function
; framesize_regs:     0
; framesize_locals:   2
; framesize_outgoing: 0
; framesize:          2
; elim ap -> fp       2
; elim fp -> sp       2
; saved regs:(none)
	; start of prologue
	SUB.W	#2, R1 		; create stack
	; end of prologue
	MOV.B	R12, 1(R1)	;move referenced char into stack
	CMP.B	#0, 1(R1) { JNE	.L2 ;cmp char with 0 0-char jump if not equal
	MOV.B	#0, R12		    ;mov zero r12
	CALL	#buzzer_set_period  ;call function
	BR	#.L1		    ;jump to .L1
.L2:
	CMP.B	#1, 1(R1) { JNE	.L4 ;1-char jump if not equal to L4
	MOV.W	#5000, R12	    ;move 5000 into r12
	CALL	#buzzer_set_period  ;call functiom
	BR	#.L1		    ;jmp to refill stack
.L4:
	CMP.B	#2, 1(R1) { JNE	.L1 ;2-char jump if not equal
	MOV.W	#1000, R12	    ;mov 1000 into R12
	CALL	#buzzer_set_period  ;call function
	NOP
.L1:
	; start of epilogue
	ADD.W	#2, R1		;refill stack
	RET
	.size	buzzerState, .-buzzerState
	.ident	"GCC: (SOMNIUM Technologies Limited - msp430-gcc 6.2.1.16) 6.2.1 20161212"
