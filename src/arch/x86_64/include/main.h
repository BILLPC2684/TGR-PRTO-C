

#ifndef MAIN_H
#define MAIN_H

typedef struct {
 uint16_t REGs[8];
 uint8_t* IS;
 bool running,flag[8],debug;
 int IP,SP,BP,IPS,TI,RP,ISz; //IP: InstuctionPointer for ROM/RAM //SP: StackPointer for stack location in RAM //BP: BasePoint for stack in RAM //IPS: InstructionsPerSecond //TI: Total Instructions //RP: RAMPointer //ISz: IndexSize
} CPU_INIT;

typedef struct {
 unsigned char *data;
 size_t size;
 bool found;
 FILE *fp;
} FileStruct;




//bool tmp_debug = true;
bool termFPS;
int FPSLimmit;
int delay;
int delay_skip;

SDL_Event event;
SDL_Renderer *GPU_SCREEN;
SDL_Window *window;

uint8_t *RAM,*VRAM;

CPU_INIT CPU;
int frames;

#endif