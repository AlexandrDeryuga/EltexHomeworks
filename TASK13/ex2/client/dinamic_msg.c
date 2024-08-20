/*
модуль ведет поток сообщений - выделяет память под строки, и хранит строки подряд,
запоминать индекс/"указатель" на строку - дело клиента модуля, на каждое записываемое сообщение 
функция будет возвращать этот индекс.
*/

#include "dinamic_msg.h"



int  AddNewMessage (RoomFileType *RoomMSG , char * str, int construct, char * Name, int privat)    // construct 0 добавить, 1 - просто создать, -1  - очистить память
{ 
char *ptr=NULL;
int msg_len=0;
if(str) {msg_len=strlen(str);} 
if(!RoomMSG) return -1;


if(-1==construct)    //очистим память если это не первый запуск
 {
 if(RoomMSG->ptr) {free(RoomMSG->ptr);RoomMSG->ptr=NULL;};
 RoomMSG->msg_counter=0;
 RoomMSG->pages=0;
 RoomMSG->next_msg=0;
 RoomMSG->private_chat=0;
 RoomMSG->max=0;
 RoomMSG->sending=0;
 RoomMSG->to_id=0;
 //RoomMSG->last_print=0;
 RoomMSG->id=0;
 //strcpy(RoomMSG->Name,Name);
 return -1;
 }

if(RoomMSG->msg_counter>=MAX_ROOMMSG) return -1;

if (1==construct) {
   RoomMSG->ptr=(char *) calloc(PAGE_SIZE,sizeof(char));  //первичное выделение памяти
   if(RoomMSG->ptr) { RoomMSG->max= PAGE_SIZE-20;  //оставим байт 20 на всякий случай.
                     
                      RoomMSG->msg_counter=0;
                      RoomMSG->pages=1;
                      RoomMSG->next_msg=0;
                      //RoomMSG->last_print=0;
                      RoomMSG->id=0;
                      RoomMSG->sending=0;
                      if(Name) strcpy(RoomMSG->Name,Name);
                      RoomMSG->private_chat=privat;  //- об этом поле должен позаботиться клиент
                      return 0;
                                    
                   }
                else { return -1;}  //не удалось выделить память
  return -2;
 }
ptr=NULL;
while ((RoomMSG->next_msg+msg_len)>=RoomMSG->max)  //памяти недостаточно, нужно будет нарастить
 {RoomMSG->pages++;
 ptr= (char *)realloc(RoomMSG->ptr, sizeof(char)*PAGE_SIZE*RoomMSG->pages); 
 
 if(ptr) {RoomMSG->ptr=ptr;
          for(int k=RoomMSG->next_msg;k<=RoomMSG->max;k++) *(ptr+k)=0;
          RoomMSG->max+= PAGE_SIZE;   
          } else{ RoomMSG->pages--; return -1;}
 }
                                // теперь достаточно памяти
strcpy((RoomMSG->ptr+RoomMSG->next_msg),str);
 
RoomMSG->Messages[RoomMSG->msg_counter]=RoomMSG->next_msg;
RoomMSG->msg_counter++;

RoomMSG->next_msg+=msg_len+2;  
return RoomMSG->Messages[RoomMSG->msg_counter-1];
}


