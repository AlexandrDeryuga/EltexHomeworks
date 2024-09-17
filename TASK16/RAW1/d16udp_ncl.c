#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "net_headers.h"

#define STR_SIZE_MAX 255 
#define DATAGRAM_LEN 100//4096
#define PORT 8080
#define MYPORT 9999


/*Домашка RAW1 - создание заголовка*/

 //struct sockaddr_un sockaddr_un_server, sockaddr_un_client, sockaddr_un_client2;
 struct sockaddr_in sockaddr_in_server,  sockaddr_in_client, sockaddr_in_client2;
  int client_sock_fd;  // в клиенте accept не нужен, поэтому только один дескриптор

int CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
  if(client_sock_fd)shutdown(client_sock_fd,2);
  if(client_sock_fd)close(client_sock_fd); 
   err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
  return err_n;  // на случай если не выходим но  была не критическая ошибка
}

void Fill_ip(struct sockaddr_in saddr, char *ip, int line)
{
strcpy(ip,"error");
if (inet_ntop(AF_INET,&saddr.sin_addr.s_addr, ip, 20) == NULL) {
              perror("inet_ntop\n");
              err(EXIT_FAILURE, "Fill_ip: %d\n", line);
              
           }
}

/*
struct udphdr
{
  u_int16_t source;
  u_int16_t dest;
  u_int16_t len;
  u_int16_t check;
};
*/


int main() {
 
 struct udphdr s_udphdr, *p_udphdr=NULL; // заголовок
 unsigned char msg_send[STR_SIZE_MAX]; 
  unsigned char msg_recv[STR_SIZE_MAX];
  
 
  char ip[20];

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем           0.подготовка структур
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем

// RAW0 - создание
int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
char recvbuf[DATAGRAM_LEN];

struct sockaddr_in saddr;

printf("NETWORK RAW Клиент запущен и слушает   сокет %d:\n",raw_sock);


// Передача сообщения
   
   
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT); // random client port
	
	if (inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) != 1)
	{
		printf("source IP configuration failed\n");
		return 1;
	}



   p_udphdr=(struct udphdr *)msg_send;
   
   
   int hlen=sizeof(struct udphdr);
   sprintf(msg_send+hlen,"HELLO!");
   int sendlen=hlen+strlen(msg_send+hlen);
   
   p_udphdr->source=htons(MYPORT);
   p_udphdr->dest=htons(PORT);
   p_udphdr->len=htons(sendlen);
   p_udphdr->check=0;
   
   //for(int i=0;i<16;i++) printf("[%02d] %03d, (%c)\n",i,msg_send[i],msg_send[i]);
   
   
   
   
   printf("заголовок=%d байт, всего будет переданно %d байт \n",hlen,sendlen);
   int realsend= sendto(raw_sock, msg_send,sendlen ,     0, (struct sockaddr *)&saddr, len);
   //sendto(raw_sock, (char *)msg_send, STR_SIZE_MAX, 0,(struct sockaddr *)&sockaddr_in_server,len);  // пошлем серверу сообщение
   printf("сообщение передано (%d ,байт)\n",realsend);
   if(realsend==-1) perror("send to");

// Прием Сообщения
  p_udphdr=(struct udphdr *)msg_recv;
  int i=0;
 while(i<2){
 int received = recvfrom(raw_sock, msg_recv, sizeof(msg_recv),     0, (struct sockaddr *)&saddr, plen);
   if(received>0){
   
    printf("---------------------------------------------------\n");
    printf("[%d байтов]получено, (%s ....)\n",received,recvbuf);
    hlen=20;
    p_udphdr=(struct udphdr *)(msg_recv+20);  // отступили TCP заголовок
   /*
    printf("[Source port]=%d\n",ntohs(p_udphdr->source));
    printf("[Dest port]=%d\n",ntohs(p_udphdr->dest));
    printf("[len]=%d\n",ntohs(p_udphdr->len));
    printf("[Chek]=%d\n",ntohs(p_udphdr->check));
    */
    hlen+=8;//отступили UDP заголовок
    if(MYPORT==ntohs(p_udphdr->dest)) {printf("[сообщение от сервера] :%s\n",(msg_recv+hlen));break;};
    
  
    i++;
    
    }
     }



  return 0;
}
