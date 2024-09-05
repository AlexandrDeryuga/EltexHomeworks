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
#define PORT 8080

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

int main() {
 

  char msg_send[STR_SIZE_MAX]; 
  char msg_recv[STR_SIZE_MAX];
  
  strcpy(msg_send,"Hi!!");
   strcpy(msg_recv,"init");
 //  int s;
 // unsigned char bufip[sizeof(struct in_addr)];
  char ip[20];

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем           0.подготовка структур
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем


  sockaddr_in_server.sin_family = AF_INET;
  if (inet_pton(AF_INET, "127.0.0.1", &sockaddr_in_server.sin_addr)<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  sockaddr_in_server.sin_port = htons(PORT); // адрес сервера что забиндили


  CheckError(client_sock_fd = socket(AF_INET,SOCK_STREAM , 0), "socket",__LINE__);   //  1.  (SOCK_DGRAM 0) - udp,  (SOCK_STREAM 0) - tcp
 
Fill_ip(sockaddr_in_server, ip, __LINE__);

  printf("(epoll) NETWORK TCP Клиент запущен и устанавливает соединение с сервером по  сокету: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));
 
  //client_sock_fd=client_fd;
  CheckError(connect(client_sock_fd, (struct sockaddr *)&sockaddr_in_server, sizeof(sockaddr_in_server)),"connect", __LINE__); // это в пару акцепту , указываем здесь len а не указатель. наш fd и серверный адрес


 
  printf("TCP Клиент>соединение установленно\n");
  
  while(1){
   printf("TCP клиент>Введите сообщение для сервера, ""exit"" - завершить программу\n");
   scanf("%s",msg_send);
   CheckError(send(client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
   if(strcmp(msg_send,"exit")==0) break;
  }


  CheckError(close(client_sock_fd), "close", __LINE__);


  return 0;
}
