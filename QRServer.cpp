#include <stdio.h>          /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>       /* for atoi() and exit() */
#include <string.h>       /* for memset() */
#include <unistd.h>      /* for close() */
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/wait.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0
#define DEFAULT_PORT 2012   //sets the default port to 2012
#define DEFAULT_RATE 3      //sets the default rate to 3
#define DEFAULT_MAX_USER 3  //sets the default max user to 3
#define DEFAULT_TIME_OUT 80 //sets the default time out to 80
#define RCVBUFSIZE 50000 //50 kb for receiving

char* buffer = new char[RCVBUFSIZE];

void DieWithError(const char *errorMsg);     /* Error handling function */
//also communicate with client, diff errors so if statements

//void *getAddress(struct sockaddr *sa);

void HandleTCPClient(int clntSocket);       /* TCP client handling function */
//logic of decoding QR code and sending it back

int main(int argc, char *argv[])
{
    int opt = TRUE;
    int masterSocket, addressLength, newSocket, clntSocket[3], activity, i, valRead, sd;
    int maxSd;
    int port = DEFAULT_PORT;
    int rate = DEFAULT_RATE;
    int maxUsers = DEFAULT_MAX_USER;
    int timeOut = DEFAULT_TIME_OUT;
    struct sockaddr_in address; //address info of socket
    char buffer[20481]; // 20kb buffer

    //set of socket descriptors
    fd_set readfds;

    //a message
    char *message = "Echo message \r\n";

    //check for correct number of arguments
    if (argc != 4){}
    fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]);
    exit(1);

    //initialise all clntSocket[] to 0 so not check
    for (i=0; i < mac_clients; i++){
        clntSocket[i] = 0;
    }

    //create a master socket for incoming connections
    if ((masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        DieWithError("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections
    if(setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0){
        DieWithError("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    memset(&address, 0, sizeof(address)); // zero out structure
    address.sin_family = AF_INET;   //internet address family
    address.sin_addr.s_addr = htonl(INADDR_ANY);   //any incoming interface
    address.sin_port = htons(port);     //local port

    //bind socket to local port/address
    if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address)) < 0){
        DieWithError("bind failed");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Listener on port %d \n", port);

    //mark the socket so it will listen for incoming connections
    if (listen(masterSocket, 0) < 0){
        DieWithError("listen");
        exit(EXIT_FAILURE);
    }

    //accept incoming connection
    while(TRUE){
        //set the size of the in-out param
        addressLength = sizeof(address);
        //wait for client to connect

        FD_ZERO(&readfds);  //clear the socket set

        //add master socket to set
        FD_SET(masterSocket, &readfds);
        maxSd = masterSocket;

        //add child sockets to set
        for (j = 0; j < maxUsers; j++){
            //socket descriptor
            sd - clntSocket[j];

            //if valid socket descriptor then add to read list
            if (sd > 0){
                FD_SET(sd, &readfds);
            }

            //highest file descriptor number, need ti for the select function
            if(sd > maxSd){
                masSd = sd;
            }

            //wait for activity for timeout
            activity = select(maxSd + 1, &readfds, NULL, NULL, timeOut);

            if((activity < 0) && (errno != EINTR)){
                printf("select error");
            }

            //if something is happening in the master socket, it's an incoming connection
            if(FD_ISSET(masterSocket, &readfds)){
                if((newSocket = accept(masterSocket, (struct sockaddr*)&address, (socklen_t*)&addressLength)) < 0){
                    DieWithError("accept");
                    exit(EXIT_FAILURE);
                }
                printf("Handling client %s\n", inet_ntoa(address.sin_addr));
                HandleTCPClient(newSocket);

                //send new connection greeting message
                if(send(newSocket, message, strlen(message), 0) != strlen(message)){
                    DieWithError("send");
                }

                //puts("welcome message sent successfully");

                //add new socket to array of sockets
                for (k = 0; k < maxUsers; k++){
                    //if position is empty
                    if(clntSocket[k] == 0){
                        clntSocket[k] = newSocket;
                        printf("adding to list of sockets as %d\n", k);
                        break;
                    }
                }
            }

            //else it's some IO operation on some other socket
            for(j = 0; j < maxUsers; j++) {
                sd = clntSocket[j];
                if (FD_ISSET(sd, &readfds)) {
                    //check if it was for closing, read incoming msg
                    if ((valRead = read(sd, buffer, 20480)) == 0) {
                        //somebody disconnected, get his details and print
                        getpeername(sd, (struct sockaddr *) &address, (socklen_t *) &addressLength);
                        printf("Host disconnected, ip: %s, port: %d \n", inet_ntoa(address.sin_addr),
                               ntohs(address.sin_port));
                        //close the socket and mark 0 in list for reuse
                        close(sd);
                        clntSocket[j] = 0;
                    }
                        //echo msg that came in
                    else {
                        //set string  terminating NULL byte on the end of teh data read
                        buffer[valRead] = '\0';
                        send(sd, buffer, strlen(buffer), 0);
                    }
                }
            }
        }
        return 0;
    }


void DieWithError(const char *errorMsg){
    printf("%s error", errorMsg);
}

void HandleTCPClient(int clientSocket){
    fprintf(stderr, "client socket is in HandleTCPClient\n");

}

void *getAddress(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
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
