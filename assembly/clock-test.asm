mov b,0
loop0:
 gclk a,0
 cmpgt a,1000
  call [tick]
 dsend e,0x0,0x07 ;render
 jmp [loop0]

tick:
; drecv a,0x01,0x00
; disp a
 gclk a,1
 inc b
 disp b
 cmpeq b,60
  call [min]
 ret

min:
 mov b,0
 push a
 mov a,256
 disp a
 pop a
 ret
