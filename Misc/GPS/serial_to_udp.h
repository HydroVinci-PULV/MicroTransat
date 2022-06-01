#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>



using namespace std;

int UDPHandle;
int serialHandle;
struct sockaddr_in remoteAddr;
in_addr_t remoteIP;
int remotePort;
#define BUFSIZE 1024

uint8_t serialBuffer[BUFSIZE];
int serialBufferCnt;

int open_udp_socket(char *remoteHostname, int _remotePort);
void send_udp_data(int sockfd, uint8_t *buffer, int data_size);
void open_serial_sonnection(char *name, speed_t baudRate);
int read_serial_connection();