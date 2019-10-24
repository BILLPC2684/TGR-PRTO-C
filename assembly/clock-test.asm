mov b,0
loop0:
 gclk a,0
 cmpgt a,1000
  call [tick]
 dsend e,0x0,0x07 ;render
 jmp [loop0]

tick:
 disp a
; drecv a,0x01,0x00
; disp a
 pop a
 inc b
 cmpeq b,60
  call [min]
 disp a
 disp b
 push a
 gclk a,1
 ret

min:
 mov b,0
 inc a
 ret
