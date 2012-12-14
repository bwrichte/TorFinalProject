/* -------------------------------------------------------------------------
 * File: server.c
 * Author: Bryan Richter (bwrichte)
 * Description: Implements a server program which accepts sequential
 * connections from different client programs and receives data from them,
 * echoing all data received to stdout.
 * -------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define MIN_PORT 1
#define MAX_PORT 65535
#define BUFFER_SIZE 4096
#define MAX_PENDING 15
#define TRUE 1

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

/* Helper to read data from the client socket connection and echo all the data
   to stdout. Note: this helper could be used in multithreadedness. */
static void handleConnection(int client_socket)
{
    char buffer[BUFFER_SIZE+1];
    int read;
    
    LOG("Beginning Reading from client socket %d\n", client_socket);
    while ((read = recv(client_socket, buffer, BUFFER_SIZE, 0 /*no flags*/)) > 0)
    {
    	LOG("Read %d bytes\n", read);
    	/* ensure the buffer is null terminated to avoid overrun */
        buffer[read] = '\0';
        fprintf(stdout, "%s", buffer);
    }
    
    if (read < 0)
        printErrorAndExit("server: recv");
    
    LOG("Closing Client Socket %d\n", client_socket);
    close(client_socket);
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr, client_addr;
    int port, sockfd, client_socket;
    socklen_t addr_len;
    
    if (argc != 2)
    {
        fprintf(stderr, "usage: server <port>\n");
        exit(1);
    }
    
    /* parse port number */
    LOG("%s", "Parsing Port Number\n");
    port = atoi(argv[1]);
    if (port < MIN_PORT || port > MAX_PORT)
    {
        fprintf(stderr, "server: Invalid Port Number\n");
        exit(1);
    }
    
    /* build address data structure */
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    /* setup passive open */
    LOG("%s", "Opening new socket\n");
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0 /*default protocol*/)) < 0)
        printErrorAndExit("server: socket");
    LOG("Passively binding socket %d\n", sockfd);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        printErrorAndExit("server: bind");
    
    LOG("Listening for connections on port %d\n", port);
    listen(sockfd, MAX_PENDING);
    
    /* wait for connection, then receive and handle */
    while (TRUE)
    {
        addr_len = sizeof(client_addr);
        if ((client_socket = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
            printErrorAndExit("server: accept");
        
        LOG("Accepted a new connection from client addr %s\n", inet_ntoa(client_addr.sin_addr));
        handleConnection(client_socket);
    }
    
    LOG("Closing server socket %d\n", sockfd);
    close(sockfd);
    
    return 0;
}

