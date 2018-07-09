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


void GPU_send(int inst, int arg) {
// printf("GPU: inst:%d\t,arg:%d\n",inst,arg);
 switch(inst) {
  case 0x00:// |   SET X    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.X to 0x%x\n", CPU.REGs[arg]); }
   GPU_x = CPU.REGs[arg];
   break;
  case 0x01:// |   SET Y    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.y to 0x%x\n", CPU.REGs[arg]); }
   GPU_y = CPU.REGs[arg];
   break;
  case 0x02:// |   SET R    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.R to 0x%x\n", CPU.REGs[arg]); }
   GPU_r = CPU.REGs[arg];
   break;
  case 0x03:// |   SET G    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.G to 0x%x\n", CPU.REGs[arg]); }
   GPU_g = CPU.REGs[arg];
   break;
  case 0x04:// |   SET B    |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.B to 0x%x\n", CPU.REGs[arg]); }
   GPU_b = CPU.REGs[arg];
   break;
  case 0x05:// | SET CPallet|  +  |
   arg = CPU.REGs[arg]%0x10;
   if (CPU.debug == true) { printf("GPU: Setting GPU.CP[0x%x\t] to [R:0x%x\t,G:0x%x\t,B:0x%x\t]\n",arg,GPU_r,GPU_g,GPU_b); }
   GPU_cp[arg][0] = GPU_r;
   GPU_cp[arg][1] = GPU_g;
   GPU_cp[arg][2] = GPU_b;
   break;
  case 0x06:// |   plot     |  +  |
   arg = CPU.REGs[arg]%0x20;
   if (arg > 0xF) {
    if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.REGs (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n", GPU_x, GPU_y, GPU_r, GPU_g, GPU_b); }
    buffer[GPU_x][GPU_y][0] = GPU_r;          buffer[GPU_x][GPU_y][1] = GPU_g;          buffer[GPU_x][GPU_y][2] = GPU_b;
   } else if (arg == 0x0) {
    if (CPU.debug == true) { printf("GPU: Skiping pixel plot at [X:0x%x\t,Y:0x%x\t]\n", GPU_x, GPU_y); }
   } else {
    if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.CP[0x%x\t] (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n",GPU_x, GPU_y, arg, GPU_cp[arg][0], GPU_cp[arg][1], GPU_cp[arg][2]); }
    buffer[GPU_x][GPU_y][0] = GPU_cp[arg][0]; buffer[GPU_x][GPU_y][1] = GPU_cp[arg][1]; buffer[GPU_x][GPU_y][2] = GPU_cp[arg][2];
   }
   break;
  case 0x07:// |   update   |  -  |
   if (frames < FPSLimmit) {
    if (CPU.debug == true) { printf("GPU: update\n"); }
    SDL_SetRenderDrawColor(GPU_SCREEN, 0, 0, 0, 255);
    SDL_RenderClear(GPU_SCREEN);
    for (int y=0;y<SH;y++) {
     for (int x=0;x<SW;x++) {
      SDL_SetRenderDrawColor(GPU_SCREEN, buffer[x][y][0], buffer[x][y][1], buffer[x][y][2], 255);
      SDL_RenderDrawPoint(GPU_SCREEN, x, y);
     }
    }
    SDL_RenderPresent(&GPU_SCREEN);
    frames++;
   } else { if (CPU.debug == true) { printf("GPU: update skip'd\n"); } }
   break;
 }
}

int GPU_recv(int inst, int arg) {
 switch(inst) {
  case 0x00:// |  GET PIXEL |  +  |
   
   break;
  case 0x01:// |  GET W     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Width"); }
   return SW;
   break;
  case 0x02:// |  GET H     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Hight"); }
   return SH;
   break;
 }
}
