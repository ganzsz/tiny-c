section     .text
global      _start 

_start: 
; ============== THE GENERATED PROGRAM ==============
	MOV R8,0        ;Found + or - so we start with 0 in case of negation
	PUSH R8	
	MOV R8, 1	 ; Move num to R8
	POP R9
	SUB R8, R9
	NEG R8
	PUSH R8	
	MOV R8, 3	 ; Move num to R8
	PUSH R8
	MOV R8, 2	 ; Move num to R8
	POP R9
	IMUL R8,R9
	PUSH R8
	MOV R8, 8	 ; Move num to R8
	PUSH R8
	MOV R8, 4	 ; Move num to R8
	POP RAX       ; Top half
	MOV RDX, 0    ; Bot half
	IDIV R8       ; Divide above by R8
	MOV R8, RAX   ; Store result in R8
	POP RAX       ; Top half
	MOV RDX, 0    ; Bot half
	IDIV R8       ; Divide above by R8
	MOV R8, RAX   ; Store result in R8
	POP R9
	ADD R8, R9
; ============== END GENERATED PROGRAM ==============

	; Print register R8
	ADD R8, 48	 ; convert to ascii
	MOV [output], R8	 ; store in output
	MOV RDX,1	 ; length to print
	MOV RCX, output	 ; point to output
	MOV RBX,1
	MOV RAX,4
	INT 0x80
	MOV RAX,1
	INT 0x80

	; Stop gracefully
	mov rax, 60       ; exit(
	mov rdi, 0        ;   EXIT_SUCCESS
	syscall           ; );
section     .data
output     db  0,0xa