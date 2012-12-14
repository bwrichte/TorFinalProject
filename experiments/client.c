/* -------------------------------------------------------------------------
 * File: client.c
 * Author: Bryan Richter (bwrichte)
 * Description: Implements a client program which opens a socket to a server
 * in order to send data read from stdin without respecting a response.
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MIN_PORT 1
#define MAX_PORT 65535
#define BUFFER_SIZE 4096

#ifdef TRANSCRIPT
#include <arpa/inet.h>
#define MAX_LOG_LINE 255
static int line = 0;
static char *log_format = "Log <%d>: ";
static char log_line[MAX_LOG_LINE+10];
#define LOG(format, arg) \
	strcpy(log_line, log_format); \
	strncat(log_line, (format), MAX_LOG_LINE); \
	fprintf(stdout, log_line, line++, (arg))
#else
#define LOG(format, arg)
#endif

/* Helper to print a provided error message and the cause as determined by
   errno to stderr before exiting with a failure indicator of 1. */
static void printErrorAndExit(const char *errMsg)
{
    perror(errMsg);
    exit(1);
}

/* Helper to read data from the provided file and write it / send it across the 
   provided socket file descriptor. */
static void readAndSendData(FILE *file, int sockfd)
{
	char buffer[BUFFER_SIZE+1]; /* +1 to allow for null byte at end */
	int len, sent;
	
	/* main loop */
    LOG("Beginning Reading from file descriptor %d\n", fileno(file));
    while(fgets(buffer, BUFFER_SIZE+1, file) != NULL)
    {
        len = strlen(buffer);
        LOG("Read in %d bytes\n", len);
        
        sent = 0;
        do
        {
            sent += send(sockfd, buffer+sent, len-sent, 0);
            if (sent < 0)
                printErrorAndExit("client: send");
            LOG("Sent %d cumulative bytes\n", sent);
        }
        while (sent < len);
    }
    
    LOG("Closing socket %d\n", sockfd);
    close(sockfd);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
    int port, sockfd;
    
    if (argc != 3)
    {
        fprintf(stderr, "usage: client <server-IP-address> <port>\n");
        exit(1);
    }
    
    /* parse port number */
    LOG("%s", "Parsing Port Number\n");
    port = atoi(argv[2]);
    if (port < MIN_PORT || port > MAX_PORT)
    {
        fprintf(stderr, "client: Invalid Port Number\n");
        exit(1);
    }
    
    /* build address data structure */
    LOG("%s", "Parsing Server IP Address\n");
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[1], &server_addr.sin_addr) == 0)
    {
		fprintf(stderr, "client: Malformed IP Address\n");
		exit(1);
	}    
    server_addr.sin_port = htons(port);
    
    /* active open */
    LOG("%s", "Opening new socket\n");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        printErrorAndExit("client: socket");
    
    /* connect */
    LOG("Connecting socket %d to server\n", sockfd);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        printErrorAndExit("client: connect");
    
    /* read and send data */
    readAndSendData(stdin, sockfd);    
    
    return 0;
}

