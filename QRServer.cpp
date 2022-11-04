#include <stdio.h>          /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>       /* for atoi() and exit() */
#include <string.h>       /* for memset() */
#include <unistd.h>      /* for close() */
#include <iostream>

#define DEFAULT_PORT 2012   //sets the default port to 2012
#define DEFAULT_RATE 3      //sets the default rate to 3
#define DEFAULT_MAX_USER 3  //sets the default max user to 3
#define DEFAULT_TIME_OUT 80 //sets the default time out to 80
#define RCVBUFSIZE 50000
#define MAX_AMOUNT 4096

char* rcvBuffer = new char[RCVBUFSIZE];

void DieWithError(const char *errorMsg);     /* Error handling function */
//also communicate with client, diff errors so if statements

void HandleTCPClient(int clntSocket);       /* TCP client handling function */
//logic of decoding QR code and sending it back

//while loop with geopt(), check for what is specified, moves to end until null is returned

int main(int argc, char *argv[])
{    int servSock;                                   /*Socket descriptor for server */
     int clntSock;                                   /* Socket descriptor for client */
     struct sockaddr_in echoServAddr; /* Local address */
     struct sockaddr_in echoClntAddr; /* Client address */
     unsigned short echoServPort;        /* Server port */
     unsigned int clntLen;                     /* Length of client address data structure */
     int port = DEFAULT_PORT;
     int rate = DEFAULT_RATE;
     int maxUsers = DEFAULT_MAX_USER;
     int timeOut = DEFAULT_TIME_OUT;
     int opt;

      if (argc > 4)     /* Test for correct number of arguments */
     { 
           fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
            exit(1);
     }

     while((opt = getopt(argc, argv, "P:R:M:T:")) != -1){
         switch (opt) {
             case 'P':
                 port = atoi(optarg);
                 printf("port is set to %d\n", port);
                 break;
             case 'R':
                 rate = atoi(optarg);
                 printf("rate is set to %d\n", rate);
                 break;
             case 'M':
                 maxUsers = atoi(optarg);
                 printf("max users is set to %d\n", maxUsers);
             case 'T':
                 timeOut = atoi(optarg);
                 printf("time out is set to %d\n", timeOut);
             default:
                 printf("incorrectly formatted, you smell\n");
                 break;
         }

     }
     echoServPort = atoi(argv[1]);        /* First arg:  local port */
	 //change to correct port instead of argv[1]
	 
    /* Create socket for incoming connections */
      if ((servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
     	DieWithError("socket() failed");

/* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));         /* Zero out structure */
    echoServAddr.sin_family         = AF_INET;                      /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port             = htons(echoServPort);     /* Local port */
    
    /* Bind to the local address */
   if (bind (servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
	DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen (servSock, DEFAULT_MAX_USER) < 0)
    	DieWithError("listen() failed");
	
	/* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));         /* Zero out structure */
    echoServAddr.sin_family         = AF_INET;                      /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port             = htons(echoServPort);     /* Local port */ //usually 80 but OS don't let everyone use port 80
    
    /* Bind to the local address */
   if (bind (servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
	DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen (servSock, maxUsers) < 0)
    	DieWithError("listen() failed");

	for (;;) /* Run forever */
    {

        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);        /* Wait for a client to connect */
        if ((clntSock = accept (servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */
        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
        HandleTCPClient(clntSock);


     }
     /* NOT REACHED */
    }

void DieWithError(const char *errorMsg){
    printf("%s error", errorMsg);
}

void HandleTCPClient(int clntSocket){
    while((n = read(clntSocket, *RCVBUFSIZE, MAX_AMOUNT - 1)) > 0){
        std::cout << "\n%s\n\n%s" << std::endl;
        if (RCVBUFSIZE[n-1] == '\n'){
            break;
        }

    }


}


//what else are we doing?
	/*
		log everything that's happening
		server gets a length of what they send
		create a buffer to hold the length of what they send
		loop until it receives all packets
			receive function, in socket programing
			keep track of what is received 
			
			
		read from binary file
			fopen("file.txt", rb);
		how big a transmitted file is
			get it with 32 bit, need to convert because it's in network form
			
	
	
	*/
