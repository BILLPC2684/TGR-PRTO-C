mov a,1
mov b,0
mov c,39602

push 0

frevloop0:
	add a,b,a
	push a
	cmpgt a,c
	jmp [frevend0]
	add b,a,b
	push b
	cmpgt a,c
	jmp [frevend0]
	jmp [frevloop0]
frevend0:
	mov b,0

frevloop1:
	pop a
	cmpeq a,b
	jmp [frevend1]
	disp a
	jmp [frevloop1]
frevend1:
	hlt