//###########################//
//###########################//
//#      THE GAME RAZER     #//
//#  Network COMPONENT FILE #//
//#     **DO NOT EDIT**     #//
//###########################//
//###########################//
//###########################//

#ifndef Network_H
#define Network_H
void System_Error( int Err, int Inst, int IP, int ID, char Name[]);

uint8_t  Network_buffer[64],Network_address[4];
uint16_t Network_port;
int sock, setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen), bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen), listen(int sockfd, int backlog), new_socket;



#endif

