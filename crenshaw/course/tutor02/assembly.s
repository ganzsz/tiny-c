section     .text
global      _start 

_start: 
; ============== THE GENERATED PROGRAM ==============
	MOV RDX, 5	 ; Move num to RDX
	PUSH RDX	
	MOV RDX, 2	 ; Move num to RDX
	POP RAX
	SUB RDX, RAX
	NEG RDX
	PUSH RDX	
	MOV RDX, 4	 ; Move num to RDX
	POP RAX
	ADD RDX, RAX
; ============== END GENERATED PROGRAM ==============

	; Print register RDX
	ADD RDX, 48	 ; convert to ascii
	MOV [output], RDX	 ; store in output
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