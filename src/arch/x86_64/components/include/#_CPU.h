//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#    GPU COMPONENT FILE   #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#ifndef CPU_H
#define CPU_H

#include <TGR.h>

void  *CPU_EXEC(void *null);
void  *CPU_CLOCK(void *null);
void   GPU_reset();
int    argc;
char  *argv[10];
char  *FN;
bool   Exit;
bool   showInfo;
int    slowdown;
int    EMU_clock;
long   TIPS;
float  RunQuality;
bool   UInput[14][2];
bool   gotUIn;
char   execLoc[1024];
char   Title_Name[23];
char  *dropped_filedir;
bool   Title_lock;
double RAMUsage;
double VRAMUsage;
bool   p;
 
//   System_Error(       0, CPU.IS[], CPU.IP,     -1,     "CPU"); //CPU
//   System_Error(       0, CPU.IS[], CPU.IP,      0,     "GPU"); //GPU Component
static void System_Error( int Err, int Inst, int IP, int ID, char Name[]) {
 switch(Err) {
  case 0x0://Unknown Instruction
   printf("*[EMU ERROR]* ");
   if (ID == -1) { printf("CPU: "); } else{ printf("Component \"%s\"[0x%x]: ",Name,ID); }
   printf("Unknown Instrucion: 0x%x...\t[at 0x%x]\n",Inst,IP);
   CPU.running = 0;
   break;
  case 0x1://OutOfBounds || Can't send data
   if (ID == -1) { printf("*[EMU ERROR]* CPU: Index Out Of Bounds... Halting Emulation...\t[at 0x%x]\n",IP);
   } else {        printf("[EMU Nottice] Component \"%s\"[0x%x]: can't send data to this device...\t[at 0x%x]\n",Name,ID,IP); }
   break;
 }
}

#endif
