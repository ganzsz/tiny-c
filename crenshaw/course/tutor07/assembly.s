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
	<Condition>
	; IF
	JE L0	 ; Jump Equal (false) to endif or else
	; START BLOCK 1
	MOV R8, 3	 ; Move num to R8
	MOV [a], R8	 ; Save result in register
	; END BLOCK 1
	; ELSE
	JMP L1	 ; after this is else, so get out of if block 
L0:
	; START BLOCK 2
	<Condition>
	; IF
	JE L2	 ; Jump Equal (false) to endif or else
	; START BLOCK 3
	MOV R8, [a]	 ; Retrieve var
	MOV [b], R8	 ; Save result in register
	; END BLOCK 3
	; ELSE
	JMP L3	 ; after this is else, so get out of if block 
L2:
	; START BLOCK 4
	MOV R8, 4	 ; Move num to R8
	MOV [b], R8	 ; Save result in register
	; END BLOCK 4
L3:
	; END BLOCK 2
L1:
	; END BLOCK 0
