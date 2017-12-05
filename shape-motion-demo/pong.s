	.file	"pong.c"

	.global stateMachine

stateMachine:
	call p2sw_read
	mov r12, r4
	mov &layer1, r11
	mov &paddle1, r12
	call layerGetBounds
	mov r12, r5		;address of paddle1
	mov &layer2, r11
	mov &paddle2, r12
	call layerGetBounds
	mov r12, r6		;address of region paddle2
	mov r4, r5		;copy switches into r5
	AND #1, r5
	JZ lpaddleup
	mov r4, r5
	AND #2, r5
	JZ lpaddledown
	mov r4, r5
	AND #4, r5
	JZ rpaddleup
	mov r4, r5
	AND #8, r5
	JZ rpaddledown
lpaddleup:
	sub #4, r1
	mov layer1(0), 0(r1)
