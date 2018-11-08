
%include assembly/callfn.asm

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

; :
; 	pop g



; 	push g
; 	ret
