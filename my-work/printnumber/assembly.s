section     .text
global      _start 
_start: 
		mov	edx, 2				; what to print
		add edx, 48				; convert to ascii
		mov [output], edx		; store in output
    mov     edx,1			; length to print   
    mov     ecx,output   ; point to register output
    mov     ebx,1   
    mov     eax,4   
    int     0x80   
    mov     eax,1  
    int     0x80   
section     .data
output     db  'a',0xa