	; START BLOCK 0
	a
	; DO
	<expression>
	MOV CX, R8	; CX is the register we compare
L0:
	DEC CX
	PUSH CX
	; START BLOCK 1
	a
	JMP L2	; Encountered a break
	c
	; START BLOCK 1
	POP CX
	JCXZ L1 	 ; Jump out of loop if CX=zero 
	JMP L0 	 ; Repaet do
L2:
	POP CX	 ; Only hit when we break out of the loop
L1:
	a
	; START BLOCK 0
	END
