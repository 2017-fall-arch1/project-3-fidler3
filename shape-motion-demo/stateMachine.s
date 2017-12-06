	.file	"stateMachine.c"
.text
	.balign 2
	.global	buzzerState
	.type	buzzerState @function
buzzerState:
	SUB	#2, R1 		; create stack
	MOV.B	R12, 1(R1)	;move referenced char into stack
	CMP.B	#0, 1(R1)
	JNE	sopt 		;cmp char with 0 0-char jump if not equal
	MOV.B	#0, R12		    ;mov zero r12
	CALL	#buzzer_set_period  ;call function
	JMP	done		    ;jump to .L1
sopt:
	CMP.B	#1, 1(R1)
	JNE	topt 		    ;1-char jump if not equal to L4
	MOV	#5000, R12	    ;move 5000 into r12
	CALL	#buzzer_set_period  ;call functiom
	JMP 	done		    ;jmp to refill stack
topt:
	CMP.B	#2, 1(R1)
	JNE	done 		    ;2-char jump if not equal
	MOV	#1000, R12	    ;mov 1000 into R12
	CALL	#buzzer_set_period  ;call function
done:
	ADD	#2, R1		;refill stack
	RET
	.size	buzzerState, .-buzzerState
	
