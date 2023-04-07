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
	<Relation>
	PUSH R8	
	MOV R8, 0	 ; False literal found
	; &
	PUSH R8
	MOV R8, -1	 ; True literal found
	POP R9
	AND R8, R9
	; |
	POP R9	 ; Get other b-term from stack
	OR R8, R9	 ; Perform or operation
	PUSH R8	
	MOV R8, -1	 ; True literal found
	; &
	PUSH R8
	MOV R8, -1	 ; True literal found
	POP R9
	AND R8, R9
	; |
	POP R9	 ; Get other b-term from stack
	XOR R8, R9	 ; Perform or operation
	PUSH R8	
	MOV R8, 0	 ; False literal found
	; |
	POP R9	 ; Get other b-term from stack
	XOR R8, R9	 ; Perform or operation
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
