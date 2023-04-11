section     .text
global      _start

; Write ascii char from RAX to stdout
_printc:
	MOV [msg], RAX	 ; store in output
	MOV RAX, 1	 ; Write (
	MOV RDI, 1	 ;   To STDOUT
	MOV RSI, msg	 ; Content of msg
	MOV RDX, 1	 ;   Length 1
	syscall	 ; )
	ret

; Write '0x' to stdout
_printhexstart:
	MOV RAX, '0x';
	MOV [msg], RAX	; Store in msg
	MOV RAX, 1	 ; Write (
	MOV RDI, 1	 ;   To STDOUT
	MOV RSI, msg	 ; Content of msg
	MOV RDX, 2	 ;   Length 1
	syscall	 ; )
	ret

; Write a half byte (nibble) from RAX to stdout
_printhb:
	MOV RBX, RAX	; copy to do stuff with
	AND RAX, 0x0F	; Get high half
	CMP RAX, 0x09	; If higher then 9
	JA make_letter	; Make it a letter
	ADD RAX, '0'	; Else make number
	JMP created_char
make_letter:
	SUB RAX, 10		; Sub 10 to find right letter
	ADD RAX, 'A'	; Make letter
created_char:
	call _printc
	ret

; Write 16bit int as hex to stdout
_printi:
	PUSH RAX
	CALL _printhexstart ; leader
	POP RAX
	MOV RCX, 4		; We're going to do this 4 times
printiloop:
	ROL AX, 4		; Rotate left 4 bits to right 4 bits
	PUSH RAX		  ; Store for later use
	PUSH RCX			; Push all registers that we reuse
	CALL _printhb	; Print last 4 bits
	POP RCX
	POP RAX			  ; else Get stored
	DEC RCX				; Decrease RCX (counter)
	JZ printidone ; Done 4 times go to stop
	JMP printiloop; And start over
printidone:
	RET
_start: 
; ============== THE GENERATED PROGRAM ==============
	; START BLOCK 0
	MOV R8, 1	 ; Move num to R8
	PUSH R8
	MOV R8, 2	 ; Move num to R8
	PUSH R8
	MOV R8, 4	 ; Move num to R8
	; *
	POP R9
	IMUL R8,R9
	; +
	POP R9
	ADD R8, R9
	MOV [a], R8	 ; Save result in register
	; Switch
	MOV R8, [a]	 ; Retrieve var
	PUSH R8	 ; Push expression to test against
	; START BLOCK 1
	; END BLOCK 1
	; Case
	MOV R8, 1	 ; Move num to R8
	 <Compare R8, [RSP]
	JNE L1	 ; Go to next statement if not equal
	; START BLOCK 2
	MOV R8, 2	 ; Move num to R8
	MOV [x], R8	 ; Save result in register
	; END BLOCK 2
L1:
	; Case
	MOV R8, 2	 ; Move num to R8
	 <Compare R8, [RSP]
	JNE L2	 ; Go to next statement if not equal
	; START BLOCK 3
	MOV R8, 3	 ; Move num to R8
	MOV [x], R8	 ; Save result in register
	; END BLOCK 3
L2:
L0:
	POP R8	 ; Clean up stack
	; END BLOCK 0
; ============== END GENERATED PROGRAM ==============

	; Print register R8
	MOV RAX, R8	 ; Print R8
	CALL _printi	; As int

	; Print \n
	MOV RAX, 10	 ; Print \n
	CALL _printc	; As char

	; Stop gracefully
	mov rax, 60       ; exit(
	mov rdi, 0        ;   EXIT_SUCCESS
	syscall           ; );
section     .data
	msg    db  0x00
	a      dw 0x0000
	x      dw 0x0000
