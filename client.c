/*UDP c AF_INIT*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include <arpa/inet.h>
#define ECHO_ADDR "127.0.0.1"

void errorExit(char err[]);

struct udpHeader{
    short int source;
    short int dest;
    short int len; 
    short int check;
};


void errorExit(char err[]){
    perror(err);
    exit(EXIT_FAILURE);
}


int main(){
    
    struct sockaddr_in serv;
    char msg[] = "Hello, server";
    char buff[100];
    struct iphdr ip;
    struct udpHeader header;
    int one = 1;
   
    //----------------------------------------------------
    int fd = socket(AF_INET,SOCK_RAW,IPPROTO_UDP);
    if(fd == -1)
        errorExit("soket");

    serv.sin_family = AF_INET;
    serv.sin_port = htons(9007);
    serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    //------------------------------------------
    if(setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) == -1)
        errorExit("sekopt");
    //------------------------------------------

    ip.ihl      = 5;
    ip.version  = 4;
    ip.tos      = 16;
    ip.tot_len  = 0;
    ip.id       = 0;
    ip.ttl      = 64;
    ip.protocol = 17;
    ip.saddr    = 0;
    ip.daddr    =  htonl(INADDR_LOOPBACK);
    ip.check    = 0;

  
    //----------------------------------------------------
    header.source = htons(7777);
    header.dest = htons(9007);
    header.len = htons(sizeof(msg) + sizeof(struct udpHeader));
    header.check = 0;
    //----------------------------------------------------

    memcpy((void *)buff, (void* )&ip, sizeof(ip));
    memcpy((void *)(buff + sizeof(struct iphdr)), (void* )&header, sizeof(header));
    memcpy((void*)(buff + sizeof(ip) + sizeof(header)), (void*)msg, sizeof(msg));

    socklen_t size = sizeof(serv);
    if(sendto(fd, buff, (sizeof(struct iphdr)+sizeof(struct udpHeader) + sizeof(msg)), 0, (struct sockaddr *)&serv, sizeof(serv)) == -1)
        errorExit("sendto");

    char out[100];
    
    while(1){
        if(recvfrom(fd, out, sizeof(out), 0, (struct sockaddr *)&serv, &size) == -1)
            errorExit("recvfrom");
        printf("%s\n", out + 28);
    }

    exit(EXIT_SUCCESS);

}