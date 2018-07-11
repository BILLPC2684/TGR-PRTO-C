
mov a,1
mov b,0
mov c,39602

floop0:
	add a,b,a
	disp a
	cmpgt a,c
	jmp [fend0]
	add b,a,b
	disp b
	cmpgt a,c
	jmp [fend0]
	jmp [floop0]
fend0:
	hlt