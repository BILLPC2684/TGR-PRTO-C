


jmp [functiontest_program_start]
%include assembly/std.asm
functiontest_program_start:



CALLFN(square,5)
mov a,h
disp a

hlt



	


