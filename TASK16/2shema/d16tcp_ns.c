#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/syscall.h>

#include <time.h>
#include <pthread.h>



#define STR_SIZE_MAX 255
#define PORT 8180
#define MAX_ab 100 //максимальное кол-во абонентских серверов
#define MAX_ra 100 //максимальное кол-во зарегестрированных абонентов - они же клиенты ( процессы клиентские)

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

// struct sockaddr_in sockaddr_in_server,  sockaddr_in_client;
 // int server_sock_fd, new_sock_fd; // new_sock_fd  это после акцепта, его должны использовать, см. активный и пассивный сокет
  

int CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
 perror("perror>");
 printf("ошибка в функции: %s: строка %d",err_str, line);
 exit(-1);   
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

typedef struct abonent {                  // типа справочник абонентов, но на самом деле это вышел справочник серверов
pthread_t tid; // идентификатор потока
long tid2; // читабельный идентификатор
char free;
char redy; // поднимим флаг в 1 когда сервер будет готов к работе
int port; // назначенный порт у рабочего сервера
}abonent;

typedef struct REGabonent{  // а вот это справочник Абонентов
long id; // tid клиентского процесса
long tid2; //какому серверу назначен
int index; //index сервера в массиве
} REGabonent;

char * parcer(char *str,int k)  // [ число1 число2 str] вернем str , k- сколько чисел пропустить
{

if(str==NULL) return NULL;
int i=0; int j=0;
for(i=0;i<strlen(str);i++) {
                    // if(str[i]==0) return NULL;
                    // i++;
                    if(str[i]==' ') j++;
                    if(j>=k) break;
                    }
return str+i+1;

}

int Get_free_index(abonent *a)
{ int i=0;
for(i=0; i<MAX_ab; i++){
if ((a+i)->free==1) return i;
}
return -1;
}

int Client_free_index(REGabonent *a)
{ int i=0;
for(i=0; i<MAX_ra; i++){
if ((a+i)->id==0) return i;
}
return -1;
}

int GetFreeTREAD(abonent *a)
{
int i=0;

//for(i=0; i<MAX_ab; i++) printf("[i=%d] free=%d tid=%ld tid2=%ld port=%d\n",i,(a+i)->free,(a+i)->tid, (a+i)->tid2,(a+i)->port);
for(i=0; i<MAX_ab; i++){
if (((a+i)->free==1)&& (((a+i)->tid)!=0))return i; // нужен первый свободный и заполненный, - значит на слоте уже крутиться процесс
}
return -1;

}

void DelAb(abonent *Abonents,REGabonent *Reg_Ab,int del_tid) // удалить клиента из справочников
{  
  for(int ii=0;ii<MAX_ra;ii++) {
     printf("[%d] клиент [%ld]   на сервере [%ld] del_tid=%d\n",ii,Reg_Ab[ii].id, Reg_Ab[ii].tid2,del_tid);
     if (Reg_Ab[ii].id==del_tid) {
                                    // обработаем Серверный справочник а затем клиентский
                                   Abonents[Reg_Ab[ii].index].free=-1; // пометим на удаление, процесс по флагу отошлет его клиенту команду выход
                                   Reg_Ab[ii].id=0;Reg_Ab[ii].index=0;Reg_Ab[ii].tid2=0;
                                   return;
                                   } 
      
  }
  printf("Клиент с id=%d не зарегестрирован\n", del_tid);
}
void *MyThreadFunc(abonent *);   /* thread  */


int main() {
 
 struct sockaddr_in sockaddr_in_server,  sockaddr_in_client;
 int server_sock_fd, new_sock_fd; // new_sock_fd  это после акцепта, его должны использовать, см. активный и пассивный сокет
 
  char msg_recv_send[STR_SIZE_MAX] = {0};
 int i=0;
  //int s;
  //unsigned char bufip[sizeof(struct in_addr)];
  char ip[20];

  abonent Abonents[MAX_ab];
  REGabonent REG_Ab[MAX_ra];
  
  for(i=0;i<=MAX_ra;i++){
  REG_Ab[i].id=0;
  REG_Ab[i].tid2=0;
  REG_Ab[i].index=0;
  }
  
  
  int abcount=0;
  
    server_sock_fd=0; new_sock_fd=0;
  //size_t countsend=0;
  
  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
 // socklen_t *plen=&len;      //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем
  
  // переведем IP строку в число


  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = inet_addr("127.0.0.1"); // можно и так
  sockaddr_in_server.sin_port = htons(PORT);


  CheckError(server_sock_fd = socket(AF_INET,SOCK_STREAM, 0), "socket", __LINE__);
  const int enable = 1;
  if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) perror("setsockopt(SO_REUSEADDR) failed"); 
  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  
    
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  
  printf("NETWORK TCP Сервер запущен на сокете: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);                                     //  2. повесили слушать, и ушли в блок до ответа клиента
  //CheckError(new_sock_fd = accept( server_sock_fd, (struct sockaddr *)&sockaddr_in_client, &len),"accept", __LINE__); 

printf("MAX кол-во рабочих серверов =%d,MAX кол-во клиентов=%d  \n",MAX_ab,MAX_ra );

  for( i=0; i<MAX_ab;i++){    //инициализация пула
     (Abonents+i)->free=1;(Abonents+i)->redy=0;
     (Abonents+i)->port=PORT+i+1;
     Abonents[i].tid=0;
     Abonents[i].tid2=0;
    }
        

 while (1)
 {
 CheckError(new_sock_fd = accept( server_sock_fd, (struct sockaddr *)&sockaddr_in_client, &len),"accept", __LINE__); 
  CheckError(recv(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),"recv", __LINE__); //  3 Получаем запрос клиента 
  printf("Сервер(управляющий)> получен новый запрос: %s\n",msg_recv_send);
  if(strcmp(msg_recv_send,"exit")==0) break;
  if(strcmp(parcer(msg_recv_send,1),"exit")==0) { // команда завершить процесс  [tid exit]
  int del_tid=0;
  sscanf(msg_recv_send,"%d\n",&del_tid);
  printf("del_tid=%d\n",del_tid);
  DelAb(Abonents,REG_Ab,del_tid); // удалить клиента из справочников                           
  continue;  // нужна след команда
  }
  int j=0;
  
  i=GetFreeTREAD(Abonents);             //заполним индексы, либо свободным потоком, либо слотом для нового потока
  printf("index свободного потока=%d\n",i);
  if(-1==i) i=Get_free_index(Abonents);
  j=Client_free_index(REG_Ab);
  
  if(-1==i) // Обработчик ситуации когда порты закончились, надо ждать пока освободятся
           {printf("сервер, не могу обслужить клиента, закончились порты\n");             //заглушка
            if(abcount<MAX_ab) printf("логическая ошибка, %d \n", __LINE__ );
           }
  if(-1==j) // Обработчик ситуации когда лимит клиентов превышен, надо ждать пока освободятся
           {printf("сервер, не могу обслужить клиента, закончились слоты\n");             //заглушка    здесь потом исправить счетчик клиентов! их надо тоже ведь разделить
            if(abcount<MAX_ab) printf("логическая ошибка, %d \n", __LINE__ );
           }
           
  printf("Сервер> назначен порт %d,  index=%d\n",Abonents[i].port,i);
  void *x;
            
           if(i>=0){    
                  if(0==Abonents[i].tid2){                           //вариант для порождения процесса
                   (Abonents+i)->free=0; //занимаем ячейку
                   x=(void*)&(Abonents[i]);         
                   pthread_create(&(Abonents[i].tid), NULL, MyThreadFunc,x);  // Создадим поток рабочего Сервера
                   printf("Сервер> создан сервер [%ld]\n",Abonents[i].tid);
                   while (0 == Abonents[i].redy) sleep(0.1);
                   printf("Сервер> создан сервер [%ld]\n",Abonents[i].tid2);
                    } else{                                            // вариант для существующего потока
                    (Abonents+i)->free=0; // вновь занимаем ячейку
                    }
                                   
                    //регистрируем клиента
                      sscanf(msg_recv_send,"%ld\n",&REG_Ab[j].id);    //[пид hi] надо взять пид                                                                      
                      REG_Ab[j].tid2=Abonents[i].tid2;REG_Ab[j].index=i;
                      printf("[%ld] Назначен клиент id= %ld\n",Abonents[i].tid2,REG_Ab[j].id);
                  
                   Fill_ip(sockaddr_in_server, ip, __LINE__);
                   //printf("Сверим мои параметры: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));

                   sprintf(msg_recv_send,"%d",Abonents[i].port); // отправим клиенту порт нового сервера 
                   
                   if(CheckError(send(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send", __LINE__)!=STR_SIZE_MAX)
                   printf("Не все данные переданы\n");
                    //(*)здесь еще все хорошо
                   
                  }
    close(new_sock_fd);
  }// end while
  
      
       Fill_ip(sockaddr_in_client, ip, __LINE__);    
    
  CheckError(printf("CLIENT PATH: %s %d\nCLIENT MSG: %s\n",ip,ntohs(sockaddr_in_client.sin_port) ,msg_recv_send), "printf", __LINE__);



  CheckError(close(new_sock_fd), "close", __LINE__);   // чистим за собой и на выход
  CheckError(close(server_sock_fd), "close", __LINE__);


  return 0;
}


void *MyThreadFunc(abonent *x)
{  
 struct sockaddr_in sockaddr_in_server,  sockaddr_in_client;
 int server_sock_fd, new_sock_fd; // new_sock_fd  это после акцепта, его должны использовать, см. активный и пассивный сокет


  char msg_send[STR_SIZE_MAX]; 
  char msg_recv[STR_SIZE_MAX];
  char msg_recv_send[STR_SIZE_MAX] = {0};
  
  char ip[20];
  server_sock_fd=0; new_sock_fd=0;
  //size_t countsend=0;

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
 // socklen_t *plen=&len;      //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем
  
  // переведем IP строку в число


  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = inet_addr("127.0.0.1"); // можно и так
  sockaddr_in_server.sin_port = htons(x->port);


  CheckError(server_sock_fd = socket(AF_INET,SOCK_STREAM, 0), "socket", __LINE__);
  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  
  x->tid2=syscall(SYS_gettid); //
  printf("[%ld] NETWORK TCP Сервер запущен на сокете: %s %d\n",x->tid2,ip,ntohs(sockaddr_in_server.sin_port));

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);   //  2. повесили слушать, и ушли в блок до ответа клиента
  x->redy=1;
  // printf("[%ld][%ld]> redy=%d\n", x->tid2, x->tid, x->redy);
  CheckError(new_sock_fd = accept( server_sock_fd, (struct sockaddr *)&sockaddr_in_client, &len),"accept", __LINE__);
  unsigned long z;
  long cpid;
  
  
  while(1)
  {
   
   CheckError(recv(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0),"recv", __LINE__); //  3 Получаем запрос клиента 
    printf("1[%ld] msg> %s\n", x->tid2, msg_recv_send );
    if (strcmp(msg_recv_send,"exit")==0) {printf("Получен exit - сигнал завершения работы\n"); //теперь нам не надо делать break;
                                          x->redy=0;  //блокируем и просто ждем нового соединения
                                          CheckError(close(new_sock_fd), "close", __LINE__); 
                                          x->free=1; 
                                          CheckError(new_sock_fd = accept( server_sock_fd, (struct sockaddr *)&sockaddr_in_client, &len),"accept", __LINE__); 
                                          //x->free=0;  // на самом деле это сделает управляюший сервер как найдет нам нового клиента
                                          continue;
                                         } // сигнал от клиента
    if(x->free == -1) {printf("Установлен флаг - сигнал завершения работы\n");
                      sprintf( msg_recv_send,"exit" );
                      CheckError(send(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем клиенту выход
                      continue; 
                       }  // сигнал от менеджера сервера
   
    sscanf(msg_recv_send, "%ld",&cpid);
    sscanf(parcer(msg_recv_send,1), "%ld",&z);
  
   sprintf(msg_recv_send,"%ld",z+1);
   printf("2[%ld] send msg> %s\n", x->tid2, msg_recv_send );
   CheckError(send(new_sock_fd, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем ответку
   
   sleep(3);
   }
    x->redy=0;
    CheckError(close(new_sock_fd), "close", __LINE__);   // чистим за собой и на выход
    CheckError(close(server_sock_fd), "close", __LINE__);
    //ичистим структуру, что бы потом понимать нужно в слот создавать поток, или он уже существует физически
    x->tid2=0;x->tid=0;x->port=0;
    x->free=1;
    printf("[%ld] завершение процесса\n",x->tid2);
 
}
/*
*typedef struct abonent {                  // типа справочник абонентов, но на самом деле это вышел справочник серверов
pthread_t tid; // идентификатор потока
long tid2; // читабельный идентификатор
char free;
char redy; // поднимим флаг в 1 когда сервер будет готов к работе
int port; // назначенный порт у рабочего сервера
}abonent;*/
