#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

 struct sockaddr_un sockaddr_un_server, sockaddr_un_client, sockaddr_un_client2;
  int client_sock_fd;  // в клиенте accept не нужен, поэтому только один дескриптор

int CheckError(int err_n, char *err_str, int line) {
  if(err_n != -1 ) return 1;  // давайте ка для общности будем возвращать значение, вдруг  в дальнейшем не будет прекращения программы
  if (err_n == -1) {
  if(client_sock_fd)close(client_sock_fd); 
  unlink(sockaddr_un_server.sun_path);
   err(EXIT_FAILURE, "%s: %d", err_str, line);
  }
  return 0;  // на случай если не выходим но  была не критическая ошибка
}

int main() {
 

  char *msg_send = "Hi!!";
  char msg_recv[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_un);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно

  memset(&sockaddr_un_client, 0, len);     // забьем всю структуру нулем
  memset(&sockaddr_un_server, 0, len);     // забьем всю структуру нулем


  sockaddr_un_server.sun_family = AF_LOCAL;// укажем адрес сервера, мы должны его знать.
  strncpy(sockaddr_un_server.sun_path, "/tmp/AF_SERVER", sizeof(sockaddr_un_server.sun_path) - 1); //адреса на клиенте и сервере обычно разные!
  
 

    sockaddr_un_client.sun_family = AF_LOCAL;
  //strncpy(sockaddr_un_client.sun_path, "x/tmp/AF_CLIENT",sizeof(sockaddr_un_client.sun_path) - 1);  // в имени можно использовать виртуальное имя которое начинается с 0
  //sockaddr_un_client.sun_path[0]=0; 
  
  strncpy(sockaddr_un_client.sun_path, "/tmp/AF_CLIENT",sizeof(sockaddr_un_client.sun_path) - 1); //но пока сделаем так, разные сокеты


  CheckError(client_sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0), "socket",__LINE__);   //(SOCK_DGRAM 0) - udp,  (SOCK_STREAM 0) - tcp
  printf("Клиент запущен на сокете: %s\n",sockaddr_un_client.sun_path);
 
  CheckError(bind(client_sock_fd, (struct sockaddr *)&sockaddr_un_client, len),"bind", __LINE__); // bind нужен для семейства АF_LOCAL, 
  //мы клиент, присваеваем своему сокету адрес
  

  
  CheckError(connect(client_sock_fd, (struct sockaddr *)&sockaddr_un_server, len),"connect", __LINE__); // это в пару акцепту , указываем здесь len а не указатель

  CheckError(send(client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
  CheckError(recv(client_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",__LINE__); // и получим ответку

  CheckError(getpeername(client_sock_fd, (struct sockaddr *)&sockaddr_un_server, plen),"getsockname", __LINE__); // заполним структуру данными того конца
  CheckError(printf("SERV PATH: %s\nSERV MSG: %s\n", sockaddr_un_server.sun_path, msg_recv), "printf", __LINE__);

 
  getsockname(client_sock_fd,(struct sockaddr *)&sockaddr_un_client2,plen);
  
  CheckError(printf("MY PATH (getsockname): %s\n", sockaddr_un_client2.sun_path), "printf", __LINE__);

  CheckError(close(client_sock_fd), "close", __LINE__);
  CheckError(unlink(sockaddr_un_client.sun_path), "unlink", __LINE__);

  return 0;
}
