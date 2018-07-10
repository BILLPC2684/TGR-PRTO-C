


mov a,1
mov b,0
mov c,39602


loop0:
	add a,b,a
	disp a
	cmpgt a,c
	jmp [end0]
	add b,a,b
	disp b
	cmpgt a,c
	jmp [end0]
	jmp [loop0]
end0:
	hlt