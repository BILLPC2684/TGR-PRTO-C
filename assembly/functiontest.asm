


jmp [functiontest_program_start]
#include assembly/std.asm
functiontest_program_start:



call [saveregs]
push 5
call [square]
call [restoreregs]

mov a,h
disp a

hlt



	


