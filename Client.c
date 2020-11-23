#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "protocol.h"   

struct info info;


void DieWithError(char *errorMessage);  /* Error handling function */
void out(void);

void TCPWay(char *servIP){
    char getInfo[20] = "GET_SYSTEM_INFO";
    int sock;
    struct sockaddr_in echoServAddr;        /* Echo server address */

    /* Create a reliable, stream socket using TCP */
    if (( sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
    echoServAddr.sin_family      = AF_INET;             /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    echoServAddr.sin_port        = htons(port);         /* Server port */

    /* Establish the connection to the echo server */
    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");
    
    if (send(sock, &getInfo, sizeof(getInfo), 0) != sizeof(getInfo))
        DieWithError("send() sent a different number of bytes than expected");

    if ((recv(sock, &info, sizeof(info), 0)) < 0)
        DieWithError("recv() failed");
    
	out();  // вывод на экран

    close(sock);
}

void UDPWay(char *servIP){
    char getInfo[20] = "GET_SYSTEM_INFO";
    int echoStringLen, sock;               /* Length of string to echo */
    unsigned int fromSize;           /* In-out of address size for recvfrom() */
    struct sockaddr_in echoServAddr;        /* Echo server address */
    struct sockaddr_in fromAddr;     /* Source address of echo */

    /* Create a datagram/UDP socket */
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct the server address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));    /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                 /* Internet addr family */
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);  /* Server IP address */
    echoServAddr.sin_port   = htons(port);     /* Server port */

    /* Send the string to the server */
    echoStringLen = strlen(getInfo);
    if (sendto(sock, getInfo, echoStringLen, 0, (struct sockaddr *)
               &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
        DieWithError("sendto() sent a different number of bytes than expected");
  
    /* Recv a response */
    fromSize = sizeof(fromAddr);
    if (recvfrom(sock, &info, sizeof(info), 0, (struct sockaddr *) &fromAddr, &fromSize) < 0)
        DieWithError("recvfrom() failed");

    if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
    {
        fprintf(stderr,"Error: received a packet from unknown source.\n");
        exit(1);
    }

	out();      // вывод на экран

    close(sock);
    exit(0);
}

int checkIP(char buffer[]){
    int CountIp = 0;
    char* NotInt;
    int current; 
    int isIp = 1;
    int cnt =0; 
    char b[20];
    int IP[4]; 
    
    memset(b,0,20); 

    for (int i=0; i <= strlen(buffer); i++){
        if(buffer[i]=='.' || buffer[i] == 0){
            current =strtol(b,&NotInt,10); 
            if ((!*NotInt && CountIp < 4) && (current >=0 && current <256)){    
                IP[CountIp] = current; 
            } 
            else{ isIp = 0; break; }
            
            memset(b,0,20);
            cnt=0; 
            CountIp++;
        }
        else{ b[cnt++] = buffer[i]; }
    }

    if (isIp)return 1;
    else return 0;
}

int main(int argc, char *argv[]){
    if (argc != 4){    
       fprintf(stderr, "Usage: %s <Server IP> <Echo Port> <TCP/UDP>\n", argv[0]);
       exit(1);
    }

    // Парсинг и валидация аргументов 

    char *servIP = argv[1];       
    port = atoi(argv[2]);         
    type_proto = argv[3];

    // --------------------------

    if (!checkIP(servIP)){
        printf("Invalid IP\n");
        exit(1);
    }

    if(port < 1024 || port > 65535){
        printf("Invalid port\n");
        exit(1);
    }
    // ==========================

    if(strcmp(type_proto, "TCP")==0 || strcmp(type_proto, "tcp")==0) { 
        printf("TCPWay\n"); 
        TCPWay(servIP);
    }
    else if(strcmp(type_proto, "UDP")==0 || strcmp(type_proto, "udp")==0) { 
        printf("UDPWay\n"); 
        UDPWay(servIP);
    }
    else { printf("Invalid protocol type\n"); exit(1); }

    exit(0);
}
