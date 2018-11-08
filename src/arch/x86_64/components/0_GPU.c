//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#    GPU COMPONENT FILE   #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#include <TGR.h>

uint8_t buffer[SW][SH][3] = {{{0}}};
char name0[] = "GPU";

void GPU_send(int inst, int arg) {
// printf("GPU: inst:%d\t,arg:%d\n",inst,arg);
 switch(inst) {
  case 0x00:// |   SET X    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.X to 0x%x\n", arg); }
   if (arg >= 0) {
    if (arg < SW) { GPU_x = arg; }
    else { GPU_x = SW; }
   } else {
    { GPU_x = 0; }
   }
   break;
  case 0x01:// |   SET Y    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.y to 0x%x\n", arg); }
   if (arg >= 0) {
    if (arg < SH) { GPU_y = arg; }
    else { GPU_y = SH; }
   } else {
    { GPU_y = 0; }
   }
   break;
  case 0x02:// |   SET R    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.R to 0x%x\n", arg); }
   GPU_r = arg % 0x100;
   break;
  case 0x03:// |   SET G    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.G to 0x%x\n", arg); }
   GPU_g = arg % 0x100;
   break;
  case 0x04:// |   SET B    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.B to 0x%x\n", arg); }
   GPU_b = arg % 0x100;
   break;
  case 0x05:// | SET CPallet|  +  |
   larg = arg%0x10;
   if (CPU.debug == true) { printf("GPU: Setting GPU.CP[0x%x\t] to [R:0x%x\t,G:0x%x\t,B:0x%x\t]\n",larg,GPU_r,GPU_g,GPU_b); }
   GPU_cp[larg][0] = GPU_r;
   GPU_cp[larg][1] = GPU_g;
   GPU_cp[larg][2] = GPU_b;
   break;
  case 0x06:// |   plot     |  +  |
   if (GPU_x < SW && GPU_y < SW) {
    larg = arg%0x20;
    if (arg > 0xF) {
     if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.REGs (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n", GPU_x, GPU_y, GPU_r, GPU_g, GPU_b); }
     buffer[GPU_x][GPU_y][0] = GPU_r;
     buffer[GPU_x][GPU_y][1] = GPU_g;
     buffer[GPU_x][GPU_y][2] = GPU_b;
    } else if (larg == 0x0) {
     if (CPU.debug == true) { printf("GPU: Skiping pixel plot at [X:0x%x\t,Y:0x%x\t]\n", GPU_x, GPU_y); }
    } else {
     if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.CP[0x%x\t] (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n",GPU_x, GPU_y, larg, GPU_cp[larg][0], GPU_cp[larg][1], GPU_cp[larg][2]); }
     buffer[GPU_x][GPU_y][0] = GPU_cp[larg][0];
     buffer[GPU_x][GPU_y][1] = GPU_cp[larg][1];
     buffer[GPU_x][GPU_y][2] = GPU_cp[larg][2];
    }
   } else {
    if (CPU.debug == true) { printf("GPU NOTTICE: skipping pixel at [X:0x%x\t,Y:0x%x\t] due to being OutOfBounds...\n", GPU_x, GPU_y); }
   }
   if (forceRender == true) { GPU.run  = true; }
   break;
  case 0x07:// |   update   |  -  |
   GPU.run  = true;
   break;
  default:// nothing
   System_Error(0, inst, CPU.IP, 0, name0);
   break;
 }
}

int  GPU_recv(int inst) {
 switch(inst) {
  case 0x00:// |  GET W     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Width"); }
   return SW-1;
   break;
  case 0x01:// |  GET H     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Hight"); }
   return SH-1;
   break;
  case 0x03:// |  GET GPU.R |  +  |
   return buffer[GPU_x][GPU_y][0];
   break;
  case 0x04:// |  GET GPU.G |  +  |
   return buffer[GPU_x][GPU_y][1];
   break;
  case 0x05:// |  GET GPU.B |  +  |
   return buffer[GPU_x][GPU_y][2];
   break;
  default:// nothing
   System_Error(0, inst, CPU.IP, 0, name0);
   break;
 }
 return 0;
}

void GPU_reset() {
 for (int i=0;i<15;i++) {
  GPU_cp[i][0] = 0x00;
  GPU_cp[i][1] = 0x00;
  GPU_cp[i][2] = 0x00;
 }
 for (int y=0;y<SH;y++) {
  for (int x=0;x<SW;x++) {
   buffer[x][y][0] = 0x00;
   buffer[x][y][1] = 0x00;
   buffer[x][y][2] = 0x00;
  }
 }
}
