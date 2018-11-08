//standerd libarys
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
//graphics libarys
#include <stdlib.h>
#include <SDL2/SDL.h>
//threads libary
#include <pthread.h>
//network libarys
#ifdef _WIN32
 /* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
 #ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0501  /* Windows XP. */
 #endif
 #include <winsock2.h>
 #include <Ws2tcpip.h>
#else
 /* Assume that any non-Windows platform uses POSIX-style sockets instead. */
 #include <sys/socket.h>
 #include <arpa/inet.h>
 #include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#endif

#ifndef TGR_H
#define TGR_H

//TGR libs

#include <Constants.h>
#include <main.h>
#include <util.h>
#include <config.h>

#include "#_CPU.h"
#include "0_GPU.h"
#include "1_INPUT.h"
#include "2_SOUND.h"
#include "3_NETWORK.h"

/*enable for additional modules
 #include <4_.h>
 #include <5_.h>
 #include <6_.h>
 #include <7_.h>
 #include <8_.h>
 #include <9_.h>
 #include <A_.h>
 #include <B_.h>
 #include <C_.h>
 #include <D_.h>
 #include <E_.h>
 #include <F_.h>
*/


#endif
