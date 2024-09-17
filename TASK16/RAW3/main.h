#include <err.h>
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
#include <netinet/ether.h>
#include <linux/if_ether.h>

#include <net/if.h>  //if_name2index

#include <features.h>   /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1 
#include <netpacket/packet.h>
#include <net/ethernet.h>       /* the L2 protocols */

#else

#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>     /* The L2 protocols */ 
#endif


////////////////////////////////////////////////
/*Домашка RAW3 - создание заголовка ETH+IP+UDP*/
////////////////////////////////////////////////

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

/* Заполнить eth заголовок и пересчитать чексумму для ip-заголовка*/

void SetETH(struct ethhdr * eth, struct iphdr * ip, char * my_mac, char * dest_mac, int iph_len);


unsigned short checksum(struct iphdr * iph, int iph_len); /* чексумма - заголовок может быть либо 20 либо 60 байт*/
int SetMAC(char *dest, char *MAC);/*считать из строки MAC адрес, и занести его по байтно по адресу dest (6 байт) */

/////////////////////////////////////////////////////////////////////////////////////////////////////////// Справочная информация

/*
sockaddr_ll является не зависимым от устройства адресом физического уровня.

    struct sockaddr_ll {
       unsigned short  sll_family;    //* Всегда AF_PACKET 
       unsigned short  sll_protocol;  //* Протокол физического уровня 
       unsigned short  sll_hatype;    //* Тип заголовка 
       unsigned char   sll_pkttype;   //* Тип пакета /
       unsigned char   sll_halen;     //* Длина адреса /
       unsigned char   sll_addr[8];   //* Адрес физического уровня 
    };
*/

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
Информация заголовка уровня соединения в общем формате предоставлена в sockaddr_ll. protocol - номер протокола в соответствии с IEEE 802.3 в сетевом порядке байтов. Список возможных протоколов приведен в файле <linux/if_ether.h> Если protocol содержит значение htons(ETH_P_ALL), то программой будут приниматься все протоколы. Все входящие пакеты этого типа протокола будут передаваться пакетному сокету до того, как они будут переданы протоколам, реализованным в ядре. 

 Когда вы посылаете пакеты, то достаточно определить только sll_family, sll_addr, sll_halen, sll_ifindex. Остальные поля должны быть равны 0. sll_hatype и sll_pkttype определяются для вас по принимаемым пакетам. Для привязки используются только sll_protocol и sll_ifindex.

ОДНАКО ЭТО НЕ РАБОТАЕТ !!!   работает htons(ETH_P_IP)
*/

