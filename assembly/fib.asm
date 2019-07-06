%TITLE

mov a,1
mov b,0
mov c,0x6ff1

floop0:
 add a,b,a
 disp a
 cmpeq a,c
  jmp [fend0]
 add b,a,b
 disp b
 cmpeq a,c
  jmp [fend0]
 jmp [floop0]
fend0:
 hlt
