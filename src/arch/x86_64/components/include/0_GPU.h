//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#    GPU COMPONENT FILE   #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#ifndef GPU_H
#define GPU_H

#include <TGR.h>

int GPU_x,GPU_y;
uint8_t GPU_r,GPU_g,GPU_b,GPU_cp[15][3];
uint8_t buffer[SW][SH][3];

void GPU_send(int inst, int arg);
int GPU_recv(int inst, int arg);


#endif