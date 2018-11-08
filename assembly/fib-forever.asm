
mov a,1
mov b,0

floop0:
	add a,b,a
	disp a
	add b,a,b
	disp b
	jmp [floop0]
