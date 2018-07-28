//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#     MAIN SYSTEM FILE    #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//


#include <TGR.h>
#include <arraylist.h>

//bool tmp_debug = true;
bool termFPS = false;
int FPSLimmit = 60;
int delay = 0;
int delay_skip = 62;
int frames = 0;


int main(int argc, char *argv[]) {
 
 // CPU.debug = true;
 //puts cpu in debug mode when BIOS has executed and the main rom is initialized
 bool stopatloadrom = false;

 SDL_Init(SDL_INIT_VIDEO);
 SDL_CreateWindowAndRenderer(SW, SH, 2, &window, &GPU_SCREEN);
 SDL_SetRenderDrawColor(GPU_SCREEN, 0, 0, 0, 255);
 SDL_RenderClear(GPU_SCREEN);
 
 //printf("%s\n",concat(*ROMFN,".tgr"));
 
 printf("Loading TGR-PRTO v0.0.19 Alpha Build...\n");
 
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

 printf("\\Initialize Memory...\n");
 RAM  = calloc(RAMSIZ,  sizeof(*RAM));
 if (!RAM){printf("MALLOC FAILED\n");exit(1);}
 VRAM = calloc(VRAMSIZ, sizeof(*VRAM));
 if (!VRAM){printf("MALLOC FAILED\n");exit(1);}
 InterruptVector = calloc(INTSIZ, sizeof(interrupt));
 if (!InterruptVector){printf("MALLOC FAILED\n");exit(1);}
 //ROM  = calloc(ROMSIZ,  sizeof(*ROM));
 //BIOS = calloc(BIOSIZ,  sizeof(*BIOS));

 //initialize BP to the end of ram and SP to the same point
 CPU.BP = RAMSIZ-1;
 CPU.SP = CPU.BP; 

 //zero reg A-H
 for (int i=0;i<8;i++) { CPU.REGs[i] = 0; }
 printf(" \\0x%x\\%d\tBytes(%d MB)\tof RAM were allocated...\n",RAMSIZ,RAMSIZ,(RAMSIZ/1024/1024)+1);
 printf(" \\0x%x\\%d\tBytes(%d MB)\tof VideoRAM was allocated...\n",VRAMSIZ,VRAMSIZ,(VRAMSIZ/1024/1024)+1);
 CPU.running = true; CPU.TI = 0;
 
 FileStruct BIOS = load("./bin/BIOS.tgr",0);
 // FileStruct BIOS = load("./bin/simplebios.tgr",0);
 if (argc < 2) { printf("\nEMU Error: Requesting ROM path...\n\n"); return 0; }
 char* FN = argv[1];
 FileStruct ROM  = load(FN,0);
 if (ROM.found == false) { return 0; }
 
 char SN[strlen(FN)+1];
 crop(SN, FN, 0, strlen(FN)-4);
 printf("%s\n",SN);
 strcat(SN, ".sav");
 printf("%s",SN);
 FileStruct SAV  = load(SN,1);
 
 printf("\n\\ROM Header info:\n \\Type: %d\n  \\System: %c%c%c\n",ROM.data[0],ROM.data[1],ROM.data[2],ROM.data[3]);
 dumpData("BIOS",BIOS,BIOSIZ);
 dumpData("ROM",ROM,ROMSIZ);
 CPU.IS = BIOS.data; CPU.ISz = BIOS.size; CPU.IP = 0; CPU.SP = 0; CPU.BP = 0;
 currentexeccontext = BIOSCTX; //set context to BIOS
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
   }
   printf("]\nstack:\n["); 
   for (int i = CPU.SP+1; i <= CPU.BP; ++i){
     if((i+1)%2==0){
        printf(" 0x");
      }
      printf("%02x",RAM[i]);
      if(i%16==0 && i != 0){
        printf("\n");
      }
   }

   printf("]\n \\instruction: "); //,CPU.REGs[0],CPU.REGs[1],CPU.REGs[2],CPU.REGs[3],CPU.REGs[4],CPU.REGs[5],CPU.REGs[6],CPU.REGs[7]);
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
    CPU.IP = ((CPU.REGs[A] << 16) | CPU.REGs[B])-6;
    break;
   case 0x0C:
    if (CPU.debug == true) { printf("JMPIMM\n\n"); } 
    CPU.IP = (IMM % 0x1000000)-6;
    break;
   case 0x0D:
    if (CPU.debug == true) { printf("CMP=: "); }    //|+|+|-|0x-------|if A == B: IC=IC+1(expected JMP) else IC=IC+2                                               |
    if (CPU.REGs[A] == CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x0E:
    if (CPU.debug == true) { printf("CMP<: "); }    //|+|+|-|0x-------|if A < B: IC=IC+1(expected JMP) else IC=IC+2                                                |
    if (CPU.REGs[A] < CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x0F:
    if (CPU.debug == true) { printf("CMP>: "); }    //|+|+|-|0x-------|if A > B: IC=IC+1(expected JMP) else IC=IC+2                                                |
    if (CPU.REGs[A] > CPU.REGs[B]) { if (CPU.debug == true) {  printf("True\n");} } else { if (CPU.debug == true) { printf("False\n");} CPU.IP = CPU.IP + 6; }
    break;
   case 0x10:
    if (CPU.debug == true) { printf("RAMPOS\n"); }  //|+|+|/|0xXXXXXXX|RAMPointer = IMM                                                                            |
    CPU.RP = IMM;
    break;
   case 0x11:
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
   case 0x12:
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
   case 0x13:
    if (CPU.debug == true) { printf("RSAV\n"); }    //|+|+|+|0x-------|C = SAV.data[A..B]                                                                          |
    CPU.REGs[C] = SAV.data[(CPU.REGs[A] << 16) | CPU.REGs[B]];
    break;
   case 0x14:
    if (CPU.debug == true) { printf("IRSAV\n"); }   //|+|-|-|0x-XXXXXX|A = SAV.data[IMM]                                                                           |
    CPU.REGs[A] = (SAV.data[IMM] % 0x1000000);
    break;
   case 0x15:
    if (CPU.debug == true) { printf("WSAV\n"); }    //|+|-|-|0x-------|SAV.data[A..B] = C                                                                          |
    SAV.data[(CPU.REGs[A] << 16) | CPU.REGs[B]] = CPU.REGs[C];
    break;
   case 0x16:
    if (CPU.debug == true) { printf("IWSAV\n"); }   //|+|+|+|0x-XXXXXX|SAV.data[IMM] = A                                                                           |
    SAV.data[IMM] = (CPU.REGs[A] % 0x1000000);
    break;
   case 0x17:
    if (CPU.debug == true) { printf("RROM\n"); }    //|+|+|+|0x-------|C = ROM.data[A..B]                                                                          |
    CPU.REGs[C] = ROM.data[(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B]) % CPU.ISz];
//    printf(">>>>>>  Adr:0x%x/%d, data:0x%x (ReadROM)  <<<<<<\n",(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B])%CPU.ISz,(((CPU.REGs[A] << 16) % 0x1000) | CPU.REGs[B])%CPU.ISz, CPU.REGs[C]);
    break;
   case 0x18:
    if (CPU.debug == true) { printf("IRROM\n"); }   //|+|-|-|0x-XXXXXX|A = ROM.data[IMM]                                                                           |
    CPU.REGs[A] = ROM.data[IMM % CPU.ISz];
//    printf(">>>>>>  Adr:0x%x/%d, data:0x%x (ImmReadROM)  <<<<<<\n",IMM % CPU.ISz,IMM % CPU.ISz, CPU.REGs[A]);
    break;
   case 0x19:
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
   case 0x1a:
    if (CPU.debug == true) { printf("PRINT\n"); }   //|+|-|-|0x-------|prints A in emulator terminal(for debugging ROMs only)                                      |
    if (CPU.REGs[A] > 9999) {
     printf(">> %d\t(0x%x)\n",CPU.REGs[A],CPU.REGs[A]);
    } else {
     printf(">> %d\t\t(0x%x)\n",CPU.REGs[A],CPU.REGs[A]);
    } break;
   case 0x1b:
    if (CPU.debug == true) { printf("FLAGS\n"); }   //|+|-|-|0x------X|A = Flags[IMM]                                                                              |
    CPU.REGs[A] = CPU.flag[IMM];
    break;
   case 0x1c:
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
   case 0x1d:
    if (CPU.debug == true) { printf("DVCRECV\n"); } //|+|-|-|0x----XXX|same as DVCSEND but recive then send                                                        |
    break;
   case 0x1e:
    if (CPU.debug == true) { printf("ICOUT\n"); }   //|+|+|-|0x-------|A,B = IC(32-bit for 24-bit)                                                                 |
    CPU.REGs[A] = CPU.IP & 0xFF;
    CPU.REGs[B] = CPU.IP >> 16;
    break;
   case 0x1f:
    if (CPU.debug == true) { printf("COPY\n"); }    //|+|+|-|0x-------|A = B                                                                                       |
    CPU.REGs[B] = CPU.REGs[A];
    break;
   case 0x20:
    if (CPU.debug == true) { printf("EXECUTE\n"); } //|-|-|-|0xXXXXXXX|execute location = IMM[6] and IMM[0-5]                                                      |
    printf("EMU NOTICE: "); CPU.IP = IMM-6;
    switch(A) {
     case 0x0: //executes BIOS
      printf("Switching to BIOS execute");
      CPU.IS  = BIOS.data;
      CPU.ISz = BIOS.size;
      currentexeccontext = BIOSCTX;
      break;
     case 0x1: //executes ROM
      if(stopatloadrom){
        printf("loading rom...\n");
        CPU.debug = true;
        getchar();
      } 
      printf("Switching to ROM execute");
      CPU.IS  = ROM.data;
      CPU.ISz = ROM.size;
      currentexeccontext = ROMCTX;
      break;
     case 0x2: //executes RAM
      printf("Switching to RAM execute");
      CPU.IS  = RAM;
      CPU.ISz = sizeof(RAM);
      currentexeccontext = RAMCTX;
      break;
    } if (CPU.debug == true) { printf(" at 0x%x/%x...\n",CPU.IP+6,CPU.ISz); } else { printf("...\n"); }
    break;
   case 0x21:
    if (CPU.debug == true) { printf("RBIOS\n"); }    //|+|+|+|0x-------|C = BIOS.data[A..B]                                                                         |
    CPU.REGs[C] = BIOS.data[(CPU.REGs[A] << 16) | CPU.REGs[B]];
    break;
   case 0x22:
    if (CPU.debug == true) { printf("POP "); }
    if(CPU.SP+1 < CPU.BP){
      CPU.SP+=2;
    }else{
      printf("PANIC! stack empty\n");
      exit(1);
    }
    CPU.REGs[A] = (uint16_t)RAM[CPU.SP] | (((uint16_t)RAM[CPU.SP-1])<<8);
    if (CPU.debug == true) { printf("%i\n",CPU.REGs[A]);}
    break;
   case 0x23:
    if (CPU.debug == true) { printf("PUSHI "); }
    RAM[CPU.SP] = (uint8_t)(IMM & 0xff);
    CPU.SP--;
    RAM[CPU.SP] = (uint8_t)(IMM >> 8);
    CPU.SP--;    
    if (CPU.debug == true) { printf("%i\n",IMM); }
    break;
   case 0x24:
    if (CPU.debug == true) { printf("PUSH "); }
    RAM[CPU.SP] = (uint8_t)(CPU.REGs[A] & 0xff);
    CPU.SP--;
    RAM[CPU.SP] = (uint8_t)(CPU.REGs[A] >> 8);
    CPU.SP--;
    if (CPU.debug == true) { printf("%i\n",CPU.REGs[A]); }
    break;
   case 0x25:
    if (CPU.debug == true) { printf("CALL "); }
    RAM[CPU.SP] = (uint8_t)(CPU.IP & 0xff);
    CPU.SP--;
    RAM[CPU.SP] = (uint8_t)((CPU.IP) >> 8);
    CPU.SP--;
    if (CPU.debug == true) { printf("%i\n",CPU.IP);}
    CPU.IP = (IMM % 0x1000000)-6;
    break;
   case 0x26:
    if (CPU.debug == true) { printf("RET "); }
    if(CPU.SP+1 < CPU.BP){
      CPU.SP+=2;
    }else{
      printf("PANIC! stack empty\n");
      exit(1);
    }
    CPU.IP = (uint16_t)RAM[CPU.SP] | (((uint16_t)RAM[CPU.SP-1])<<8);
    if (CPU.debug == true) { printf("%i\n",CPU.IP);}
    break;
   case 0x27:
    if (CPU.debug == true) { printf("SWAPTOP "); }
    if(CPU.SP+1 < CPU.BP){
      CPU.SP+=2;
    }else{
      printf("PANIC! stack empty\n");
      exit(1);
    }
    uint16_t tmp1 = (uint16_t)RAM[CPU.SP] | (((uint16_t)RAM[CPU.SP-1])<<8);
    if(CPU.SP+1 < CPU.BP){
      CPU.SP+=2;
    }else{
      printf("PANIC! stack empty\n");
      exit(1);
    }
    uint16_t tmp2 = (uint16_t)RAM[CPU.SP] | (((uint16_t)RAM[CPU.SP-1])<<8);
    RAM[CPU.SP] = (uint8_t)(tmp1 & 0xff);
    CPU.SP--;
    RAM[CPU.SP] = (uint8_t)(tmp1 >> 8);
    CPU.SP--;    
    RAM[CPU.SP] = (uint8_t)(tmp2 & 0xff);
    CPU.SP--;
    RAM[CPU.SP] = (uint8_t)(tmp2 >> 8);
    CPU.SP--;  
    break;
   case 0x28:
    if (CPU.debug == true) { printf("INT 0x%04x",IMM); }
    if (IMM > INTSIZ){
      printf("ERROR INT NUMBER LARGER THAN INT VECTOR!\n");
      exit(1);
    }
    interrupt i = InterruptVector[IMM];
    returnval = (interrupt){currentexeccontext,CPU.IP};
    switch(i.context){
     case 0x0: //executes BIOS
      CPU.IS  = BIOS.data;
      CPU.ISz = BIOS.size;
      currentexeccontext = BIOSCTX;
      break;
     case 0x1: //executes ROM
      CPU.IS  = ROM.data;
      CPU.ISz = ROM.size;
      currentexeccontext = ROMCTX;
      break;
     case 0x2: //executes RAM
      CPU.IS  = RAM;
      CPU.ISz = sizeof(RAM);
      currentexeccontext = RAMCTX;
      break;
    }
    CPU.IP = (i.adress % 0x1000000)-6;

    break;
   case 0x29:
    if (CPU.debug == true) { printf("SETINT 0x%04x -- > 0x%04x",IMM,CPU.REGs[A]); } 
    int addr = IMM;
    int position = CPU.REGs[A];
    char context = currentexeccontext;
    if (position > INTSIZ){
      printf("ERROR INT NUMBER LARGER THAN INT VECTOR!\n");
      exit(1);
    }
    InterruptVector[position] = (interrupt){context,addr};
    break;
   case 0x2A:
    if (CPU.debug == true) { printf("INTEND 0x%04x",IMM);}
    switch(returnval.context){
     case 0x0: //executes BIOS
      CPU.IS  = BIOS.data;
      CPU.ISz = BIOS.size;
      currentexeccontext = BIOSCTX;
      break;
     case 0x1: //executes ROM
      CPU.IS  = ROM.data;
      CPU.ISz = ROM.size;
      currentexeccontext = ROMCTX;
      break;
     case 0x2: //executes RAM
      CPU.IS  = RAM;
      CPU.ISz = sizeof(RAM);
      currentexeccontext = RAMCTX;
      break;
    }
    CPU.IP = (returnval.adress % 0x1000000);
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
 printf("EMU Notice: Screen Closed...\n");
 printf("\nEMU: We have detected the main loop has stopped due to a halt...\n\nThis is due to a halt (meaning the program has ended) or is a Emulation Error.\nIf there is a Emulation Error check above for what the problem is in the ROM.\n");
 //printf("The screen isn't active but is can still be viewed, to reset the emulator you need to goto your\n terminal and press [CTRL] + [C] and relaunch.");
 //printf("TIP: pressing [CTRL] + [M] will list RAM from RenderRAMPOS + 0x0000 to RenderRAMPOS + 0x02FF\n same with VideoRAM by pressing [CTRL] + [V]...\n and pressing [CTRL] + [KeyPad+] or [CTRL] + [KeyPad-] will adjust RenderRAMPOS by 0x10");
 printf("\nTotal ran instructions: %d\n\n",CPU.TI);
 return 0;
}

//right val nibble = byte & 0xF;
//left  val nibble = byte >> 4;
