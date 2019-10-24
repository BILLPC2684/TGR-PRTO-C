%TITLE
mov a,0
mov b,0
mov c,0xFF
loop0:
 ;dsend g,0x0,0x07
 rpos a,b
 wram c
 disp a,b
 rram d
 disp d
 mov d,0x400
 cmplt a,d
  call [Vwrite]
 mov d,0xFFFF
 cmpeq b,d
 jmp [bfull]
 add b,1,b
 jmp [loop0]
bfull:
 add a,1,a
 mov b,0
 mov d,0x800
 cmpeq a,d
 hlt
 jmp [loop0]
Vwrite:
 wvram c
 rvram d
 disp d
 ret