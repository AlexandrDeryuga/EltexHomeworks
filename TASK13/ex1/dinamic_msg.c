/*
модуль ведет поток сообщений - выделяет память под строки, и хранит строки подряд,
запоминать индекс/"указатель" на строку - дело клиента модуля, на каждое записываемое сообщение 
функция будет возвращать этот индекс.
*/

#include "dinamic_msg.h"

#define PAGE_SIZE 100 

tipedef struct RoomFileType
{
char *ptr;
int msg_counter;
int pages;

static unsigned long max=0;// доступно байтов 
static unsigned long next_msg=0; //  указывает куда будем размещать след.строку
} RoomFileType

int  AddNewMessage (char * str, int construct)
{ 

static RoomFileType RoomMSG;

int msg_len;
static char * ptr=NULL;
 
msg_len=strlen(str);

if ((NULL==ptr)||(construct)) {
   RoomMSG.ptr=(char *) calloc(PAGE_SIZE,sizeof(char));  //первичное выделение памяти
   if(RoomMSG.ptr) { RoomMSG.a_bytes+= PAGE_SIZE;  //конструктор
                     RoomMSG.pages=1; 
                     msg_counter=0;
                     if(construct) return 0;
                   }
                else { return -1;} 
 }

while ((RoomMSG.next_msg+msg_len)>=RoomMSG.max)  //памяти недостаточно, нужно будет нарастить
 {
 ptr= (char *)realloc(ptr, sizeof(char)*PAGE_SIZE); 
 if(ptr) {
          RoomMSG.a_bytes+= PAGE_SIZE;   
          } else{ return -1;}
 }
                                // теперь достаточно памяти
strcpy(*(RoomMSG.ptr),str);
RoomMSG.next_msg+=msg_len+2;  
ptr=RoomMSG.ptr;

}


