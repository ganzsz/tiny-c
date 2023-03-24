section     .text
global      _start 

_start: 
; ============== THE GENERATED PROGRAM ==============
	MOV EDX, 5	 ; Move num to EDX
	MOV EAX, EDX	
	MOV EDX, 2	 ; Move num to EDX
	SUB EDX, EAX
	NEG EDX
; ============== END GENERATED PROGRAM ==============

	; Print register EDX
	ADD EDX, 48	 ; convert to ascii
	MOV [output], EDX	 ; store in output
	MOV EDX,1	 ; length to print
	MOV ECX, output	 ; point to output
	MOV EBX,1
	MOV EAX,4
	INT 0x80
	MOV EAX,1
	INT 0x80

	; Stop gracefully
	mov rax, 60       ; exit(
	mov rdi, 0        ;   EXIT_SUCCESS
	syscall           ; );
section     .data
output     db  0,0xa