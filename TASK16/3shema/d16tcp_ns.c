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
#include <mqueue.h>

/*  АЛГОРИТМ

Поменяем алгоритм на следующий
справочник абонентов теперь и будет у нас очередью

тогда управляющий сервер делает акцепт, и сохраняет в справочник клиентов дескриптор и структуру
а первый  освободившийся сервер берет из справочника клиента, и сообщает ему по порту управляющего сервера
свой новый порт, и далее клиент уже идет по новому порту и в целом все как обычно

справочник тогда организуем так - поступления клиентов тогда будут идти в соответствующие слоты цикле по порядку, а будем 
сдвигать индекс какого клиента брать следующим, 
конкуренцию разрешим так - что бы не терять на мьютексах, пусть сервера берут клиента ставят свой код, и через несколько инструкций сверят что код не изменился -
в случае когда клиентов много а серверов мало - конкуренции и так не будет, а в обратном случае то время что что будут сервера тупить - не критично.


=============================                           //   не тот алгоритм что требовался, ошибка - клиент попавший в очередь на этот сервер будет ждать своей очереди
                                                              даже когда остальные сервера свободны
1 управляющий сервер прлучает запрос от клиента  
2 определяет на какой сервер его посадить 
3 если не достигнуто макс кол-во серверов , создает сервера
4. если достигнуто то определяет на какой сервер его посадить , функция - выбрать наименее загруженный сервер
5. устанавливает серверу флаг что у него новый абонент, заполняет слот в серверном справочнике на нового абонента

6. Сервер видя флаг, останавливается на обработке, и запускает акцепт на новый сокет. который теперь будет расположен в обьекте клиента

*/

#define STR_SIZE_MAX 255
#define PORT 8180
#define MAX_ab 3 //максимальное кол-во абонентских серверов
#define MAX_ra 6 //максимальное кол-во зарегестрированных абонентов - они же клиенты ( процессы клиентские)

int P2next=0; // указатель на след клиента, который стоит в очереди на обработку
int P2from=0; // указатель на след свободный слот для регистрации нового клиента

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

typedef struct REGabonent{  // а вот это справочник Абонентов (Клиентов)
long id; // tid клиентского процесса
long tid2; //какому серверу назначен
int index; //index сервера в массиве

int socket;                 // поместим сокет в Клиента
struct sockaddr_in  addr;
int socket0;                 // поместим и сокет управляющего сервера  в Клиента 
struct sockaddr_in  addr0;  

} REGabonent;


typedef struct abonent {                  // типа справочник абонентов, но на самом деле это вышел справочник серверов
pthread_t tid; // идентификатор потока
long tid2; // читабельный идентификатор
char free;
char redy; // поднимим флаг в 1 когда сервер будет готов к работе
int port; // назначенный порт у рабочего сервера
int NewClient;// флаг - если 0 это режим для одного клиента на сервере,  если не ноль, то это индекс нового абонента
int ClientsCount;
int MaxMyClients;
REGabonent *CL;

}abonent;


/*
typedef struct THdata{              // структура аргумента
REGabonent *CL; // указатель на весь массив справочника клиентов.
abonent *S;// указатель на конкретный сервер из массива серверов
} THdata;*/

void InitStruct(REGabonent *r, abonent *a)
{
socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
int i;

  for(i=0;i<=MAX_ra;i++){// справочник клиентов
  (r+i)->id=0;
  (r+i)->tid2=0;
  (r+i)->index=0;

  (r+i)->socket=0;                 
  memset(&((r+i)->addr), 0, len);
  }
  
  for( i=0; i<MAX_ab;i++){    //инициализация пула
     (a+i)->free=1;(a+i)->redy=0;
     (a+i)->port=PORT+i+1;
     (a+i)->tid=0;
     (a+i)->tid2=0;
     (a+i)->NewClient=0;// флаг - если 0 это режим для одного клиента на сервере,  если не ноль, то это индекс нового абонента
     (a+i)->ClientsCount=0;
    }
}


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

//  Найдет слот для нового сервера в массиве серверов - в данной реализации это уже не особо нужно 
int Get_free_index(abonent *a)
{ int i=0;
for(i=0; i<MAX_ab; i++){
if ((a+i)->free==1) return i;
}
return -1;
}

int Client_free_index(REGabonent *a, int from )  // поиск первого свободного слота начиная с индекса from, в данной реализации параметр from нужен скорее для оптимизации поиска слота
{ int i=0; int j=0;
//for(int ii=0;ii<MAX_ra;ii++) {
//    printf(">>[%d] клиент [%ld]   на сервере [%ld]\n",ii,a[ii].id, a[ii].tid2);}
if(from>=MAX_ra) j=from;
for (i=from; i<MAX_ra; i++) {//printf(">>>[%d] клиент [%ld]   на сервере [%ld]\n",i,a[i].id, a[i].tid2);
                             if ((a+i)->id==0) return i;}
i=0; 
while(i<j) { if ((a+i)->id==0) return i;
             i++; 
           }
return -1;
}


int GetFreeTREAD(abonent *a)
{
int i=0;

for(i=0; i<MAX_ab; i++) printf("[i=%d] free=%d tid=%ld tid2=%ld port=%d\n",i,(a+i)->free,(a+i)->tid, (a+i)->tid2,(a+i)->port);
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


int setP2next(abonent *a)
{
int i;
for( i=0;i<MAX_ra;i++)
{
printf("p2next[%d] Stid2=%ld  CLpid=%ld\n",P2next,(a->CL+P2next)->tid2,(a->CL+P2next)->id);
}
for( i=0;i<2;i++)  {   //  выполним поиск не более 2 раз
  while (P2next<MAX_ra)
{
if((a->CL+P2next)->id!=0) {  //это кл в очереди
                             if((a->CL+P2next)->tid2==0)  //.. и ему не назначен сервер
                                                         {
                                                            (a->CL+P2next)->tid2=a->tid2; // назначаем текущий
                                                            return P2next;
                                                         }
                                                         else
                                                         {    // сервер уже назначен
                                                            if((a->CL+P2next)->tid2==a->tid2) return P2next; // это наш сервер
                                                            P2next++;continue;
                                                         }
                             }
                             else
                             { // это пустой слот
                             P2next++;
                             }
                             
}
P2next=0;}; //след поиск
return -1;
}

void *MyThreadFunc(abonent *);   /* thread  */



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SIZE 10000  // в очереди будем пересылать индекс

int main()          // Попробуем заново
{
 struct sockaddr_in sockaddr_in_server, sockaddr_in_client;

  struct mq_attr queue_attr;      // Атрибуты нашей очереди. Они могут быть установлены только при создании.
  queue_attr.mq_maxmsg = 32;     // максимальное число сообщений в очереди в один момент времени 
  queue_attr.mq_msgsize = SIZE; // максимальный размер очереди 
  mqd_t ds;  //очередь posix
  char QIndex[SIZE]; //буфер для очереди
  
  if ((ds = mq_open("/my_queue", O_CREAT | O_RDWR , 0600,  NULL )) == (mqd_t)-1){ //  имя начинается со слэша
   perror("open queue error");
   return -1;
  }
  
  

  
char msg_recv_send[STR_SIZE_MAX] = {0};
 int i=0;
 
  char ip[20];
   int server_sock_fd;
  // struct sockaddr_in sockaddr_in_server;

  
  abonent Abonents[MAX_ab];   
  REGabonent REG_Ab[MAX_ra];
  
  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
 // socklen_t *plen=&len;      //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.


 InitStruct(REG_Ab,Abonents);
 
  int abcount=0;
  
  server_sock_fd=0; 
  
  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем
  
  // переведем IP строку в число


  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = inet_addr("127.0.0.1"); // можно и так
  sockaddr_in_server.sin_port = htons(PORT);

  CheckError(server_sock_fd = socket(AF_INET,SOCK_STREAM, 0), "socket", __LINE__);  // да верно это

  const int enable = 1;
  if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) perror("setsockopt(SO_REUSEADDR) failed"); 
  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - обьявили и забиндили
  
    
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  
  printf("NETWORK TCP Сервер запущен на сокете: %s %d\n",ip,ntohs(sockaddr_in_server.sin_port));

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);                                     //  2. повесили слушать  управляющий сервер
  
  //создадим таблицу серверов.
  
  printf("MAX кол-во рабочих серверов =%d,MAX кол-во клиентов=%d  \n",MAX_ab,MAX_ra );

  for( i=0; i<MAX_ab;i++){    //1. инициализация пула серверов
    
     Abonents[i].port=PORT+i+1;
     Abonents[i].tid=0;
     Abonents[i].tid2=0;
    // Abonents[i].NewClient=0;// флаг - если 0 это режим для одного клиента на сервере,  если не ноль, то это индекс нового абонента
     Abonents[i].ClientsCount=0;
     Abonents[i].CL=REG_Ab;
    void *x;
   
    x=(void*)&Abonents[i];    
    pthread_create(&(Abonents[i].tid), NULL, MyThreadFunc,x);  // Создадим поток рабочего Сервера
    while (0 == Abonents[i].redy) sleep(0.01); //дождемся окончания готовности
    printf("Сервер> создан сервер [%ld]\n",Abonents[i].tid);
    }
  
 int Cpointer=-1; 
  //2 . Получаем регистрацию клиента или команду , в случае регистрации - добавляем в список клиентов
while(1)
{
  P2from=0;
  Cpointer=Client_free_index(REG_Ab,0);
  while(Cpointer<0) {
  Cpointer=Client_free_index(REG_Ab,0);
  //printf("[Упр. сервер],  индекс нового клиента=%d\n",Cpointer);
                    }  // определим слот, куда разместим нового клиента 
  
 CheckError(REG_Ab[Cpointer].socket0 = accept( server_sock_fd, (struct sockaddr *)&REG_Ab[Cpointer].addr0, &len),"accept", __LINE__); // найти абонента !!!!!!!!
  CheckError(recv(REG_Ab[Cpointer].socket0, (char *)msg_recv_send, STR_SIZE_MAX, 0),"recv", __LINE__); //  3 Получаем запрос клиента 
  printf("[Упр.Сервер]> получен новый запрос: %s\n",msg_recv_send);
  
  if(strcmp(msg_recv_send,"exit")==0) break;
  if(strcmp(parcer(msg_recv_send,1),"exit")==0) { // команда завершить процесс  [tid exit]
  int del_tid=0;
  sscanf(msg_recv_send,"%d\n",&del_tid);
  printf("del_tid=%d\n",del_tid);
  //DelAb(Abonents,REG_Ab,del_tid); // удалить клиента из справочников                           
  continue;  // нужна след команда
  }  
  
   //
   sscanf(msg_recv_send,"%ld",&REG_Ab[Cpointer].id);    //[пид hi] надо взять пид   
   printf("[Упр. сервер] Добавлен клиент id= %ld, по индексу= %d\n",REG_Ab[Cpointer].id,Cpointer);
   for(int ii=0;ii<MAX_ra;ii++) {
     printf("[%d] клиент [%ld]   на сервере [%ld]\n",ii,REG_Ab[ii].id, REG_Ab[ii].tid2);}
   
  sprintf(QIndex,"%d",Cpointer);
  //printf("[Упр. Сервер] send Qindex=%s\n",QIndex);
  if (mq_send(ds, QIndex, strlen(QIndex), 1) == -1){    //положили индекс в очередь, кто первый возьмет тому  и работа
    perror("Sending message error");
    return -1;
     }
   }
    
    if (mq_close(ds) == -1)  // закрываем очередь
    perror("Closing queue error");
    if (mq_unlink("/my_queue") == -1)

    perror("Removing queue error");
    
}  //end main


void *MyThreadFunc(abonent *x)
{  
int server_sock_fd;
struct sockaddr_in sockaddr_in_server;
mqd_t ds;
char buf[SIZE];

 char msg_send[STR_SIZE_MAX]; 
  char msg_recv[STR_SIZE_MAX];
  char msg_recv_send[STR_SIZE_MAX] = {0};
  
   unsigned long z=0;
  long cpid=0;
  
  char ip[20];
  server_sock_fd=0; 
  //size_t countsend=0;
  

   x->tid2=syscall(SYS_gettid); //
   
   
   while ((ds = mq_open("/my_queue", O_RDWR, 0600, NULL)) == (mqd_t)-1){
    perror(" ошибка очереди..\n");
    printf("[%ld] ошибка очереди",x->tid2);
    //return -1;
  }
  //printf("client: идентификатор очереди# %d\n" ,(int) ds);
  

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
 // socklen_t *plen=&len;      //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно.

  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем

 // переведем IP строку в число  и в целом создаем новый сервер

  
  sockaddr_in_server.sin_family = AF_INET;
  sockaddr_in_server.sin_addr.s_addr = inet_addr("127.0.0.1"); // можно и так
  sockaddr_in_server.sin_port = htons(x->port);


  CheckError(server_sock_fd = socket(AF_INET,SOCK_STREAM, 0), "socket", __LINE__);
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  //printf("[%ld] NETWORK TCP Сервер будет запущен на сокете: %s %d\n",x->tid2,ip,ntohs(sockaddr_in_server.sin_port));
  CheckError(bind(server_sock_fd, (struct sockaddr *)&sockaddr_in_server, len),"bind", __LINE__); //1 - объявили и забиндили
  
  Fill_ip(sockaddr_in_server, ip, __LINE__);
  
 
  printf("[%ld] NETWORK TCP Сервер запущен на сокете: %s %d\n",x->tid2,ip,ntohs(sockaddr_in_server.sin_port));

  CheckError(listen(server_sock_fd, 5), "listen", __LINE__);   //  2. повесили слушать
  x->redy=1;

int prio;
//Алгоритм
while(1)
{

   //захват нового клиента
   printf("[%ld] захват нового клиента \n",x->tid2);
   if (mq_receive(ds, buf, SIZE, &prio) == -1){
    perror("cannot receive");
    //return -1;
     } 
   int P2n;
   sscanf(buf,"%d",&P2n); 
   printf("[%ld] захвачен клиент msg=%s, P2n=%d \n",x->tid2,buf,P2n);
  //int P2n=setP2next(x);  
  if((P2n<0)||(P2n>=MAX_ra)) {printf("P2n!!!");continue;}
  
//if((x->CL+P2n)->tid2!=0)if(x->tid2==(x->CL+P2n)->tid2){
 //printf("[%d] индекс нового клиента= %d, id=%d, сервер=%d\n ",x->tid2, P2n,(x->CL+P2n)->id, (x->CL+P2n)->tid2);
  sprintf(msg_recv_send,"%d",x->port); // отправим клиенту порт нового сервера по старому сокету
        printf("[%d]sending: %s\n",x->tid2 ,msg_recv_send);          
         if(CheckError(send((x->CL+P2n)->socket0, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send", __LINE__)!=STR_SIZE_MAX)
          printf("Не все данные переданы\n");
     
  CheckError((x->CL+P2n)->socket = accept(server_sock_fd, (struct sockaddr *)&((x->CL+P2n)->addr), &len),"accept", __LINE__); // перезатираем сокет и структуру
// x->S->ClientsCount++;   //итак захватили сервер;
  
  
   CheckError(recv((x->CL+P2n)->socket, (char *)msg_recv_send, STR_SIZE_MAX, 0),"recv", __LINE__); //  3 Получаем запрос клиента 
    printf("1[%ld] msg> %s\n", x->tid2, msg_recv_send );
    
     sscanf(msg_recv_send, "%ld",&cpid);
    sscanf(parcer(msg_recv_send,1), "%ld",&z);
  
   sprintf(msg_recv_send,"%ld",z+1);
   //printf("2[%ld] send msg> %s\n", x->tid2, msg_recv_send );
   CheckError(send((x->CL+P2n)->socket, (char *)msg_recv_send, STR_SIZE_MAX, 0), "send",__LINE__);  // пошлем ответку
   
   sleep(0.0001); // x->S->ClientsCount--;// 
   
     CheckError(close((x->CL+P2n)->socket0), "close", __LINE__);
      CheckError(close((x->CL+P2n)->socket), "close", __LINE__);
   
   (x->CL+P2n)->id=0;
   (x->CL+P2n)->tid2=0;
   
 
 
}
CheckError(close(server_sock_fd), "close", __LINE__);
} //end TREAD



/*

typedef struct abonent {                  // S -типа справочник абонентов, но на самом деле это вышел справочник серверов
pthread_t tid; // идентификатор потока
long tid2; // читабельный идентификатор
char free;
char redy; // поднимим флаг в 1 когда сервер будет готов к работе
int port; // назначенный порт у рабочего сервера
int NewClient;// флаг - если 0 это режим для одного клиента на сервере,  если не ноль, то это индекс нового абонента
int Clientscount;
}abonent;

typedef struct REGabonent{  //  CL -а вот это справочник Абонентов (Клиентов)
long id; // tid клиентского процесса
long tid2; //какому серверу назначен
int index; //index сервера в массиве

int socket;                 // поместим сокет в Клиента
struct sockaddr_in  addr;
} REGabonent;

*/

/*
int GetMinServer(abonent *a)  // выберем сервер с минимальным кол-вом клиентов  , ну или в общем случае другие критерии
{                                   //-1 - мест больше нет
int i=0;int min=0;
long sum=0;
for(i=0; i<MAX_ra; i++){
sum+=(a+i)->ClientsCount;
if (((a+i)->ClientsCount) < ((a+min)->ClientsCount)) min=i;
}
if (sum>=(MAX_ra*MAX_ab)) return -1;
return min;
}
*/
