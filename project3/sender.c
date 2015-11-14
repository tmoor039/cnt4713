#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int windowSize = 100;
int packetSize = 1024;
int timeout = 10;

void syserr(char* msg) { perror(msg); exit(-1); }
int receiveACK(int sockfd){
	/*
	char* buff;
	int n = recv(sockfd,buff,3,0);
	if(n<=0) syserr("lost connection to receiver");
	if(strcmp(buff,"ACK"))	return -1;
	*/
	return 0;
}

void printPacket(void* data,size_t length){
	char* d = (char*) data;
	size_t i = 0;
	for(i;i+1<length;i+=2){
		uint16_t word;
		memcpy(&word,d+i,2);
		printf("%x",word);
	}
	if(length%2!=0){
		uint16_t word;
		memcpy(&word,d+length-1,1);
		printf("%x",word);
	}
	printf("\n");
}

uint16_t calculateChecksum(void* data,size_t length){
	char* d = (char*) data;
	uint16_t sum = 0;
	size_t i = 0;
	for(i;i+1<length;i+=2){
		uint16_t word;
		memcpy(&word,d+i,2);
		sum+=word;
	}	
	if(length%2!=0){
		uint16_t word;
		memcpy(&word,d+length-1,1);
		sum+=word;
	}
	return sum;
}

int sendPacket(int sockfd,struct sockaddr* recv_addr, int seqNum,void *data,size_t data_size){
	char packet[sizeof(int)+sizeof(uint16_t)+data_size];
	uint16_t checksum;
	void *offset = packet;
	memset(packet,0,sizeof(packet));
	checksum = calculateChecksum(&seqNum,sizeof(int));
	checksum += calculateChecksum(data,data_size);
	printf("checksum: %x\n",checksum);
	checksum = 0xffff - checksum;
	memcpy(offset,&seqNum,sizeof(int));
	//printPacket(packet,sizeof(packet));
	offset+=sizeof(int);
	memcpy(offset,&checksum,sizeof(uint16_t));
	printf("\n");
	offset+=sizeof(uint16_t);
	memcpy(offset,data,data_size);
	printPacket(packet,sizeof(packet));
	//checksum = htons(checksum);
	//strcpy(packet,snum);
	//strcat(packet,cs);
	
	//printf("packet: %s\n",packet);
	//n = sendto(sockfd, , strlen(buffer), 0, (struct sockaddr*)&recv_addr, addrlen);
	uint16_t check = calculateChecksum(packet,sizeof(packet));
	printf("check: %x\n",check);
	return 1;
}	

int main(int argc, char* argv[])
{
 // int sockfd, portno, n;
 // struct hostent* receiver;
 // struct sockaddr_in recv_addr;
 // socklen_t addrlen;
	FILE *f;	
	char *filename;
	/*
  if(argc != 4) {
    fprintf(stderr, "Usage: %s <ip> <port> <file>\n", argv[0]);
    return 1;
  }
	*/
	//open file and check if it exists
	filename = argv[3];
	f = fopen(filename,"rb");
	if(f==NULL) syserr("cannot open file: make sure it exists");
	/*
	//check if receiver is a valid host
  receiver = gethostbyname(argv[1]);
  if(!receiver) {
    fprintf(stderr, "ERROR: no such host: %s\n", argv[1]);
    return 2;
  }
  portno = atoi(argv[2]);
  
  sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd < 0) syserr("can't open socket");

  memset(&recv_addr, 0, sizeof(recv_addr));
  recv_addr.sin_family = AF_INET;
  recv_addr.sin_addr = *((struct in_addr*)receiver->h_addr);
  recv_addr.sin_port = htons(portno);
	addrlen = sizeof(recv_addr);


 // n = recvfrom(sockfd, buffer, 255, 0, (struct sockaddr*)&recv_addr, &addrlen);

  close(sockfd);
	*/
	char buffer[packetSize];
	int n = fread(buffer,packetSize,1,f);
	if(n<=0) syserr("fuck\n");
	//printf("%d\n",calculateChecksum(buffer,1024));
	//printf("payload: %s\n",buffer);
	printPacket(buffer,packetSize);
	sendPacket(1,NULL,5,buffer,packetSize);
	fclose(f);
  return 0;
}
