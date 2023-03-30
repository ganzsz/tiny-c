	a
	<condition>
	JE L0	 ; Jump Equal (false) to endif
	b
	JMP L1	 ; after this is else, so get out of if block 
L0:
	c
L1:
	d
	END
