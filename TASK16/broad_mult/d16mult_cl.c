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
  int server_sock_fd;

int CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
  //if(client_sock_fd)shutdown(client_sock_fd,2);
  //if(client_sock_fd)close(client_sock_fd); 
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
  if (inet_pton(AF_INET, "224.0.0.1", &sockaddr_in_server.sin_addr)<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  sockaddr_in_server.sin_port = htons(PORT); // адрес сервера что забиндили
  

 
  CheckError(client_sock_fd = socket(AF_INET,SOCK_DGRAM , 0), "socket",__LINE__);   //  1.  (SOCK_DGRAM 0) - udp,  (SOCK_STREAM 0) - tcp
  
      const int enable = 1;
CheckError(setsockopt(client_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)),"setsockport",__LINE__); 
 
Fill_ip(sockaddr_in_server, ip, __LINE__);
CheckError(bind(client_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  printf("NETWORK TCP Клиент запущен и устанавливает соединение с сервером по  сокету: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));
  
    /*
struct  ip_mreq {
    struct in_addr imr_multiaddr; // * многоадресная группа для присоединения 
    struct in_addr imr_interface; // * интерфейс для присоединения 
}*/
  
  
   struct ip_mreq mreq;   //структура для setsockopt
   
   if (inet_aton("224.0.0.1", &mreq.imr_multiaddr) < 0) {
    perror("inet_aton");
    return 1;
  }
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  CheckError(setsockopt(client_sock_fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)),"setsock opt IP_ADD_MEMBERSHIP ",__LINE__);
   
  

  CheckError(recvfrom(client_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0,(struct sockaddr *)&sockaddr_in_server, plen), "recvfrom",__LINE__); 

  
 // CheckError(getpeername(client_sock_fd, (struct sockaddr *)&sockaddr_in_server, plen),"getsockname", __LINE__); // заполним структуру данными того конца
  getsockname(client_sock_fd,(struct sockaddr *)&sockaddr_in_client2,plen);
  
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  CheckError(printf("SERV MSG: %s\n",(char *)msg_recv), "printf", __LINE__);
  
  
  //
  //Fill_ip(sockaddr_in_client2, ip, __LINE__);
  //CheckError(printf("MY PATH: %s %d \n", ip, ntohs(sockaddr_in_client2.sin_port)), "printf", __LINE__);
  
  //CheckError(printf("MY PATH (getsockname): %s\n", sockaddr_un_client2.sun_path), "printf", __LINE__);

  CheckError(close(client_sock_fd), "close", __LINE__);


  return 0;
}
