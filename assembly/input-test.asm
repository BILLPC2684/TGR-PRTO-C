loop0:
	drecv a,0x01,0x00
	disp a
	jmp [loop0]
