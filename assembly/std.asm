
%include assembly/callfn.asm

saveregs:
	pop h
	push a
	push b
	push c
	push d
	push e
	push f
	push g
	push h
	ret

restoreregs:
	pop a
	pop g
	pop f
	pop e
	pop d
	pop c
	pop b
	push a
	swap
	pop a
	ret


mult:
	pop g

	pop a
	pop b

	mov c,0
	mov d,0

	multloop0:
		cmpeq d,b
		jmp [multend0]
		add c,a,c
		dec b
		jmp [multloop0]

multend0:
	push g

	mov h,c
	ret

square:
	pop g

	pop a

	CALLFN(mult,a,a)

	push g
	ret

div:
	pop g

	pop b
	pop a

	mov c,a
	mov d,0

	divloop0:
		sub a,b,a
		inc d

		cmpgt a,c
		jmp [divend0]

		mov c,a
		jmp [divloop0]

	divend0:
		sub d,1,h

	push g
	ret


mod:
	pop g

	pop b
	pop a

	mov c,a
	mov d,0

	divloop0:
		sub a,b,a
		inc d

		cmpgt a,c
		jmp [divend0]

		mov c,a
		jmp [divloop0]

	divend0:
		mov h,c

	push g
	ret