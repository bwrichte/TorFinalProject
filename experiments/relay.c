#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "monitor.h"

#define MIN_PORT 1
#define MAX_PORT 65535
#define COPY_BUFFER_SIZE 512
#define MAX_PENDING 15
#define TRUE 1
#define FALSE 0

static Monitor monitor;

static int flag = FALSE;

/* Helper to print a provided error message and the cause as determined by
   errno to stderr before exiting with a failure indicator of 1. */
static void printErrorAndExit(const char *errMsg)
{
    perror(errMsg);
    exit(1);
}

static void *monitorBufferSize(void *data)
{
	(void) data;
	while (!flag)
	{
		double frac = CircBuffer_Size(&monitor.cb) / (double) BUFFER_SIZE;
		printf("%.2f\n", frac * 100);
		sleep(1);
	}
	double frac = CircBuffer_Size(&monitor.cb) / (double) BUFFER_SIZE;
	printf("%.2f\n", frac * 100);
	return NULL;
}

static void *handleClientConnection(void *data)
{
	char buffer[COPY_BUFFER_SIZE];
	int client_socket = (int) data;
	int read;
	
	while ((read = recv(client_socket, buffer, COPY_BUFFER_SIZE, 0 /*no flags*/)) > 0)
    {
        Monitor_AddBuffer(&monitor, buffer, read);
    }
    
    if (read < 0)
    {
        printErrorAndExit("relay: recv");
    }
    
    Monitor_CloseBuffer(&monitor);
    
    close(client_socket);
    return NULL;
}

static void *handleServerConnection(void *data)
{
	struct sockaddr_in * server_addr = (struct sockaddr_in *)data;
	char buffer[COPY_BUFFER_SIZE];
	int read, sent, sockfd;
		
	/* active open to server */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        printErrorAndExit("relay: socket");
    
    /* connect */
    if (connect(sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr_in)) < 0)
        printErrorAndExit("relay: connect");
    
    while ((read = Monitor_RemoveBuffer(&monitor, buffer, COPY_BUFFER_SIZE)) > 0)
    {		
		sent = 0;
		do
		{
		    sent += send(sockfd, buffer+sent, read-sent, 0);
		    if (sent < 0)
		        printErrorAndExit("relay: send");
		}
		while (sent < read);
	}
    
    close(sockfd);
    return NULL;
}

/* Helper to read data from the client socket connection and echo all the data
   to stdout. Note: this helper could be used in multithreadedness. */
static void handleConnection(int client_socket, struct sockaddr_in *server_addr)
{
	Monitor_Init(&monitor);
	pthread_t reader, writer, overseer;
	
    pthread_create(&reader, NULL, handleClientConnection, (void*) client_socket);
    pthread_create(&writer, NULL, handleServerConnection, (void*) server_addr);
    pthread_create(&overseer, NULL, monitorBufferSize, NULL);
    
    pthread_join(reader, NULL);
    pthread_join(writer, NULL);
    flag = TRUE;
    pthread_join(overseer, NULL);
    flag = FALSE;
}

int main(int argc, char *argv[])
{	
    struct sockaddr_in relay_addr, client_addr, server_addr;
    int in_port, out_port, sockfd, client_socket;
    socklen_t addr_len;
    
    if (argc != 4)
    {
        fprintf(stderr, "usage: relay <incoming_port> <server_ip_addr> <server_port>\n");
        exit(1);
    }
    
    /* parse port numbers */
    in_port = atoi(argv[1]);
    if (in_port < MIN_PORT || in_port > MAX_PORT)
    {
        fprintf(stderr, "relay: Invalid Incoming Port Number\n");
        exit(1);
    }
    
    out_port = atoi(argv[3]);
    if (out_port < MIN_PORT || out_port > MAX_PORT)
    {
        fprintf(stderr, "relay: Invalid Outgoing Port Number\n");
        exit(1);
    }
    
    /* build relay address data structure */
    memset((char *)&relay_addr, 0, sizeof(relay_addr));
    relay_addr.sin_family = AF_INET;
    relay_addr.sin_addr.s_addr = INADDR_ANY;
    relay_addr.sin_port = htons(in_port);
    
    /* build server address data structure */
    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, argv[2], &server_addr.sin_addr) == 0)
    {
		fprintf(stderr, "relay: Malformed Server IP Address\n");
		exit(1);
	}    
    server_addr.sin_port = htons(out_port);
    
    /* setup passive open */
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0 /*default protocol*/)) < 0)
        printErrorAndExit("relay: socket");
    if (bind(sockfd, (struct sockaddr *)&relay_addr, sizeof(relay_addr)) < 0)
        printErrorAndExit("relay: bind");
    
    listen(sockfd, MAX_PENDING);
    
    /* wait for connection, then receive and handle */
    while (TRUE)
    {
        addr_len = sizeof(client_addr);
        if ((client_socket = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
            printErrorAndExit("relay: accept");
        
        handleConnection(client_socket, &server_addr);
    }
    
    close(sockfd);   
    
    return 0;
}
