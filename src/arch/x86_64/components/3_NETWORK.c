//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#  Network COMPONENT FILE #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

/*
sock = socket(AF_INET, SOCK_STREAM, 0);
new_socket = accept(int sock, struct sockaddr *addr, socklen_t *addrlen);



char name3[] = "NETWORK";

void Network_send(int inst, int arg) {
 if (sock == -1) {
  return NULL;
 }
 // printf("NETWORK: inst:%d\t,arg:%d\n",inst,arg);
 switch(inst) {
  case 0x00:// | SET IP[*]  |  +  |
   if (CPU.debug == true) { printf("NETWORK: Setting IP to 0x%x\n", arg); }
   GPU_x = arg;
   break;
  case 0x01:// |  SET PORT  |  +  |
   if (CPU.debug == true) { printf("NETWORK: Setting PORT to 0x%x\n", arg); }
   GPU_y = arg;
   break;
  case 0x02:// |  CONNECT   |  +  |
   if (CPU.debug == true) { printf("NETWORK: Connecting to Server [%d.%d.%d.%d:%d]\n", arg); }
   GPU_r = arg;
   break;
  case 0x03:// |    BIND    |  +  |
   if (CPU.debug == true) { printf("NETWORK: Binding to Server [%d.%d.%d.%d:%d]\n", arg); }
   GPU_g = arg;
   break;
  case 0x04:// |  SEND DATA |  +  |
   if (CPU.debug == true) { printf("GPU: Setting GPU.B to 0x%x\n", arg); }
   GPU_b = arg;
   break;
  case 0x05:// | DISCONNECT |  +  |
   arg = arg%0x10;
   if (CPU.debug == true) { printf("GPU: Setting GPU.CP[0x%x\t] to [R:0x%x\t,G:0x%x\t,B:0x%x\t]\n",arg,GPU_r,GPU_g,GPU_b); }
   GPU_cp[arg][0] = GPU_r;
   GPU_cp[arg][1] = GPU_g;
   GPU_cp[arg][2] = GPU_b;
   break;
  case 0x06:// |   plot     |  +  |
   arg = arg%0x20;
   if (arg > 0xF) {
    if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.REGs (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n", GPU_x, GPU_y, GPU_r, GPU_g, GPU_b); }
    buffer[GPU_x][GPU_y][0] = GPU_r;
    buffer[GPU_x][GPU_y][1] = GPU_g;
    buffer[GPU_x][GPU_y][2] = GPU_b;
   } else if (arg == 0x0) {
    if (CPU.debug == true) { printf("GPU: Skiping pixel plot at [X:0x%x\t,Y:0x%x\t]\n", GPU_x, GPU_y); }
   } else {
    if (CPU.debug == true) { printf("GPU: Plotting pixel at [X:0x%x\t,Y:0x%x\t] as GPU.CP[0x%x\t] (R:0x%x\t,G:0x%x\t,B:0x%x\t)\n",GPU_x, GPU_y, arg, GPU_cp[arg][0], GPU_cp[arg][1], GPU_cp[arg][2]); }
    buffer[GPU_x][GPU_y][0] = GPU_cp[arg][0]; buffer[GPU_x][GPU_y][1] = GPU_cp[arg][1]; buffer[GPU_x][GPU_y][2] = GPU_cp[arg][2];
   }
   break;
  case 0x07:// |   update   |  -  |
   GPU.inst = 0x07;
   GPU.run  = true;
   break;
  default:// nothing
   System_Error(0, inst, CPU.IP, 0, name3);
   break;
 }
}

int  GPU_recv(int inst) {
 if (sock == -1) {
  return 0;
 }
 switch(inst) {
  case 0x00:// |  GET W     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Width"); }
   return SW;
   break;
  case 0x01:// |  GET H     |  +  |
   if (CPU.debug == true) { printf("GPU: Returning Screen Hight"); }
   return SH;
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
   System_Error(0, inst, CPU.IP, 0, name3);
   break;
 }
 return 0;
}

addr.sin_port = htons(Network_port);
addr.sin_addr.s_addr = 0;
addr.sin_addr.s_addr = INADDR_ANY;
addr.sin_family = AF_INET;
if(bind(sock, (struct sockaddr *)&addr,sizeof(struct sockaddr_in) ) == -1) {
 printf("Error binding socket\n");
 return -1;
}
printf("Successfully bound to port %u\n", PORT);
*/
