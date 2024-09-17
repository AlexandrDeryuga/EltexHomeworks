#include <err.h>
//#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/ip.h> 
#include <netinet/udp.h>
#include <linux/if_ether.h>

//#include "net_headers.h"

#define STR_SIZE_MAX 255 
#define DATAGRAM_LEN 100  //4096
#define PORT 8080
#define MY_PORT 9999
#define DEST_IP "10.0.2.4"          //IP - СЕРВЕРА
#define MY_IP "10.0.2.15"          // ЭТО ПРИЛОЖЕНИЕ КЛИЕНТ - и это наш IP, (IP клиента)

////////////////////////////////////////////
/*Домашка RAW2 - создание заголовка IP+UDP*/
////////////////////////////////////////////

int CheckError(int err_n, char *err_str, int line); //наследственное, печать ошибок
void Fill_ip(struct sockaddr_in saddr, char *ip, int line); //наследственное, не используем


/*вернет указатель куда будем писать объект размером len
bufsze - физический размер буфера
также вернем в sendlen текущий размер на отправку
init=1 - перед отправкой нового пакета нужно обнулить статическую переменную внутри функции, в ней находиться текущий размер пакета,
с каждым новым вызовом при init=0, размер увеличивается на очередную длину len
*/
char * GetPointer(int init, char *BUF,int bufsize, int len, int* sendlen);

void SetUDPH(struct udphdr *p_udphdr,int MYPORT, int DPORT, int sendlen);// установить заголовок UDP, sendlen ранее получаем через GetPointer

/*установить заголовок IP , sendlen ранее получаем через GetPointer,
  int bit_Frag1,int bit_Frag2,int Fofset - соберутся в поле "фрагментация/офсет"
  int v,int ihl - четерехбитовые числа "версии" и "длины заголовка" 
*/
void SetIPH(struct iphdr *p_iphdr,int v,int ihl, int bit_Frag1,int bit_Frag2,int Fofset, int MYPORT, int DPORT, int sendlen, char *my_ip, char* dest_ip);


 struct sockaddr_in sockaddr_in_server,  sockaddr_in_client, sockaddr_in_client2;
 //int client_sock_fd;  // в клиенте accept не нужен, поэтому только один дескриптор




int main() {
 
 struct udphdr s_udphdr, *p_udphdr=NULL; // заголовок UDP
 struct iphdr s_iphdr,*p_iphdr;  //заголовок IP межсетевой
 struct ethhdr s_ethhdr,*p_ethhdr; //Заголовок канального уровня eth , p2p 
 
 unsigned char msg_send[STR_SIZE_MAX]; 
  unsigned char msg_recv[STR_SIZE_MAX];
  
 
  char ip[20];
  char SendString[STR_SIZE_MAX];

  socklen_t len = sizeof(struct sockaddr_in);  // это указатель на размер передаваемого сообщения, но размер может быть не тем который говорим, 
  socklen_t *plen =&len;  //поэтому используется указатель, и он рестрикт, поэтому должен быть единственный, вот и объявляем его явно

  memset(&sockaddr_in_client, 0, len);     // забьем всю структуру нулем           0.подготовка структур
  memset(&sockaddr_in_server, 0, len);     // забьем всю структуру нулем

// RAW0 - создание
int raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);  // - для ip и udp


char recvbuf[DATAGRAM_LEN];

struct sockaddr_in saddr;

printf("NETWORK RAW Клиент запущен и слушает   сокет %d:\n",raw_sock);


//0 контейнер - строка сообщения
 int sendlen=0;
 strcpy(SendString,"Hello!"); // - голое сообщение которое посылаем серверу
 strcpy(GetPointer(0,msg_send,STR_SIZE_MAX,strlen(SendString),&sendlen),SendString);//засунем в конец буфера нашу строку

//1 Контейнер UDP
// Формируем заголовок UDP   
	
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(PORT); // random client port
	
	if (inet_pton(AF_INET, DEST_IP, &saddr.sin_addr) != 1)
	{
		printf("source IP configuration failed\n");
		return 1;
	}

   

   p_udphdr=(struct udphdr *)GetPointer(0,msg_send,STR_SIZE_MAX,sizeof(struct udphdr),&sendlen);
   SetUDPH(p_udphdr,MY_PORT,PORT,sendlen); //заполним структуру
   
   
   ///
   // заголовок IP
  int flag=1;
  setsockopt(raw_sock,        IPPROTO_IP, IP_HDRINCL  , &flag, sizeof(flag)); //level=IPPROTO_IP 


  p_iphdr=(struct iphdr *) GetPointer(0,msg_send,STR_SIZE_MAX,sizeof(struct iphdr),&sendlen);
  SetIPH(p_iphdr,4,5,0,0,0,9999, 8080,sendlen,MY_IP,DEST_IP);

  //заголовок eth
  
  //0 - используем другой сокет
  //1 - поправить контрольную сумму в ip
  // p_ethhdr=(struct ethhdr *) GetPointer(0,msg_send,STR_SIZE_MAX,sizeof(struct ethhdr),&sendlen);
  
  
  
  //char *sendPointer=(char *)p_udphdr;         // укажем на начало сообщения на отправку
   char *sendPointer=(char *)p_iphdr;          

 //====================================================================================================  
  // for(int i=0;i<sendlen;i++) printf("[%02d] %03d, (%c)\n",i, sendPointer[i],sendPointer[i]);
   
  /// Блок передачи сообщения 
   printf("Всего будет переданно %d байт \n",sendlen);
   int realsend= sendto(raw_sock, sendPointer,sendlen ,     0, (struct sockaddr *)&saddr, len);
   printf("сообщение передано (%d ,байт)\n",realsend);
   if(realsend==-1) {perror("send to");printf("error line %d\n",__LINE__);}

// Прием Сообщения
  p_udphdr=(struct udphdr *)msg_recv;
  int i=0;
 while(i<2){
 int received = recvfrom(raw_sock, msg_recv, sizeof(msg_recv),     0, (struct sockaddr *)&saddr, plen);
   if(received>0){
   
    printf("---------------------------------------------------\n");
    printf("[%d байтов]получено, (%s ....)\n",received,recvbuf);
   int hlen=20;
    p_udphdr=(struct udphdr *)(msg_recv+hlen);  // отступили TCP заголовок
   /*
    printf("[Source port]=%d\n",ntohs(p_udphdr->source));
    printf("[Dest port]=%d\n",ntohs(p_udphdr->dest));
    printf("[len]=%d\n",ntohs(p_udphdr->len));
    printf("[Chek]=%d\n",ntohs(p_udphdr->check));
    */
    hlen+=8;//отступили UDP заголовок
    if(MY_PORT==ntohs(p_udphdr->dest)) {printf("[сообщение от сервера] :%s\n",(msg_recv+hlen));break;};
    
  
    i++;
    
    }
     }



  return 0;
}


///////////////////////////////////////////////////////////////////////////////////  РАЗДЕЛ функций

/*вернет указатель куда будем писать объект размером len
bufsze - физический размер буфера
также вернем в sendlen текущий размер на отправку
*/
char * GetPointer(int init, char *BUF,int bufsize, int len, int* sendlen)
{
static int index=1;  //сколько байт уже записано с поправкой на нулевую индексацию
int ret; // индекс откуда будем писать
if(init) {index=1;return NULL;}
ret=(bufsize-index-len);
if(ret <0) return NULL;
index+=len;
*sendlen=index-1;
return (BUF+ret);
}

int CheckError(int err_n, char *err_str, int line) {
  if (err_n == -1) {
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

/*
Функция установит 8 байтный заголовок UDP

*/
void SetUDPH(struct udphdr *p_udphdr,int MYPORT, int DPORT, int sendlen)
{
p_udphdr->source=CheckError(htons(MYPORT),"MYPORT:",__LINE__);
p_udphdr->dest=CheckError(htons(DPORT),"DPORT:",__LINE__);
p_udphdr->len=htons(sendlen);
p_udphdr->check=0;
return;
}

void SetIPH(struct iphdr *p_iphdr,int v,int ihl, int bit_Frag1,int bit_Frag2,int Fofset, int MYPORT, int DPORT, int sendlen, char *my_ip, char* dest_ip)
{
p_iphdr->ihl=ihl;        //4 бита- размер , обычно =5 (5*4=20)   размер кодируется по 4 байта
p_iphdr->version=v;      //4-бита -версия
p_iphdr->tos=0;          //1 байт - он же ds - обычно 0, приоритет для дорогостоящего оборудования, DS - диференцируемый сервис
p_iphdr->tot_len=sendlen;//2 байта размер вместе с заголовком
p_iphdr->id;             //2 байта -идентификация пакета связанная с MTU
//p_iphdr->frag_off ;    //1-байт - первые 3 бита фрагментация:
                                     //1 бит резерв=0, 
                                     //2 бит =если 1 - не фрагментировать у нас =0, 
                                    //3 бит - есть еще фрагменты, у нас тоже будет=0
                                    // остальные биты - офсет
  //установим флаги
  p_iphdr->frag_off=bit_Frag1;
  (p_iphdr->frag_off)<<1;
  p_iphdr->frag_off=(p_iphdr->frag_off)&bit_Frag2; 
  (p_iphdr->frag_off)<<5;
  p_iphdr->frag_off=(p_iphdr->frag_off)&Fofset; 
  //                                  
  p_iphdr->ttl=255;   // кол-во маршрутизаторов
  p_iphdr->protocol=17;//код протокола UDP
  p_iphdr->check=0;// 2 байта- заполниться автоматически
  //p_iphdr->saddr=;// 4 байта Мой IP адрес - заполнятся автоматически
  //p_iphdr->daddr=;// 4 байта IP адрес получателя - заполнятся автоматически
  p_iphdr->saddr=inet_addr(MY_IP);// 4 байта Мой IP адрес
  p_iphdr->daddr=inet_addr(DEST_IP);// 4 байта IP адрес получателя


  	

return;
}
