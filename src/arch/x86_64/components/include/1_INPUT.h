//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#  INPUT COMPONENT FILE   #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#ifndef INPUT_H
#define INPUT_H

uint16_t controller[2];
void System_Error( int Err, int Inst, int IP, int ID, char Name[]);

void INPUT_send(int inst, int arg);
int  INPUT_recv(int inst);


#endif
