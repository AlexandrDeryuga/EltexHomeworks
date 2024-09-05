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

/*   //ОПИСАНИЕ СТРУКТУРЫ!
typedef struct sockaddr_in {
#if ...
  short          sin_family;
#else
  ADDRESS_FAMILY sin_family;
#endif
  USHORT         sin_port;
  IN_ADDR        sin_addr;
  CHAR           sin_zero[8];
} SOCKADDR_IN, *PSOCKADDR_IN;

struct in_addr { // IP адрес
  union {
    struct {
      u_char s_b1;
      u_char s_b2;
      u_char s_b3;
      u_char s_b4;
    } S_un_b;
    struct {
      u_short s_w1;
      u_short s_w2;
    } S_un_w;
    u_long S_addr;
  } S_un;

*/

 struct sockaddr_in sockaddr_in_server,  sockaddr_in_client;
  int server_sock_fd, new_sock_fd; // new_sock_fd  это после акцепта, его должны использовать, см. активный и пассивный сокет
  
  
int CheckError(int err_n, char *err_str, int line) {          //Подсмотрел, понравилась конструкция
  if (err_n < 0) {   
  if(new_sock_fd)shutdown(new_sock_fd,2);
  if(server_sock_fd)shutdown(server_sock_fd,2);
  if(new_sock_fd) close(new_sock_fd);                            // добавим только  close на случай ошибок
  if(server_sock_fd)close(server_sock_fd);
   err(EXIT_FAILURE, "%s: %d", err_str, line);   // ну или как обычно perror можно использовать
  } 
  return err_n;
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
 
  char msg_recv_send[STR_SIZE_MAX] = {0};
 
  //int s;
  //unsigned char bufip[sizeof(struct in_addr)];
  char ip[20];

  
  server_sock_fd=0; new_sock_fd=0;

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen=&len;      //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем
  
  // переведем IP строку в число


  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = inet_addr("255.255.255.255"); // можно и так
  sockaddr_in_server.sin_port = htons(PORT);


  CheckError(server_sock_fd = socket(AF_INET,SOCK_DGRAM, 0), "socket", __LINE__);  //  1.  (SOCK_DGRAM 0) - udp,  (SOCK_STREAM 0) - tcp
  //CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  
  printf("NETWORK UDP Сервер запущен на сокете: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));
  /*
  Если многоадресная датаграмма отправляется в группу, к которой принадлежит сам отправляющий хост, копия датаграммы по умолчанию закольцовывается уровнем IP для локальной доставки. Другая опция сокета дает отправителю явный контроль над тем, будут ли закольцованы последующие датаграммы:

цикл u_char;
setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop))

где loop равен 0 для отключения loopback и 1 для включения loopback. Этот параметр обеспечивает выигрыш в производительности для приложений, которые имеют только один экземпляр на одном хосте, устраняя накладные расходы на получение собственных передач. Приложения, которые могут иметь более одного экземпляра на одном хосте или для которых отправитель не принадлежит к группе назначения, не должны использовать этот параметр.
*/
int flag=1;
setsockopt(server_sock_fd , SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

 
  // new_sock_fd=server_sock_fd;     // Получим сообщение и заполним структуру клиента, Для UDP нет new_sock_fd
   //CheckError(recvfrom(server_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0,(struct sockaddr *)&sockaddr_in_client, plen),"recv_from", __LINE__); 
        
     //  Fill_ip(sockaddr_in_client, ip, __LINE__);    
    
  //CheckError(printf("CLIENT PATH: %s %d\nCLIENT MSG: %s\n",ip,ntohs(sockaddr_in_client.sin_port) ,msg_recv_send), "printf", __LINE__);

  sprintf(msg_recv_send,"рассылка от сервера"); //4. видоизменяем полученное сообщение и отправляем клиентам

 //CheckError(sendto(client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0,(struct sockaddr *)&sockaddr_in_server,len), "send",__LINE__);  // пошлем серверу сообщение

  if(CheckError(sendto(server_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0,(struct sockaddr *)&sockaddr_in_server,len), "send", __LINE__)!=STR_SIZE_MAX)
      printf("Не все данные переданы\n");
   
      
  
  printf("рассылаю строку: ""%s""\n",(char *)msg_recv_send);
  
  CheckError(close(server_sock_fd), "close", __LINE__);


  return 0;
}



