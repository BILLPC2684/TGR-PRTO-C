
#ifndef CONFIG_H
#define CONFIG_H

//INPUT Settings:
 //Player 1:
  #define input0_A        SDL_SCANCODE_Z            //KeyBoard[Z]
  #define input0_B        SDL_SCANCODE_X            //KeyBoard[X]
  #define input0_C        SDL_SCANCODE_C            //KeyBoard[C]
  #define input0_X        SDL_SCANCODE_A            //KeyBoard[A]
  #define input0_Y        SDL_SCANCODE_S            //KeyBoard[S]
  #define input0_Z        SDL_SCANCODE_D            //KeyBoard[D]
  #define input0_L        SDL_SCANCODE_Q            //KeyBoard[Q]
  #define input0_R        SDL_SCANCODE_W            //KeyBoard[W]
  #define input0_Start    SDL_SCANCODE_RETURN       //KeyBoard[<-/Enter]
  #define input0_Select   SDL_SCANCODE_BACKSPACE    //KeyBoard[<-BackSpace]
  #define input0_Up       SDL_SCANCODE_UP           //KeyBoard[^]
  #define input0_Down     SDL_SCANCODE_DOWN         //KeyBoard[v]
  #define input0_Left     SDL_SCANCODE_LEFT         //KeyBoard[<]
  #define input0_Right    SDL_SCANCODE_RIGHT        //KeyBoard[>]
 //Player 2:
  #define input1_A        SDL_SCANCODE_KP_1         //NumPad[1]
  #define input1_B        SDL_SCANCODE_KP_5         //NumPad[5]
  #define input1_C        SDL_SCANCODE_KP_3         //NumPad[3]
  #define input1_X        SDL_SCANCODE_KP_7         //NumPad[7]
  #define input1_Y        SDL_SCANCODE_KP_DIVIDE    //NumPad[/]
  #define input1_Z        SDL_SCANCODE_KP_9         //NumPad[9]
  #define input1_L        SDL_SCANCODE_KP_0         //NumPad[0]
  #define input1_R        SDL_SCANCODE_KP_PERIOD    //NumPad[.]
  #define input1_Start    SDL_SCANCODE_KP_ENTER     //NumPad|Enter|
  #define input1_Select   SDL_SCANCODE_KP_PLUS      //NumPad|+|
  #define input1_Up       SDL_SCANCODE_KP_8         //NumPad[8]
  #define input1_Down     SDL_SCANCODE_KP_2         //NumPad[2]
  #define input1_Left     SDL_SCANCODE_KP_4         //NumPad[4]
  #define input1_Right    SDL_SCANCODE_KP_6         //NumPad[6]

//Video Settings:
 #define video_LimmitFPS  60                        //Default: 60    |
 #define video_Zoom       1                         //Default: 1     | MAX: 4
 #define video_ScanLines  true                      //Default: false |
 #define video_showinput  false                     //Default: false |

//Emulation Settings:
 #define emulation_Speedx 1                         //Default: 1     | MAX: 8
 #define emulation_                                 //Default:       |
 #define emulation_                                 //Default:       |
 
#endif
