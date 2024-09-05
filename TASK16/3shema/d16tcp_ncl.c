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
#define NUM_THREADS 10


/*
TCP клиент для всех схем.

АЛГОРИТМ
1-ИНИЦИАЛИЗАЦИЯ
===
0 - создать соединения и занулить структуры
1. Отправить pid
2. Получить порт

2. Работа
2а Инициализация
    0 - создать соединения и занулить структуры
    1 вывести на экран меню
    2 если выбран >Close Server послать команду exit серверу и на  выход
    3 если выбран >Test  (спросить кол-во процессов)
   
   породить потоки типа
  
       ЦИКЛ, пока не команда  выход от сервера
        >
        3.1 отправить серверу [pid + число]  // для удобства тестирования
        3.2 получить от сервера [этот же pid +новое число] или команду выход.
        3.3. увеличить статистику
        3.4 распечатать число
        <
        
        
        если выход, то распечатать статистику  выйти
  
  
  
*/






 //struct sockaddr_un sockaddr_un_server, sockaddr_un_client, sockaddr_un_client2;


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

int initTCP(char * ip, int port, struct sockaddr_in * sockaddr_in_server, int *client_sock_fd, char * msg_send)
{
sockaddr_in_server->sin_family = AF_INET;
  if (inet_pton(AF_INET, "127.0.0.1", &(sockaddr_in_server->sin_addr))<= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  sockaddr_in_server->sin_port = htons(port); // адрес сервера что забиндили
    
  
  CheckError(*client_sock_fd = socket(AF_INET,SOCK_STREAM , 0), "socket",__LINE__);   //  1.  (SOCK_DGRAM 0) - udp,  (SOCK_STREAM 0) - tcp
 
  Fill_ip(*sockaddr_in_server, ip, __LINE__);

  printf("1NETWORK TCP Клиент запущен и устанавливает соединение с сервером по  сокету: %s %d\n",ip,ntohs(sockaddr_in_server->sin_port));

  CheckError(connect(*client_sock_fd, (struct sockaddr *)sockaddr_in_server, sizeof(*sockaddr_in_server)),"connect", __LINE__); // это в пару акцепту , указываем здесь len а не указатель. наш fd и серверный адрес
/*  
  long tid=syscall(SYS_gettid);
 printf("NETWORK TCP Клиент регистрация на сервере,  [tid =%ld]\n",tid);
 sprintf(msg_send,"%ld [Hi!]",tid);
 
 */
 //CheckError(send(*client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
  
return 0;
}

int Pingpong( char *msg_recv, char *msg_send,int *client_sock_fd)  //получим от сервера порт
{
 long tid=syscall(SYS_gettid);
 printf("NETWORK TCP Клиент регистрация на сервере,  [tid =%ld]\n",tid);
 sprintf(msg_send,"%ld [Hi!]",tid);
 CheckError(send(*client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
 sprintf(msg_recv,"empty");
 CheckError(recv(*client_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",__LINE__); // и получим ответку
 int x;
 sscanf(msg_recv,"%d",&x);
 printf("[%ld]>нам назначен порт: %d\n",tid,x);
 return x;
}

long PingpongWORK( char *msg_recv, char *msg_send, long data,int *client_sock_fd )  //взаимодействие с рабочим сервером - кинем запрос - число и получим ответку -( число+1)
{
 long tid=syscall(SYS_gettid);
 sprintf(msg_send,"%ld %ld",tid,data);// и добавим свой тид для удобства тестированияитого сообщение: [tid число]
 CheckError(send(*client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
 CheckError(recv(*client_sock_fd, (char *)msg_recv, STR_SIZE_MAX, 0), "recv",__LINE__); // и получим ответку
 printf("[%ld]> recive data:%s\n",tid,msg_recv);
 return atol(msg_recv);// и вернем сразу ввиде числа
}

int Menu()
{
int choose=0;
printf ("Меню:\n");
printf ("1. (администрирование) завершить Клиента/Cервер\n");
printf ("2. (Тестирование)\n");


 
while (1){
 printf ("ваш выбор:\n");choose=getchar();
 while(choose==10) choose=getchar();
 choose-='0';
 printf ("вы выбрали:%d\n",choose);
 if(choose==1){
 printf ("введите [номер клиента] или 0 - для завершения сервера\n");
 int i;
 scanf("%d",&i);
 if(i>=0) i=i*(-1);
 if(i==0) i=-1;
 return i;
 }
 if((choose>0)&&(choose<3)) {break;} 
}


choose=-1;
printf("*сколько клиентов запустить 1-10?\n");
while (1){
 printf ("ваш выбор:\n");choose=getchar();
 while(choose==10) choose=getchar();
 choose-='0';
 printf ("вы выбрали:%d\n",choose);

 if((choose>0)&&(choose<10)) {break;} 
}
 
printf("На тестирование будет запущено %d клиентов в отдельных потоках\n", choose);
return choose;
}// end menu

void SendAdminCommand(int cmd ,int * client_sock_fd,char *msg_send)
{
long tid=syscall(SYS_gettid);
 switch(cmd)
 {           // 
 case -1: sprintf(msg_send,"exit");break; //exit
          //завершить конкретный процесс
 default: 
 sprintf(msg_send,"%d exit",cmd*(-1));break;
 }
 CheckError(send(*client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение
 printf("[%ld] Административная команда (%d) отослана: %s\n",tid,cmd,msg_send);
return;
}


/////////////////////////////////////////////////////////СЕКЦИЯ перед main
//pthread_t tids[NUM_THREADS];  

typedef  struct data
    {
    pthread_t tid;  // 
    long tid2;
    unsigned long count;// сколько раз поработал на  назначенном сервере
    unsigned long respond;// ответ сервера, ввиде числа, собственно count должен = respond в итоге
    int Adminflag; // если не 0, то это админский процесс, пусть отсылает команду сервера, код команды
    } data;

void *MyThreadFunc(data *);   /* thread  */



int main() {
 

 data Clients_data[NUM_THREADS]; // для каждого клиента найдется процесс

  char msg_send[STR_SIZE_MAX]; 
  char msg_recv[STR_SIZE_MAX];
  
  sprintf(msg_send,"Hi!!");
  strcpy(msg_recv,"init");
  
  int cl_count=-1;

 int i=0;

void *x;
     
     
     for(i=0;i<NUM_THREADS;i++){
     Clients_data[i].tid=0; Clients_data[i].tid2=0; Clients_data[i].Adminflag=0;
     }
     
 //2 работа
 while (cl_count==-1){         //  в цикле выполняются админские действия
 cl_count=Menu();                 
 
 if(cl_count<0) 
  {
 //запустить админский процесс
 Clients_data[0].Adminflag=cl_count;
 Clients_data[0].count=0;Clients_data[0].respond=0;
 x=(void*)(Clients_data);
 pthread_create(&(Clients_data[0].tid), NULL, MyThreadFunc,x);
 printf ("Административный процесс  создан [%ld]\n",Clients_data[i].tid);
 pthread_join(Clients_data[0].tid, NULL);
 printf ("Административный процесс выполнен\n");
  }
 }   //на выходе у нас сколько клиентский процессов надо запустить
 
 printf ("нужно создать cl_count=%d клиентов\n",cl_count);
 int start =1;
 while (1)
 { for ( i = 0; i < cl_count; i++){
            if(start){Clients_data[i].count=0;Clients_data[i].respond=0;Clients_data[i].Adminflag=0;};
            //void *x;
            x=(void*)(Clients_data+i);
            pthread_create(&(Clients_data[i].tid), NULL, MyThreadFunc,x);  // Создадим потоки Клиентов
             printf ("клиент%d создан [%ld]\n",i+1,Clients_data[i].tid);
            }
        for ( i = 0; i < cl_count; i++)
            pthread_join(Clients_data[i].tid, NULL);
     start=0;
      printf ("Все потоки завершились, cl_count=%d,  стартуем еще раз \n",cl_count);
    //..(.)  здесь пока стоим        
}
  return 0;
}

void *MyThreadFunc(data *x)
{  
  char msg_send[STR_SIZE_MAX]; 
  char msg_recv[STR_SIZE_MAX];
  
  x->tid2=syscall(SYS_gettid); // 1. первичная инициализация
  
  struct sockaddr_in sockaddr_in_server,  sockaddr_in_client, sockaddr_in_client2;
  int client_sock_fd;  // в клиенте accept не нужен, поэтому только один дескриптор
  
  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
 // socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно
  char ip[20];
  

  int newport=0;

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем           0.подготовка структур
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем

  initTCP(ip,PORT,&sockaddr_in_server,&client_sock_fd,msg_send); 
  
  if(x->Adminflag!=0) {SendAdminCommand(x->Adminflag,&client_sock_fd,msg_send);
                       close(client_sock_fd);exit(0);
                       }
                      
  
  //   связаться с управляющим и получить порт
  newport=Pingpong(msg_recv,msg_send,&client_sock_fd);
  CheckError(close(client_sock_fd), "close", __LINE__); // освободить соединение с управляющим
  
  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем           0.подготовка структур
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем
  initTCP(ip,newport, &sockaddr_in_server,&client_sock_fd,msg_send); //инициализируем соединение по новому адресу
  
 
  unsigned long tmp;
  tmp=PingpongWORK(msg_recv,msg_send, x->respond,&client_sock_fd);
  if(strcmp(msg_recv,"exit") == 0) {printf("Получен exit - сигнал завершения работы\n"); } // от сервера,убрали  break;
  x->count++;x->respond=tmp;
  printf("[%ld] count=%ld, respond=%ld\n", x->tid2,x->count,x->respond);
  
  //sprintf(msg_send,"%ld exit",x->tid2);
  //CheckError(send(client_sock_fd, (char *)msg_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем серверу сообщение о выходе здесь бы и проверку что все дошло
 
  
  CheckError(close(client_sock_fd), "close", __LINE__); // освободить соединение с управляющим
  printf("[%lu]> закрываем сокет и выходим %lu",x->tid,x->tid2);
 

}
