#---------------------------------------------------------------
# File: Makefile
# Author: Bryan Richter (bwrichte)
# Description: Makefile for Assignment 0
#---------------------------------------------------------------

CC = g++

# Compiler flags
CCOPTS = -Wall -pedantic 
LOG_FLAG = -DTRANSCRIPT

# Assignment Parameters
COURSE = cos561
ASSIGNMENT = proj
USER = brsx

# Executable Names
SERVER = server
CLIENT = client
RELAY = relay

# Makefile targets
default: regular

regular: serverRegular clientRegular relayRegular

logged: serverLogged clientLogged

serverRegular: server.o
	$(CC) $(CCOPTS) server.o -o $(SERVER)

serverLogged: serverLogged.o
	$(CC) $(CCOPTS) serverLogged.o -o $(SERVER)

clientLogged: clientLogged.o
	$(CC) $(CCOPTS) clientLogged.o -o $(CLIENT)

clientRegular: client.o
	$(CC) $(CCOPTS) client.o -o $(CLIENT)

relayRegular: relay.o circbuf.o monitor.o
	$(CC) $(CCOPTS) relay.o circbuf.o monitor.o -o $(RELAY) -lpthread

server.o: server.c
	$(CC) $(CCOPTS) -c server.c -o server.o

serverLogged.o: server.c
	$(CC) $(CCOPTS) $(LOG_FLAG) -c server.c -o serverLogged.o

# POSIX_SOURCE is used to overcome bug in gcc warnings related to fileno
clientLogged.o: client.c 
	$(CC) $(CCOPTS) $(LOG_FLAG) -D_POSIX_SOURCE -c client.c -o clientLogged.o

client.o: client.c
	$(CC) $(CCOPTS) -c client.c -o client.o

relay.o: relay.c 
	$(CC) $(CCOPTS) -c relay.c -o relay.o

circbuf.o: circbuf.c circbuf.h
	$(CC) $(CCOPTS) -c circbuf.c -o circbuf.o

monitor.o: monitor.c monitor.h
	$(CC) $(CCOPTS) -c monitor.c -o monitor.o

# Clean up!
clean:
	rm -f *~
	rm -f *.o
	rm -f $(SERVER) $(CLIENT) $(RELAY)
	rm -f $(COURSE)_$(ASSIGNMENT)_$(USER).tgz

# How to compile a C file
%.o:%.c
	$(CC) $(CCOPTS) $<

# make the project ito a tarball
tar: client.c server.c Makefile
	tar -cvzf $(COURSE)_$(ASSIGNMENT)_$(USER).tgz client.c server.c Makefile


