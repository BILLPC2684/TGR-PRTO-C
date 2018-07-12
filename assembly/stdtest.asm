


jmp [functiontest_program_start]
%include assembly/std.asm
functiontest_program_start:



CALLFN(mod,11,6)
mov a,h
disp a

hlt



	


