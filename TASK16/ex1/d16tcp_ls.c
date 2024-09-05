#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define STR_SIZE_MAX 255

 struct sockaddr_un sockaddr_un_server, sockaddr_un_client;
  int server_sock_fd, new_sock_fd; // new_sock_fd  это после акцепта, его должны использовать, см. активный и пассивный сокет
  
  
static void CheckError(int err_n, char *err_str, int line) {          //Подсмотрел, понравилась конструкция
  if (err_n < 0) {   
  if(new_sock_fd) close(new_sock_fd);                            // добавим только  close на случай ошибок
  if(server_sock_fd)close(server_sock_fd);
  unlink(sockaddr_un_server.sun_path);
   err(EXIT_FAILURE, "%s: %d", err_str, line);   // ну или как обычно perror можно использовать
  }   
}

int main() {
 
  char msg_recv_send[STR_SIZE_MAX] = {0};
  server_sock_fd=0; new_sock_fd=0;

  socklen_t len = sizeof(struct sockaddr_un);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen=&len;                       //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.

  sockaddr_un_server.sun_family = AF_LOCAL;
  strncpy(sockaddr_un_server.sun_path, "/tmp/AF_SERVER", sizeof(sockaddr_un_server.sun_path) - 1); //адреса на клиенте и сервере обычно разные!

  CheckError(server_sock_fd = socket(AF_LOCAL,SOCK_STREAM, 0), "socket", __LINE__);
  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_un_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  
  printf("Сервер стартовал и ждет клиента\n");
  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);                                     //  2. повесили слушать, и ушли в блок до ответа клиента
  CheckError(new_sock_fd = accept( server_sock_fd, (struct sockaddr *)&sockaddr_un_client, &len),"accept", __LINE__); 


  CheckError(recv(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),"recv", __LINE__);     // 3. ждем сообщение от клиента и получаем в структуру адрес того конца
  CheckError( getpeername(new_sock_fd, (struct sockaddr *)&sockaddr_un_client, plen),"getsockname", __LINE__); //  и теперь в структуре есть адрес клиента

  CheckError(printf("CLIENT PATH: %s\nCLIENT MSG: %s\n",sockaddr_un_client.sun_path, msg_recv_send), "printf", __LINE__);

  strcat(msg_recv_send,"+ добавка от сервера"); //4. видоизменяем полученное сообщение и отправляем клиенту
  CheckError(send(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send", __LINE__);

  CheckError(close(new_sock_fd), "close", __LINE__);   // чистим за собой и на выход
  CheckError(close(server_sock_fd), "close", __LINE__);

  CheckError(unlink(sockaddr_un_server.sun_path), "unlink", __LINE__);

  return 0;
}
