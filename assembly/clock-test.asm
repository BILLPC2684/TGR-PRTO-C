mov b,1000
mov c,0
loop0:
 gclk a,0
 cmpgt a,b
  jmp [tick]
 dsend e,0x0,0x07 ;render
 jmp [loop0]

tick:
; drecv a,0x01,0x00
; disp a
 gclk a,1
 disp c
 inc c
; mov b,a
 add b,1000,b
 jmp [loop0]
