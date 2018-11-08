//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#  INPUT COMPONENT FILE   #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#include <TGR.h>

/*
14 BUTTONS(fits in 1 16-bit int)
(A)       :   1
(B)       :   2
(C)       :   4
(X)       :   8
(Y)       :  16
(Z)       :  32
/L|       :  64
|R\       : 128
(START)   : 256
(SELECT)  : 512
|Up|      :1024
|Down|    :2048
[Left     :4096
Right]    :8192
*/

//uint16_t controller[2] = {0};
bool     gotUIn;
bool     UInput[14][2];
char name1[] = "INPUT";

void INPUT_send(int inst, int arg) {
// printf("GPU: inst:%d\t,arg:%d\n",inst,arg);
 switch(inst) {
  default:// nothing
   System_Error(1, inst, CPU.IP, 0, name1);
   break;
 }
}

int  INPUT_recv(int inst) {
 switch(inst) {
  case 0x00:// |RECV CONTR.0|  +  |
   if (CPU.debug == true) { printf("INPUT: Returning Controler[0]'s input data"); }
//   while (gotUIn == false) {}
   return (UInput[0][0]*1)+(UInput[1][0]*2)+(UInput[2][0]*4)+(UInput[3][0]*8)+(UInput[4][0]*16)+(UInput[5][0]*32)+(UInput[6][0]*64)+(UInput[7][0]*128)+(UInput[8][0]*256)+(UInput[9][0]*512)+(UInput[10][0]*1024)+(UInput[11][0]*2048)+(UInput[12][0]*4096)+(UInput[13][0]*8192);
   break;
  case 0x01:// |RECV CONTR.1|  +  |
   if (CPU.debug == true) { printf("INPUT: Returning Controler[1]'s input data"); }
//   while (gotUIn == false) {}
   return (UInput[0][1]*1)+(UInput[1][1]*2)+(UInput[2][1]*4)+(UInput[3][1]*8)+(UInput[4][1]*16)+(UInput[5][1]*32)+(UInput[6][1]*64)+(UInput[7][1]*128)+(UInput[8][1]*256)+(UInput[9][1]*512)+(UInput[10][1]*1024)+(UInput[11][1]*2048)+(UInput[12][1]*4096)+(UInput[13][1]*8192);
   break;
  case 0x02:// |PING CONTR.0|  +  |
   if (CPU.debug == true) { printf("INPUT: Returning Controler[0]'s input type"); }
   return 0; // no controller types defined yet
   break;
  case 0x03:// |PING CONTR.1|  +  |
   if (CPU.debug == true) { printf("INPUT: Returning Controler[1]'s input type"); }
   return 0; // no controller types defined yet
   break;
  default:// nothing
   System_Error(0, inst, CPU.IP, 0, name1);
   break;
 }
 return 0;
}
