//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#     MAIN SYSTEM FILE    #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define RAMSIZ  (0x7FFFFFF)
#define VRAMSIZ (0x3FFFFFF)
#define ROMSIZ  (0xFFFFFF)
#define SAVSIZ  (0x7FFFFF)
#define BIOSIZ  (0x7FFF)
#define SW 480
#define SH 360

//bool tmp_debug = true;
bool termFPS = false;
int FPSLimmit = 60;
int delay = 0;
int delay_skip = 62;

SDL_Event event;
SDL_Renderer *GPU_SCREEN;
SDL_Window *window;

char* concat(const char *s1, const char *s2) {
 const size_t len1 = strlen(s1);
 const size_t len2 = strlen(s2);
 char *result = malloc(len1+len2+1);//+1 for the null-terminator
 //in real code you would check for errors in malloc here
 memcpy(result, s1, len1);
 memcpy(result+len1, s2, len2+1);//+1 to copy the null-terminator
 return result;
}

void bin_dump(uint64_t u) {
 int i = 63;
 while (i --> 0) {
  printf("%hhu", (u & (uint64_t) pow(2, i)) ? 1 : 0);
 }
 printf("\n");
}

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

FileStruct load(char *address, int make) {
 FileStruct file;
 file.found = true;
 if (access(address, F_OK) == -1) {
  printf("EMU ERROR: file(\"%s\") was not found...\n", address);
  if (make == 0) {
   file.found = false;
   return file;
  } else {
   printf("making file \"%s\"", address);
   file.fp = fopen(address, "wb");
   file.data = calloc(SAVSIZ, SAVSIZ);
   fwrite(file.data, SAVSIZ, 1, file.fp);
  }
 }
 file.fp = fopen(address, "rb");
 fseek(file.fp, 0, SEEK_END);
 file.size = ftell(file.fp);
 rewind(file.fp);
 file.data = malloc((file.size + 1) * sizeof(*file.data));
 fread(file.data, file.size, 1, file.fp);
 file.data[file.size] = '\0';
 return file;
}

int dumpData(char *name, FileStruct file, int size) {
 printf(".______________________________________________________.\n|%s",name);
 if        (strlen(name) < 2) {  printf("     ");
 } else if (strlen(name) < 3) {  printf("    ");
 } else if (strlen(name) < 4) {  printf("   ");
 } else if (strlen(name) < 5) {  printf("  ");
 } else if (strlen(name) < 6) {  printf(" ");
 } printf("|00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F|\n|------|-----------------------------------------------|\n|000000|");
 int j = 1;
 for (int i=0; i < file.size; i++) {
  if (j > 15) {
   if (file.data[i] < 0x10) { printf("0"); } printf("%x|\n|",file.data[i]);
   if       (i+1 < 0x1) {       printf("000000%x|",i+1); }
    else if (i+1 < 0x10) {      printf("00000%x|",i+1); }
    else if (i+1 < 0x100) {     printf("0000%x|",i+1); }
    else if (i+1 < 0x1000) {    printf("000%x|",i+1); }
    else if (i+1 < 0x10000) {   printf("00%x|",i+1); }
    else if (i+1 < 0x100000) {  printf("0%x|",i+1); }
    else if (i+1 < 0x1000000) { printf("%x|",i+1); }
   j = 0;
  } else {
   if (file.data[i] < 0x10) { printf("0"); } printf("%x ",file.data[i]);
  } j++;
 }
 if (j > 0) {
  for (int i=j; i < 16; i++) {
   printf("-- ");
  } printf("--|\n");
 } printf("|______|_______________________________________________|\n\\Size: 0x%x/%d Bytes(",file.size,file.size);
 if (file.size < 1024) { printf("%d KB)",file.size/1024); } else { printf("%d MB)",file.size/1024/1024); }
 printf(" of 0x%x/%d bytes(",size,size);
 if (size < 1024) { printf("%d KB)\n\n",(size/1024)+1); } else { printf("%d MB)\n\n",(size/1024/1024)+1); }
 return 0;
}

void crop(char *dst, char *src, size_t mn, size_t mx) {
 const int len = mx - mn;
 src += mn;
 for (int i = 0; i < len; i++) dst[i] = src[i];
}
uint8_t *RAM,*VRAM;

int main(int argc, char *argv[]) {
 int frames = 0;
 SDL_Init(SDL_INIT_VIDEO);
 SDL_CreateWindowAndRenderer(SW, SH, 2, &window, &GPU_SCREEN);
 SDL_SetRenderDrawColor(GPU_SCREEN, 0, 0, 0, 255);
 SDL_RenderClear(GPU_SCREEN);
 
 //printf("%s\n",concat(*ROMFN,".tgr"));
 
 printf("Loading TGR-PRTO v0.0.19 Alpha Build...\n");
 CPU_INIT CPU;
 
 bool SoftLoop = 0;
 int FPS = 0;
 int dskip = 0;
 char* REG = "ABCDEFGH********"; //* is used for if REG[index] is out of bounds to prevent a crash
 printf("%d\n",argc);
 for (int i=2; i<argc; i++) {
  printf("ARG[%d/%d]: %s\n",i,argc-1,argv[i]);
  if (strcmp(argv[i],"--debug") | strcmp(argv[i],"-d"     )) { CPU.debug = true; }
 }
 
 if (CPU.debug == true) { printf("Debug Mode: Enabled\n"); } else { printf("Debug Mode: Disbaled\n"); }
 
 #include "./bin/components/0_GPU.c"
 #include "./bin/components/1_INPUT.c"
 #include "./bin/components/2_SOUND.c"
 #include "./bin/components/3_NETWORK.c"
 //#include "./bin/components/4_.c"
 //#include "./bin/components/5_.c"
 //#include "./bin/components/6_.c"
 //#include "./bin/components/7_.c"
 //#include "./bin/components/8_.c"
 //#include "./bin/components/9_.c"
 //#include "./bin/components/A_.c"
 //#include "./bin/components/B_.c"
 //#include "./bin/components/C_.c"
 //#include "./bin/components/D_.c"
 //#include "./bin/components/E_.c"
 //#include "./bin/components/F_.c"
 
 printf("\\Initialize Memory...\n");
 RAM  = calloc(RAMSIZ,  sizeof(*RAM));
 VRAM = calloc(VRAMSIZ, sizeof(*VRAM));
 //ROM  = calloc(ROMSIZ,  sizeof(*ROM));
 //BIOS = calloc(BIOSIZ,  sizeof(*BIOS));
 for (int i=0;i<8;i++) { CPU.REGs[i] = 0; }
 printf(" \\0x%x\\%d\tBytes(%d MB)\tof RAM was allowcated...\n",RAMSIZ,RAMSIZ,(RAMSIZ/1024/1024)+1);
 printf(" \\0x%x\\%d\tBytes(%d MB)\tof VideoRAM was allowcated...\n",VRAMSIZ,VRAMSIZ,(VRAMSIZ/1024/1024)+1);
 CPU.running = true; CPU.TI = 0;
 
 FileStruct BIOS = load("./bin/BIOS.tgr",0);
 if (argc < 2) { printf("\nEMU Error: Requesting ROM path...\n\n"); return 0; }
 char* FN = argv[1];
 FileStruct ROM  = load(FN,0);
 if (ROM.found == false) { return 0; }
 char SN[strlen(FN)];
 crop(SN, FN, 0, strlen(FN)-4);
 strcat(SN, ".sav");
 printf("%s",SN);
 FileStruct SAV  = load(SN,1);
 
 printf("\n\\ROM Header info:\n \\Type: %d\n  \\System: %c%c%c\n",ROM.data[0],ROM.data[1],ROM.data[2],ROM.data[3]);
 dumpData("BIOS",BIOS,BIOSIZ);
 dumpData("ROM",ROM,ROMSIZ);
 CPU.IS = BIOS.data; CPU.ISz = BIOS.size; CPU.IP = 0; CPU.SP = 0; CPU.BP = 0;
 free(RAM); free(VRAM);
 //MAIN LOOP
 //CPU.running = 0;
 clock_t clock();
 int timer = clock();
 while(CPU.running) {
  uint8_t A   =  CPU.IS[CPU.IP+1] >> 4 ;       //4 \.
  uint8_t B   =  CPU.IS[CPU.IP+1] & 0xF;       //4 |-> A/B/C = 1.5 bytes
  uint8_t C   =  CPU.IS[CPU.IP+2] >> 4 ;       //4 /'
  int32_t IMM = (CPU.IS[CPU.IP+2] & 0xF) << 8; //4 \.
  IMM  = (IMM |  CPU.IS[CPU.IP+3]) << 8;       //8 |-> IMM   = 3.5 bytes
  IMM  = (IMM |  CPU.IS[CPU.IP+4]) << 8;       //8 | 
  IMM |=         CPU.IS[CPU.IP+5];             //8 /'
  if (CPU.debug == true) {
   printf("\nIC: 0x%x/0x%x   \t>> [",CPU.IP,CPU.ISz-1);
   for (int i=0; i < 6; i++) { if (CPU.IS[CPU.IP+i] < 0x10) { printf("0x0%x",CPU.IS[CPU.IP+i]); } else { printf("0x%x",CPU.IS[CPU.IP+i]); } if (i < 5) { printf(", "); } }
   printf("] | [A:%c, B:%c, C:%c, IMM:0x",REG[A],REG[B],REG[C]);
   if        (IMM < 0x10) {       printf("000000%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x100) {      printf("00000%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x1000) {     printf("0000%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x10000) {    printf("000%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x100000) {   printf("00%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x1000000) {  printf("0%x]\n\\REGs: [",IMM);
   } else if (IMM < 0x10000000) { printf("%x]\n\\REGs: [",IMM); }
   for (int i=0; i < 8; i++) {
    if        (CPU.REGs[i] < 0x10) {    printf("%c:0x000%x",REG[i],CPU.REGs[i]);
    } else if (CPU.REGs[i] < 0x100) {   printf("%c:0x00%x",REG[i],CPU.REGs[i]);
    } else if (CPU.REGs[i] < 0x1000) {  printf("%c:0x0%x",REG[i],CPU.REGs[i]);
    } else if (CPU.REGs[i] < 0x10000) { printf("%c:0x%x",REG[i],CPU.REGs[i]);
    } if (i < 7) { printf(", "); } 
   } printf("]\n \\instruction: "); //,CPU.REGs[0],CPU.REGs[1],CPU.REGs[2],CPU.REGs[3],CPU.REGs[4],CPU.REGs[5],CPU.REGs[6],CPU.REGs[7]);
  }
//  printf("IC:0x%x/%d | 0x%x\n",CPU.IP,CPU.IP,CPU.IS[CPU.IP]);
  switch(CPU.IS[CPU.IP]) {
   case 0x00:
    if (CPU.debug == true) { printf("LOAD\n"); }     //|+|-|-|0x---XXXX|REG[A] = IMM                                                                               |
    CPU.REGs[A] = IMM % (0x10000);
    break;
   case 0x01:
    if (CPU.debug == true) { printf("ADD\n"); }      //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A + B/IMM                                       |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] + CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] + (IMM % 0x10000);
    }
    break;
   case 0x02:
    if (CPU.debug == true) { printf("SUB\n"); }      //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A - B/IMM                                       |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] - CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] - (IMM % 0x10000);
    }
    break;
   case 0x03:
    if (CPU.debug == true) { printf("MUL\n"); }      //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A * B/IMM                                       |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] * CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] * (IMM % 0x10000);
    }
    break;
   case 0x04:
    if (CPU.debug == true) { printf("DIV\n"); }      //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A / B/IMM                                       |
    if (IMM == 0) {
     if (B == 0) {
      printf("EMU Error: try'd to divide by 0\n");
      CPU.REGs[C] = 0;
      CPU.flag[4] = 1;
     } else {
      CPU.REGs[C] = CPU.REGs[A] / CPU.REGs[B];
     }
    } else {
     CPU.REGs[C] = CPU.REGs[A] / (IMM % 0x10000);
    }
    break;
   case 0x05:
    if (CPU.debug == true) { printf("AND\n"); }     //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A &AND B/IMM                                     |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] & CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] & (IMM % 0x10000);
    }
    break;
   case 0x06:
    if (CPU.debug == true) { printf("OR\n"); }      //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A |OR  B/IMM                                     |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] | CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] | (IMM % 0x10000);
    }
    break;
   case 0x07:
    if (CPU.debug == true) { printf("XOR\n"); }     //|+|+|+|0x---XXXX|if IMM > 0 then replace B with IMM  |  C = A ^XOR B/IMM                                     |
    if (IMM == 0) {
     CPU.REGs[C] = CPU.REGs[A] ^ CPU.REGs[B];
    } else {
     CPU.REGs[C] = CPU.REGs[A] ^ (IMM % 0x10000);
    }
    break;
   case 0x08:
    if (CPU.debug == true) { printf("NOT\n"); }     //|+|+|-|0x-------|B = NOT A                                                                                   |
    CPU.REGs[B] = ~CPU.REGs[A];
    break;
   case 0x09:
    if (CPU.debug == true) { printf("SPLIT\n"); }   //|+|+|+|0x------X|B,C = A splitted | if IMM == 0: 8-bit split else 4-bit split                                |
    if ((IMM % 0x10) == 0) {
     CPU.REGs[C] = CPU.REGs[A] & 0xFF;
     CPU.REGs[B] = CPU.REGs[A] >> 8;
    } else {
     CPU.REGs[C] = CPU.REGs[A] & 0xF;
     CPU.REGs[B] = CPU.REGs[A] >> 4;
    }
//    printf(">>>>>>  A:0x%x >> B:0x%x, C:0x%x (SPLITED)  <<<<<<\n",CPU.REGs[A],CPU.REGs[B],CPU.REGs[C]);
    break;
   case 0x0A:
    if (CPU.debug == true) { printf("COMBINE\n"); } //|+|+|+|0x------X|C = A combined with B | IMM rules same as SPLIT                                             |
    if ((IMM % 0x10) == 0) {
     CPU.REGs[C] = (CPU.REGs[A] << 8) | (CPU.REGs[B] & 0xFF);
    } else {
     CPU.REGs[C] = (CPU.REGs[A] << 4) | (CPU.REGs[B] & 0xF);
    }
//    printf(">>>>>>  A:0x%x, B:0x%x >> C:0x%x (COMBINE)  <<<<<<\n",CPU.REGs[A],CPU.REGs[B],CPU.REGs[C]);
    break;
   case 0x0B:
    if (CPU.debug == true) { printf("JMP\n\n"); }     //|+|+|/|0x-XXXXXX|if C == 1 then IC = A..B else IC = IMM                                                     |
    if (C == 1) {
     CPU.IP = ((CPU.REGs[A] << 16) | CPU.REGs[B])-6;
    } else {
     CPU.IP = (IMM % 0x1000000)-6;
    }
    break;
   case 0x0C:
    if (CPU.debug == true) { printf("CMP=: "); }    //|+|+|-|0x-------|if A == B: IC=IC+1(expected JMP) else IC=IC+2                                               |
    if (CPU.REGs[A] == CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x0D:
    if (CPU.debug == true) { printf("CMP<: "); }    //|+|+|-|0x-------|if A < B: IC=IC+1(expected JMP) else IC=IC+2                                                |
    if (CPU.REGs[A] < CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x0E:
    if (CPU.debug == true) { printf("CMP>: "); }    //|+|+|-|0x-------|if A > B: IC=IC+1(expected JMP) else IC=IC+2                                                |
    if (CPU.REGs[A] > CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x0F:
    if (CPU.debug == true) { printf("RAMPOS\n"); }  //|+|+|/|0xXXXXXXX|RAMPointer = IMM                                                                            |
    CPU.RP = IMM;
    break;
   case 0x10:
    if (CPU.debug == true) { printf("RRAM\n"); }    //|+|/|/|0xXXXXXXX|if B == 1 then use VRAM instead of RAM / if C == 1 then IMM will replace with RAMPointer    |
    if (B == 1) {
     if (C == 1) {
      CPU.REGs[A] = VRAM[IMM % 0x1000000];
     } else {
      CPU.REGs[A] = VRAM[CPU.RP];
     }
    } else {
     if (C == 1) {
      CPU.REGs[A] = RAM[IMM % 0x1000000];
     } else {
      CPU.REGs[A] = RAM[CPU.RP];
     }
    }
    break;
   case 0x11:
    if (CPU.debug == true) { printf("WRAM\n"); }    //|+|/|/|0xXXXXXXX|if B == 1 then index VRAM else RAM / if C == 1 then IMM will replace with RAMPointer        |
    if (B == 1) {
     if (C == 1) {
      VRAM[IMM % 0x1000000] = CPU.REGs[A];
     } else {
      VRAM[CPU.RP] = CPU.REGs[A];
     }
    } else {
     if (C == 1) {
      RAM[IMM % 0x1000000] = CPU.REGs[A];
     } else {
      RAM[CPU.RP] = CPU.REGs[A];
     }
    }
    break;
   case 0x12:
    if (CPU.debug == true) { printf("RSAV\n"); }    //|+|+|+|0x-------|C = SAV.data[A..B]                                                                          |
    CPU.REGs[C] = SAV.data[(CPU.REGs[A] << 16) | CPU.REGs[B]];
    break;
   case 0x13:
    if (CPU.debug == true) { printf("IRSAV\n"); }   //|+|-|-|0x-XXXXXX|A = SAV.data[IMM]                                                                           |
    CPU.REGs[A] = (SAV.data[IMM] % 0x1000000);
    break;
   case 0x14:
    if (CPU.debug == true) { printf("WSAV\n"); }    //|+|-|-|0x-------|SAV.data[A..B] = C                                                                          |
    SAV.data[(CPU.REGs[A] << 16) | CPU.REGs[B]] = CPU.REGs[C];
    break;
   case 0x15:
    if (CPU.debug == true) { printf("IWSAV\n"); }   //|+|+|+|0x-XXXXXX|SAV.data[IMM] = A                                                                           |
    SAV.data[IMM] = (CPU.REGs[A] % 0x1000000);
    break;
   case 0x16:
    if (CPU.debug == true) { printf("RROM\n"); }    //|+|+|+|0x-------|C = ROM.data[A..B]                                                                          |
    CPU.REGs[C] = ROM.data[(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B]) % CPU.ISz];
//    printf(">>>>>>  Adr:0x%x/%d, data:0x%x (ReadROM)  <<<<<<\n",(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B])%CPU.ISz,(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B])%CPU.ISz, CPU.REGs[C]);
    break;
   case 0x17:
    if (CPU.debug == true) { printf("IRROM\n"); }   //|+|-|-|0x-XXXXXX|A = ROM.data[IMM]                                                                           |
    CPU.REGs[A] = ROM.data[IMM % CPU.ISz];
//    printf(">>>>>>  Adr:0x%x/%d, data:0x%x (ImmReadROM)  <<<<<<\n",IMM % CPU.ISz,IMM % CPU.ISz, CPU.REGs[A]);
    break;
   case 0x18:
    if (CPU.debug == true) { printf("HALT\n"); }    //|-|-|-|0x------X|halts index IMM located in [[HALT-INFO]]                                                    |
    switch(IMM) {
     case 0x0: //|nothing                                 |N/A        |
      break;
     case 0x1: //|stop running everything                 |CPU        |
      printf("EMU: CPU HATLTED...\n");
      CPU.running = 0;
      break;
     case 0x2: //|resets the Audio                        |AUDIO      |
      break;
     case 0x3: //|resets the Input                        |INPUT      |
      //            A B C X Y Z L R S s ^ v < >
      //input.P1 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
      //input.P2 = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
      break;
     case 0x4: //|resets Network System                   |NETWORK    |
      break;
     case 0x5: //|stops everything and prints IP          |CPU        |
      printf("EMU: CPU HATLTED at InstructionPointer 0x%x/%d...\n",CPU.IP,CPU.IP);
      CPU.running = 0;
      break;
     case 0x7: //|nothing                                 |N/A        |
      break;
     case 0x8: //|nothing                                 |N/A        |
      break;
     case 0x9: //|nothing                                 |N/A        |
      break;
     case 0xA: //|nothing                                 |N/A        |
      break;
     case 0xB: //|nothing                                 |N/A        |
      break;
     case 0xC: //|nothing                                 |N/A        |
      break;
     case 0xD: //|nothing                                 |N/A        |
      break;
     case 0xE: //|nothing                                 |N/A        |
      break;
     case 0xF: //|resets everything(restarts the emu)     |EMU        |
      CPU.IS  = BIOS.data;
      CPU.ISz = BIOS.size;
      CPU.IP  = 0; CPU.IPS = 0; CPU.TI  = 0;
      break;
    } break;
   case 0x19:
    if (CPU.debug == true) { printf("PRINT\n"); }   //|+|-|-|0x-------|prints A in emulator terminal(for debugging ROMs only)                                      |
    if (CPU.REGs[A] > 9999) {
     printf(">> %d\t(0x%x)\n",CPU.REGs[A],CPU.REGs[A]);
    } else {
     printf(">> %d\t\t(0x%x)\n",CPU.REGs[A],CPU.REGs[A]);
    } break;
   case 0x1A:
    if (CPU.debug == true) { printf("FLAGS\n"); }   //|+|-|-|0x------X|A = Flags[IMM]                                                                              |
    CPU.REGs[A] = CPU.flag[IMM];
    break;
   case 0x1B:
    if (CPU.debug == true) { printf("DVCSEND\n"); } //|+|-|-|0x----XXX|send message to device IMM[3], IMM[1-2] are for device oICode                               |
    switch ((IMM >> 8) % 0x10) {
     case 0: //GPU
      GPU_send(IMM & 0xFF, A);
      break;
     case 0x1: //GPU
      break;
     case 0x2: //
      break;
     case 0x3: //
      break;
     case 0x4: //
      break;
     case 0x5: //
      break;
     case 0x6: //
      break;
     case 0x7: //
      break;
     case 0x8: //
      break;
     case 0x9: //
      break;
     case 0xA: //
      break;
     case 0xB: //
      break;
     case 0xC: //
      break;
     case 0xD: //
      break;
     case 0xE: //
      break;
     case 0xF: //
      break;
    }
    break;
   case 0x1C:
    if (CPU.debug == true) { printf("DVCRECV\n"); } //|+|-|-|0x----XXX|same as DVCSEND but recive then send                                                        |
    break;
   case 0x1D:
    if (CPU.debug == true) { printf("ICOUT\n"); }   //|+|+|-|0x-------|A,B = IC(32-bit for 24-bit)                                                                 |
    CPU.REGs[A] = CPU.IP & 0xFF;
    CPU.REGs[B] = CPU.IP >> 16;
    break;
   case 0x1E:
    if (CPU.debug == true) { printf("COPY\n"); }    //|+|+|-|0x-------|A = B                                                                                       |
    CPU.REGs[B] = CPU.REGs[A];
    break;
   case 0x1F:
    if (CPU.debug == true) { printf("EXECUTE\n"); } //|-|-|-|0xXXXXXXX|execute location = IMM[6] and IMM[0-5]                                                      |
    printf("EMU NOTTICE: "); CPU.IP = IMM-6;
    switch(A) {
     case 0x0: //executes BIOS
      printf("Switching to BIOS execute");
      CPU.IS  = BIOS.data;
      CPU.ISz = BIOS.size;
      break;
     case 0x1: //executes ROM
      printf("Switching to ROM execute");
      CPU.IS  = ROM.data;
      CPU.ISz = ROM.size;
      break;
     case 0x2: //executes RAM
      printf("Switching to RAM execute");
      CPU.IS  = RAM;
      CPU.ISz = sizeof(RAM);
      break;
    } if (CPU.debug == true) { printf(" at 0x%x/%x...\n",CPU.IP+6,CPU.ISz); } else { printf("...\n"); }
    break;
   case 0x20:
    if (CPU.debug == true) { printf("RBIOS\n"); }    //|+|+|+|0x-------|C = BIOS.data[A..B]                                                                         |
    CPU.REGs[C] = BIOS.data[(CPU.REGs[A] << 16) | CPU.REGs[B]];
    break;
   case 0x21:
    
    break;
   case 0x22:
    
    break;
   case 0x23:
    
    break;
   case 0x24:
    
    break;
   case 0x25:
    
    break;
   case 0x26:
    
    break;
   case 0x27:
    
    break;
   case 0x28:
    
    break;
   case 0x29:
    
    break;
   case 0x2A:
    
    break;
   case 0x2B:
    
    break;
   case 0x2C:
    
    break;
   case 0x2D:
    
    break;
   case 0x2E:
    
    break;
   case 0x2F:
    
    break;
   default:
    if (CPU.debug == true) { printf("UNKNOWN\n  \\"); }
    printf("ERROR: unknown instruction: 0x%x\n",CPU.IS[CPU.IP]);
    CPU.running = 0;
  }
  CPU.IP = CPU.IP + 6;
  CPU.IPS++;
  CPU.TI++;
  if (CPU.IP > CPU.ISz) { //sizeof(CPU.IS)*6) {
   if (SoftLoop == 1) {
    CPU.IP = 0;
   } else {
    printf("EMU Error: Index Out Of Bounds... Halting Emulation...\n");
    CPU.running = 0;
   }
  }
  if (clock() > timer+1000) {
   FPS = frames;
   if (termFPS == true) { printf("FPS:%d\n",FPS); }
   frames = 0;
   timer = clock();
  }
  //update();
  SDL_RenderPresent(GPU_SCREEN);
  if (dskip <= delay_skip) { dskip++; } else { SDL_Delay(delay); }
 }
 SDL_DestroyRenderer(GPU_SCREEN);
 SDL_DestroyWindow(window);
 SDL_Quit();
 printf("EMU Nottice: Screen Closed...\n");
 printf("\nEMU: We have detected the main loop has stopped due to a halt...\n\nThis is due to a halt(meaning the program has ended) or is a Emulation Error.\nIf there is a Emulation Error check above for what the probblem is in the ROM.\n");
 //printf("The screen isn't active but is can still be viewed, to reset the emulator you need to goto your\n terminal and press [CTRL] + [C] and relaunch.");
 //printf("TIP: pressing [CTRL] + [M] will list RAM from RenderRAMPOS + 0x0000 to RenderRAMPOS + 0x02FF\n same with VideoRAM by pressing [CTRL] + [V]...\n and pressing [CTRL] + [KeyPad+] or [CTRL] + [KeyPad-] will adjust RenderRAMPOS by 0x10");
 printf("\nTotal ran instructions: %d\n\n",CPU.TI);
 return 0;
}

//right val nibble = byte & 0xF;
//left  val nibble = byte >> 4;
