

#ifndef MAIN_H
#define MAIN_H

typedef struct {
 uint16_t REGs[8];
 uint8_t* IS;
 bool running,flag[8],debug,ticked,reset;
 int IP,SP,BP,IPC,RP,ISz;
 uint64_t TI,time;
 //IP:  InstuctionPointer for ROM/RAM
 //SP:  StackPointer for stack location in RAM
 //BP:  BasePoint for stack in RAM
 //IPC: InstructionsPerSecond
 //TI:  Total Instructions
 //RP:  RAMPointer
 //ISz: IndexSize
} CPU_INIT;

typedef struct {
 uint16_t x,y;
 uint8_t r,g,b,inst;
 bool run;
} GPU_INIT;

typedef struct {
 unsigned char *data;
 size_t size;
 bool found;
 FILE *fp;
} FileStruct;

//bool tmp_debug = true;
bool   termFPS;
int    FPSLimmit;
int    delay;
int    delay_skip;
bool   HUDinfo;
bool   ShowInput;
bool   CPU_lock;
char   execLoc[1024];
char   Title_Name[23];
char  *dropped_filedir;
bool   Title_lock;
double RAMUsage;
double VRAMUsage;
float  RunQuality;
char  *FN;

SDL_Event     event;
SDL_Renderer *GPU_SCREEN;
SDL_Window   *window;

uint8_t *RAM,*VRAM,*SAV;

CPU_INIT CPU;
GPU_INIT GPU;
int frames,FPS,IPS;

void getChar(char* Letter, int x, int y, int R, int G, int B, bool A, bool shadow);

#endif
