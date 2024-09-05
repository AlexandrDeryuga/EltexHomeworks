#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#define STR_SIZE_MAX 255
#define DATAGRAM_LEN 100//4096
#define PORT 8080


int main() {
 
  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно

 
int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

char recvbuf[DATAGRAM_LEN];

struct sockaddr_in saddr;
/*
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT); // random client port
	
	if (inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) != 1)
	{
		printf("source IP configuration failed\n");
		return 1;
	}
*/
printf("NETWORK RAW Клиент запущен и слушает   сокет:\n");


  while(1){
   
   int received = recvfrom(sock, recvbuf, sizeof(recvbuf),     0, (struct sockaddr *)&saddr, plen);
   if(received>0){
    printf("[%d байтов]получено, см от заголовка:%s\n",received,recvbuf);
    printf("[%d байтов]получено, см от сообщения:%s\n",received,(recvbuf+28));}
     
  }

  CheckError(close(sock), "close", __LINE__);

  
  return 0;
  
  
}
