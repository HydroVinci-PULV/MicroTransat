#include "serial_to_udp.h"

// usage -S=<serial port> -B=<baud rate> -H=<remote host> -P<remote port>
main(int argc, char* argv[]){
    int c;
    char *serialPort = NULL;
    char *remoteHost = NULL;
    int remotePort = 0;
    int baudRate = 0;
    
    while ((c = getopt(argc, argv, "S:B:H:P:")) != -1) {
        switch (c) {
            case 'S':
                serialPort = optarg;
                break;
            case 'B':
                baudRate = atoi(optarg);
                break;
            case 'H':
                remoteHost = optarg;
                break;
            case 'P':
                remotePort = atoi(optarg);
                break;
            default:
                printf("Usage: %s -S=<serial port> -B=<baud rate> -H=<remote host> -P<remote port>\n", argv[0]);
                exit(1);
        }
    }
    
    if (serialPort == NULL || remoteHost == NULL || remotePort == 0 || baudRate == 0) {
        printf("Usage: %s -S=<serial port> -B=<baud rate> -H=<remote host> -P<remote port>\n", argv[0]);
        exit(1);
    }

    open_serial_sonnection(serialPort, baudRate);

    UDPHandle = open_udp_socket(remoteHost, remotePort);

    while (1) {
        int bytesRead = read_serial_connection();
        if (bytesRead > 0) {
            send_udp_data(UDPHandle, serialBuffer, bytesRead);
        }
    }
}

// Function that opens a UDP socket on a given port
int open_udp_socket(char *remoteHostname, int _remotePort) {
    int sockfd;
    struct hostent *he;
    struct sockaddr_in their_addr;
    int yes = 1;

    if ((he = gethostbyname(remoteHostname)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    remoteIP = inet_addr(remoteHostname);
    remotePort = _remotePort;

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(remotePort);
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    remoteAddr = their_addr;

    return sockfd;
}

// Function that sends binary data to a UDP socket
void send_udp_data(int sockfd, uint8_t *buffer, int data_size) {
    int bytesSent = sendto(sockfd, buffer, data_size, 0, (struct sockaddr *) &remoteAddr, sizeof (remoteAddr));
    if (bytesSent < 0) {
        perror("sendto");
        exit(1);
    }
}

// Function that opens a Serial connection
void open_serial_sonnection(char *name, speed_t baudRate)
{
	serialHandle = open(name, O_RDWR | O_NOCTTY | O_NDELAY);
	fcntl(serialHandle, F_SETFL, FNDELAY);

	struct termios options;
	/* get the current options */
	tcgetattr(serialHandle, &options);

	/* set raw input, 1 second timeout */
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 10;

	cfsetspeed(&options, baudRate);

	/* set the options */
	tcsetattr(serialHandle, TCSANOW, &options);
}

// Function to read from the serial connection
int read_serial_connection()
{
    serialBufferCnt = 0;

    if ((serialBufferCnt = read(serialHandle, serialBuffer, BUFSIZE)) == -1) {
        perror("read");
        exit(1);
    }

    return serialBufferCnt;
}