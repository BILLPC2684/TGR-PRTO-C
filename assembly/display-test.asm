; ._______________.
; |DISPLAY TESTING|
; |_______________|


jmp [start]

delay:
 push a
 push b
 gclk a,1
 mov b,1000
 deloop:
  dsend a,0x0,0x07 ;render
  gclk a,0
  disp a,b
  cmpgt a,b
   jmp [deret]
  jmp [deloop]
 deret:
  pop b
  pop a
  ret

start:
 dsend f,0x0,0x07  ;render
 mov b,0
 mov c,0
 drecv d,0x0,0x00   ;width
 drecv h,0x0,0x01   ;height
 loop0:
  cmpgt c,h
   jmp [start]
  loop1:
   cmpgt b,d
    jmp [reset_x]
   gclk f,0
   dsend f,0x0,0x02  ;set R
   dsend f,0x0,0x03  ;set G
   dsend f,0x0,0x04  ;set B
   
   dsend b,0x0,0x00  ;set xpos
   dsend c,0x0,0x01  ;set ypos
   dsend f,0x0,0x06  ;plot
   dsend f,0x0,0x07  ;render
   add b,1,b
   call [delay]
   jmp [loop1]
  reset_x:
   dsend g,0x0,0x07 ;updating screen
   mov b,0
   add c,1,c
   jmp [loop0]

;end:
; dsend f,0x0,0x07  ;render
; hlt
