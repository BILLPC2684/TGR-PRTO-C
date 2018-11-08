;|0x13|  RSAV  |+|+|+|0x-------|C = SAV.data[A..B]                                                                          |
;|0x15|  WSAV  |+|+|+|0x------X|SAV.data[A..B] = C                                                                          |

mov a,0
mov b,0
mov c,0xFF
loop0:
 raw 0x13,0x01,0x30,0x00,0x00,0x00 ;RSAV a..b,d
 add d,3,d
 raw 0x15,0x01,0x30,0x00,0x00,0x00 ;WSAV a..b,d
 disp b
 cmpeq b,c
 jmp [end]
 add b,1,b
 jmp [loop0]
end:
 hlt
