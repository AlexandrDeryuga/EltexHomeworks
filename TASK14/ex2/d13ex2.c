#include <termios.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <panel.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "d13ex2.h"

#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>

#include "dinamic_msg.h"
#include "interface.h"

#include "msg_by_sm.h"

#define SYSBUF 250
#define BIGBUF 1000
#define MAX_ROOMS 100  // максимальное кол-во клиентов

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int RoomsCount=0;
int ExitFlag=0;
WINDOW *logw,*abonentw,*msgw;

typedef struct  SystemMsgLister                  // создадим пока пару шаблонов, можно было и одним обойтись
{  
//int qId1,qId2;

myQsys_type *Qsys;
myQmsg_type *Qmsg;

}SystemMsgLister;

typedef struct  MsgLister
{  
//int qId1; 
//int qId2;
myQsys_type *Qsys;
myQmsg_type *Qmsg;

}MsgLister;


void wprintQ(WINDOW *wnd,myQsys_type *Q)
{
wprintw(wnd, "q-print>, count=%d\n",Q->mcount);
for (int i=0;i<6;i++) 
 wprintw(wnd, "%d# type=%2ld prior=%2ld wait=%d  message>%s<\n",i,Q->SM_system[i].mType,Q->SM_system[i].priority,Q->SM_system[i].wait,Q->SM_system[i].mText);
 wrefresh(wnd);
 refresh();
}



/////////////////////////////////////////глобальные
/*
struct msgbuf {
   long mType;
   char mText[SYSBUF];
};

struct msgBigBuf {
   long mType;
   char mText[BIGBUF];
};
*/
RoomFileType Rooms[MAX_ROOMS];  // массив комнат (каждому клиенту своя, и одна общая под номером 0, 1 системные сообщения - лог)

int C1,C2, VIEW1, VIEW2,VIEW3,VIEW4 ; //переменные определяющие размеры окон(панелей навигатора) и область прокрутки
int fresh_all=0;
//char InputMessage[1000];
int cursor1=0,cursor2=0,cursor3=0,cursor4=0, selected=1; //курсоры прокрутки и выделенный абонент


RoomFileType * FindAbonent(unsigned long id)  // вернем ссылку на комнату по ее id
{
int i=0;
while (i<RoomsCount)
 {
  if(Rooms[i].id==id) return &Rooms[i];
  i++;
 }
 return NULL;
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


/*
// эта функция - поток, будет слушать системные сообщения
//
//  сообщения на регистрацию от клиентов, и сообщения команды от головной программы. (exit, может еще чего)
// при регистрации нового клиента, создается комната для него, и рассылка всем клиентам - комнату нового клиента, а этому клиенту список всех комнат
*/
void *SystemMsgFunc(SystemMsgLister *d)
{  
    
    //  Будем использовать SYSTEM V очереди

char txtBuf[SYSBUF];
   //int qId;                                      //идентификаторы очереди
   key_t key;                                   // уникальные ключ для очереди
  
  //struct msgbuf msg, buf;   // одно сообщение на прием и отправку
  msgbuf msg,buf;
  msgBigBuf msg2;
  
  struct msqid_ds msgCtlBuf; 
  char str[SYSBUF];
  char *s;
  int PidClient=0;
  
   int ClientNumber=1;
       wprintw(logw,"SERVER: th1 -SystemMsg: started\n");wrefresh(logw);
    
  //while(1);  //!!! затычка
  
  
  
    //////////////////////////////////////////////////////////////////////////////////////
      msg.mType = 1;  //тип входящих сообщений - "регистрация" от клиента
   while(1){ 
       msg.mType = 1;
     //  if ( msgrcv( d->qId1, &buf, sizeof msg.mText, msg.mType, 0 ) == -1 )   //
      int tmp=Qsys_msgrcv( d->Qsys, &buf,1, 0.1, msg.mType );
      if ( tmp <0 )              //int sys_msgrcv( myQsys_type * Q, &buf, chat wait ,float sleeptime, int type );
     { perror( "client: msgrcv failed:" );
        wprintw(logw,"th1 client: msgrcv failed\n");wrefresh(logw);
     }
   else
   {//wprintw(logw,"_while1 %d\n",__LINE__); wrefresh(logw);while (1); 
      //printf( "client: Получено сообщение от сервера = %s\n", buf.mText );
      wprintw(logw,"th1 -SystemMsg: incoming sysmsg..\n");wrefresh(logw);
      
    //  wprintw(logw,"_while1 %d [%s] tmp=%d\n",__LINE__,"buf",tmp); wrefresh(logw);while (1);
      
      
      
      
      if(0==strcmp("exit_now",buf.mText)) break; //спецкоманды
      
      //F1 void clientReg(); - кандидат на функцию
      if(RoomsCount<MAX_ROOMS){
         RoomsCount++;
         
         sscanf(buf.mText,"%d %s",&PidClient, str);
          pthread_mutex_lock( &mutex );///add1  
          
                                              // вид сообщения [ PID имя_клиента ]    приват - это на будушее  для p2p комнат Клиент-server-Клиент
         AddNewMessage(&Rooms[RoomsCount-1],buf.mText,1,str,0); // -конструктор!
         Rooms[RoomsCount-1].id=PidClient;
         sprintf(Rooms[RoomsCount-1].Name,"Client_%d",ClientNumber);
         ClientNumber++;
         
           printClients(abonentw,VIEW2);
           pthread_mutex_unlock( &mutex );///add1    
           
        //////////////////////////////////////////////////////////////////////////////////////////// регистрация завершина
         
         
         //
         //дальше отправим в очередь для этого клиента всех абонентов (комнаты)
         
         
        wprintw(logw,"SEVER:th1 -SystemMsg: reg msg->send rooms..\n");wrefresh(logw);
             
         wprintw(logw,"pid=%u   pidlast=%lu\n",PidClient,Rooms[RoomsCount-1].id); wrefresh(logw);
         msg.mType = PidClient;     //кому отправляем - последнему добавленному
       
       for( int ii=0;ii<(RoomsCount-1);ii++){ 
        
            sprintf(msg.mText,"%lu %s",Rooms[ii].id,Rooms[ii].Name);
                       
            wprintw(logw,"SERVER: th1 msgsnd to[%ld]>%s\n",msg.mType,msg.mText);wrefresh(logw);
            //if ( msgsnd( d->qId1, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) 
            if ( Qsys_msgsnd( d->Qsys, &msg, 0,0.1 ) < 1 )
            {
            perror( "client: msgsnd failed:" );
      // exit( 3 ); //ноувайт вроде бы в ошибку должен выпадать,поэтому эксит нельзя, проверим потом
       }
       sleep(0.1);
    }//закончили рассылку, далее надо отправить всем кроме нового инфу о нем
    
    // теперь мы должны отослать клиенту все сообщения из общей комнаты
    //сделаем это через мета команду  в процесс 2 [addm кому_сообщение]
   
   
     /////////////////////////// блок отсылки
  
  
     msg2.mType=1;
     sprintf(msg2.mText,"addm %ld", Rooms[RoomsCount-1].id);// отправка последнему добавленному ну и по идее это PidClient
     wprintw(logw,"Server th1 to th2[%ld]>%s\n",msg2.mType,msg2.mText);wrefresh(logw); 
     //if ( msgsnd( d->qId2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 )
     if ( Q_msgsnd( d->Qmsg, &msg2, 0,0.1 ) < 0 )
     {
                                                                          perror( "server: msgsnd failed:" );
                                                                          exit( 3 );
                                                                          } else{   };
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
   
   
    }  else {  //макс число абонентов
  
            }
    }
  sleep(0.1);  
  }
   //pthread_mutex_lock( &mutex ); 
  
   //pthread_mutex_unlock( &mutex );
   
      /*if ( msgctl( d->qId1, IPC_RMID, &msgCtlBuf ) == -1 )
      {             //удаляем очередь
      perror( "server: msgctl failed:" );
      exit( 4 );
   }*/
   sys_unlink(d->Qsys);
   
}


// эта функция - поток, будет слушать сообщения от клиентов
void *MsgFunc(MsgLister *d)
{  
    
    //  Будем использовать SYSTEM V очереди

char txtBuf[SYSBUF];
 
  struct msgBigBuf msg, buf;                      // одно сообщение на прием и отправку
  struct msqid_ds msgCtlBuf; 
  char str[BIGBUF];
  char *s;
  int i;
  
  
  unsigned long PidClient=0,id=0;
                                                   
      msg.mType = 1;  //тип сообщений - сообщение  [id message]
    
    wprintw(logw,"Sever th2 started\n");wrefresh(logw);
    
    while (1){
        msg.mType = 1; 
       //if ( msgrcv( d->qId2, &buf, sizeof msg.mText, msg.mType, 0 ) == -1 )   //  получим сообщения типа [id  message] и добавим его в соответствующую комнату 
       if ( Q_msgrcv( d->Qmsg, &buf,1, 0.1, msg.mType ) < 0 ) 
      perror( "client: msgrcv failed:" );
   else
   {
        wprintw(logw,"SEVER th2> message incoming..\n");wrefresh(logw);
      if(0==strcmp("exit_now",buf.mText)) break;                         //мета команда 1 - exit
      
      
      //pthread_mutex_lock( &mutex );  //add2
       strcpy(str,buf.mText);                        //мета команда 2 -  addm - разослать клиенту все сообщения из общей комнаты                        
       s=strtok(str," "); // первый токен
       if(0==strcmp("addm",s)) {
         
       s=strtok(NULL," ");
        
        id=atol(s);
         wprintw(logw,"SEVER th2>ADDM id=%ld,msg_counter=%d\n",id,Rooms[0].msg_counter);wrefresh(logw);
        i=0;
        pthread_mutex_lock( &mutex );  //add2
        
         //AddNewMessage(&Rooms[0],"test",0,NULL,0);///
         
         //sprintf(msg.mText,"%ld %s",id ,Rooms[0].ptr);// отправка последнему добавленному ну и по идее это PidClient
        // wprintw(logw,"Server th2 (addm) to[%ld]>%s\n",msg.mType,Rooms[0].ptr);wrefresh(logw); sleep(1);
 
 
 while (i<Rooms[0].msg_counter){ 
        
         /////////////////////////// блок отсылки
            
             
         msg.mType=id;// кому    Rooms[0].Messages[Rooms[0].msg_counter-Rooms[0].id,Rooms[msgptr);// отправка последнему добавл(енному ну и +по идее это PidClient
         sprintf(msg.mText,"%ld %s",id ,Rooms[0].ptr+Rooms[0].Messages[i]);
         wprintw(logw,"Server th2 addm to[%ld]>%s\n",msg.mType,msg.mText);wrefresh(logw);
    //if ( msgsnd( d->qId2, &msg, sizeof (msg.mText), IPC_NOWAIT ) == -1 ) 
    if ( Q_msgsnd( d->Qmsg, &msg, 0,0.1 ) < 0 ) 
    {
                                                                          perror( "server: msgsnd failed:" );
                                                                          exit( 3 );
                                                                          } else{   };
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////
        i++;
        
        }// end while
        pthread_mutex_unlock( &mutex );  //un add 2
        continue;  
       } //addm
       
       
      
      //это сообщение от главного потока сервера
      if(0==strcmp("sends_msg",buf.mText)) { // тупо разослать всем последнее новое сообщение от сервера в общую комнату, его уже добавил основной поток 
     
     pthread_mutex_lock( &mutex );
     wprintw(logw,"SEVER th2> sends_msg\n");wrefresh(logw); //sleep(10);
     for(int ii=0;ii<RoomsCount;ii++){
       msg.mType =Rooms[ii].id;
         if(msg.mType>10){/// все что ниже могут быть служебными
      //wprintw(logw,"th2 msgsnd to[%ld]>%s\n",msg.mType,msg.mText);wrefresh(logw);
       sprintf(msg.mText,"%u %s", msg.mType,(Rooms[0].ptr+Rooms[0].Messages[Rooms[0].msg_counter-1])); //адрес + смещение для последнего сообщения в комнате 
       wprintw(logw,"th2 msgsnd to[%ld]>%s\n",msg.mType,msg.mText);wrefresh(logw);
         //if ( msgsnd( d->qId2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) 
         if ( Q_msgsnd( d->Qmsg, &msg, 0,0.1 ) <0 )
         {
                                                                          perror( "server: msgsnd failed:" );
                                                                          exit( 3 );
                                                                          }
         }
      }
   printChat(msgw,-1,VIEW1,&Rooms[0]);
   pthread_mutex_unlock( &mutex );
   continue;
   }                 /// конец send_msg
   
   
      pthread_mutex_lock( &mutex );
          
        //AddNewMessage(buf.mText,0);  // вид сообщения [pidfrom pid message]    добавить сообщение в комнату
                                       //     и разослать всем пришедшее сообщение  преобразовав [Имя> message] 
                                       // если пид =0 то общая 
        
         id=0;   int idfrom=0; 
        strcpy(str,buf.mText);
        sscanf(buf.mText,"%d %ld",&idfrom, &id);
        //  wprintw(logw,"%d %d %s\n",idfrom, id, buf.mText);
         char *x;
        // if(!id) id=1;
         
          sprintf(msg.mText,"%s> %s\n",FindAbonent(idfrom)->Name, parcer(buf.mText,2));  //клиенту уже придет в таком виде, отправить так это задача сервера
          wprintw(logw,"th2 reposted1: from [%d] to [%ld]>%s\n",idfrom,id,msg.mText);wrefresh(logw);
        
        
        //sleep (12);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         
       //  for(i=0;i<RoomsCount;i++) 
           if(id) {                                            //добавить сообщение в нужную комнату преобразовав его
           
              if(-1!=AddNewMessage(&Rooms[i],msg.mText,0,NULL,0)) {// если успешно добавили то отправка последнего сообщения из комнаты с эти id
                                                                            msg.mType=id;
                                                                           //sprintf(msg.mText,"%s",(Rooms[i].ptr+Rooms[i].Messages[Rooms[i].msg_counter-1]));
                                                                           //printChat(wtxt1,-1,VIEW1,&Rooms[0]);  
                                                                          perror( "server: msgsnd failed:" );
                                                                          exit( 3 );
                                                                          
                                                                          wrefresh(msgw);
                                                                
                                                                          
                                                                           } else{   };
                                                                   
           } 
           
           else if(-1!=AddNewMessage(&Rooms[0],msg.mText,0,NULL,0)) {   printChat(msgw,-1,VIEW1,&Rooms[0]);   // для общей комнаты
                                                                           
                                                                      
                                                                      for(int ii=1;ii<RoomsCount;ii++){     //разошлнм всем КЛИЕНТАМ последнее            
                                                                        
                                                                          // блок отсылки
                                                                          msg.mType=Rooms[ii].id;
                                                                          sprintf(msg.mText,"%d %s",idfrom,Rooms[0].ptr+Rooms[0].Messages[Rooms[0].msg_counter-1]);
                                                                           wprintw(logw,"th2 reposted last: to[%ld]>%s\n",msg.mType,msg.mText);wrefresh(logw); 
                                                                          //if (msgsnd( d->qId2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) 
                                                                          if ( Q_msgsnd( d->Qmsg, &msg, 0,0.1 ) <0 )
                                                                          {
                                                                          perror( "server: msgsnd failed:" );
                                                                          exit( 3 );
                                                                          } else{   };
                                                                         ////////////////////////////////////////
                                                                                                                                           
                                                                      }
                               
                                                                      
          }
          
          pthread_mutex_unlock( &mutex );
          sleep(0.1);
         
         }
      
    
    
  //}
  sleep(0.1);
 }   
    
   //pthread_mutex_lock( &mutex ); 
   //AvailableShops--;
   //Shp->Free=0;
   //pthread_mutex_unlock( &mutex );

 /*if (msgctl(d->qId2, IPC_RMID, &msgCtlBuf ) == -1 ) 
 {             //удаляем очередь
      perror( "server: msgctl failed:" );
      exit( 4 );
   }*/
msg_unlink(d->Qmsg);

}



int main()
{
pthread_t tid[NUM_THREADS];
initscr();
//char *w1dir,*w2dir;
WINDOW * wnd1,*wnd2,*wnd3,*wnd4;     //окна с рамками
WINDOW * wtxt1,*wtxt2,*awtxt,*wtxt3, *wtxt4; //подокна под текст
struct winsize size;
start_color();
//refresh();
int i=0,activewindow=0;
char ch;



SystemMsgLister SML_data; // аргументы для потоков
MsgLister ML_data;

//  ИНИЦИАЛИЗИРУЕМ Системные комнаты 

//for(i=0;i<MAX_ROOMS;i++) Rooms[i]=(RoomsFileTypeNULL;
Rooms[0];// общая комната
Rooms[1];// системная информация

AddNewMessage (&Rooms[0] , NULL, 1,"COMMON_ROOM",0);    // construct 0 добавить, 1 - просто создать, -1  - очистить память, private=0 -это абонент, показать в списке
Rooms[0].id=0;Rooms[0].msg_counter=0;
//AddNewMessage (&Rooms[1] , NULL, 1,"SYSLOG",1);    // construct 0 добавить, 1 - просто создать, -1  - очистить память, private=1, это чат с абонентом,                      
//Rooms[1].id=10;                                              //не дублировать отображение


RoomsCount=1;
 
 


/////////////////////////////////////ПОСТРОЕНИЕ ОКОН
initscr();
signal(SIGWINCH, sig_winch);
curs_set(FALSE);
start_color();

noecho();
Resolve();
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //size.ws_row, size.ws_col теперь доступны

int col1,row1,col2,row2,col3,row3,col4,row4;
col1=3*size.ws_col/4;
row1=size.ws_row-16;

col2=size.ws_col-col1;
row2=size.ws_row;

row3=8;
col3=col1;
row4=8;
col4=col1;



wnd1 = newwin(row1,col1, 0, 0);                   // окно комнаты
wnd2 = newwin(row2,col2, 0, col1);     // окно клиентов
wnd3 = newwin(row3,col3,row1, 0);                 // окно отправки сообщения
wnd4 = newwin(row4,col4,row1+row3, 0);                // служебное окно

wtxt1= derwin(wnd1, row1-2,col1-3, 1, 1);     // текстовые области для окон        
wtxt2= derwin(wnd2, row2-2,col2-3, 1, 1);    //
wtxt3= derwin(wnd3, row3-2,col1-3, 1, 1);           //
wtxt4= derwin(wnd4, row4-2,col1-3, 1, 1);          //

VIEW1=row1-3;   // установим сколько строк мы можем напечатать в окно
VIEW2=row2-3;
VIEW3=row3-3;
VIEW4=row4-3;

init_pair(1, COLOR_WHITE, COLOR_BLACK);  //список Директорий
init_pair(2, COLOR_YELLOW, COLOR_BLUE);  //рамка
init_pair(3, COLOR_GREEN, COLOR_BLACK);   //
init_pair(4, COLOR_BLUE, COLOR_YELLOW);  //Активное окно
init_pair(5, COLOR_BLUE, COLOR_YELLOW);  //курсор
init_pair(6, COLOR_BLUE, COLOR_GREEN);  // окно ввода



scrollok(wtxt1,TRUE);
scrollok(wtxt2,TRUE);
scrollok(wtxt3,TRUE);
scrollok(wtxt4,TRUE);

wattron(wnd1, COLOR_PAIR(2));
wattron(wnd2, COLOR_PAIR(2));
wattron(wnd3, COLOR_PAIR(4));
wattron(wnd4, COLOR_PAIR(6));

box(wnd1, '|', '-');
box(wnd2, '|', '-');
box(wnd3, '|', '-');
box(wnd4, '|', '-');

refresh();
wrefresh(wnd1);
wrefresh(wnd2); 
wrefresh(wnd3);
wrefresh(wnd4);

logw=wtxt3;        // глобальные для процессов
abonentw=wtxt2;
msgw=wtxt1;

//////////////////////////////////////////////////////////////////////////////////////ТЕСТ СЕКЦИЯ ДИНАМИЧЕСКОГО МОДУЛЯ
/*
char TEXT[45000];char TEXT2[45000];TEXT[0]=0;TEXT2[0]=0;
for (i=0; i<1000;i++) { 
 sprintf(TEXT,"[%d]",i);
 strcat(TEXT2,TEXT);
 if(AddNewMessage (&Rooms[0] , TEXT2, 0,NULL,0)<0) {
     wprintw(wtxt1,"!cannot allocate memory!");
     wrefresh(wtxt1);getch();
    }
printChat(wtxt1,-1,VIEW1,&Rooms[0]);
}
  wprintw(wtxt1,"Dinamic test: ptr: %lu , MCount=%d  next_msg_index=%lu",(unsigned long)Rooms[0].ptr,Rooms[0].msg_counter, Rooms[0].next_msg);
 wprintw(wtxt1,"\nDinamic test - this is last message\nDinamic test complete - added 1000 messags in this room, press any key to start!");
 wrefresh(wtxt1);getch();
 AddNewMessage (&Rooms[0] , NULL, -1,NULL,0);  //отчищение памяти
 wprintw(wtxt1,"\ndelete all messages: ptr: %lu , MCount=%d  next_msg_index=%lu",(unsigned long)Rooms[0].ptr,Rooms[0].msg_counter, Rooms[0].next_msg);
 wrefresh(wtxt1);getch();

//wprintw(wtxt1,"Starting windows1\n"); for(int ii=0;ii<1025;ii++) //wprintw(wtxt1,"test lines %d qwertyuiop[asdfghjkl;'zxcvbnm,./qwertyuiop[]asdfghjkl;'zxcvbnm,./\n",ii);
//wprintw(wtxt2,"Starting windows2\n"); for(int ii=0;ii<40;ii++) wprintw(wtxt2,"test lines %d\n",ii);
//wprintw(wtxt3,"Starting windows3\n"); for(int ii=0;ii<10;ii++) wprintw(wtxt3,"test lines %d\n",ii);
//wprintw(wtxt4,"Starting windows4\n"); for(int ii=0;ii<10;ii++) wprintw(wtxt4,"test lines %d\n",ii);
*/

refresh();
wrefresh(wnd1);
wrefresh(wnd2); 
wrefresh(wnd3);
wrefresh(wnd4);
wrefresh(wtxt1);
wrefresh(wtxt2);
wrefresh(wtxt3);
wrefresh(wtxt4);

refresh();

//getch();
awtxt=wtxt1;

  Panel(awtxt, "          Help   screen:            Scroll window: cusors in acive window,Tab - select window,                  Enter - for new line in mesenger,     Ins - -send message                   F5 for exit,        F1 this help");
                
wrefresh(awtxt);
//getch();




//////////////////////// секция общения с нитями

char txtBuf[SYSBUF];
char txt2Buf[BIGBUF];
   //int qId_th1,qId_th2;
   //key_t key_th1,key_th2; 
   //struct msgbuf msg, buf;
   msgbuf msg; //buf;
   msgBigBuf msg2;
   struct msqid_ds msgCtlBuf;
   
   myQsys_type *Qsys=NULL;
   myQmsg_type *Qmsg=NULL;
   
    Qsys=Qsys_open( "sysQ", 1); //1 - создать аналог очереди сообщений в разделяемой памяти
    Qmsg=Q_open(    "msgQ", 1);
    
/*
                                                     // генерируем ключи очередей
   if ( ( key_th1 = ftok( "/tmp", 1 ) ) == -1 ) {   //
      perror( "client: ftok failed:" );
      exit( 1 );
   }
   
     if ( ( key_th2 = ftok( "/tmp", 2 ) ) == -1 ) {//
      perror( "client: ftok failed:" );
      exit( 1 );
   }

  
   if ( ( qId_th1 = msgget( key_th1, IPC_CREAT | 0666 ) ) == -1 ) {   //создаем очередь сообщений для нитей
      perror( "server: Failed to create message queue:" );
      exit( 2 );
   }

   if ( ( qId_th2 = msgget( key_th2, IPC_CREAT | 0666 ) ) == -1 ) {  
      perror( "server: Failed to create message queue:" );
      exit( 2 );
   }*/

  //  wprintw(wtxt1,"Start THREADS..\n");
  //  wprintw(wtxt2,"Start THREADS..\n");
    wprintw(wtxt3,"SERVER Start THREADS..\n");
  //  wprintw(wtxt4,"Start THREADS..\n");
    
    wrefresh(wtxt1);
    wrefresh(wtxt2);
    wrefresh(wtxt3);
    wrefresh(wtxt4);
    refresh();
  //  getch();


//SML_data.qId1=qId_th1;SML_data.qId2=qId_th2;

SML_data.Qsys=Qsys;SML_data.Qmsg=Qmsg;
pthread_create(&tid[0], NULL, (void*)SystemMsgFunc,(void*)&SML_data);  // Создадим поток

ML_data.Qsys=Qsys;ML_data.Qmsg=Qmsg;
pthread_create(&tid[1], NULL, (void*)MsgFunc,(void*)&ML_data);  // Создадим поток



printChat(wtxt1, 0, 5, &Rooms[0] );
printClients(wtxt2,5);

int pushmessage=10;
///////////////////////////////////////////////////////////////
// тело основного потока


//while (!Update) { }; // дождаться ответки от процесса 1 , что бы понять сколько клиентов у нас (RoomsCount)   - это код для клиента

activewindow=1;
while(1)
{
printChat(wtxt1, -1, VIEW1, &Rooms[0] );
printClients(wtxt2,5);


/*if(fresh_all) {
  box(wnd1, '|', '-');
  box(wnd2, '|', '-');

  wrefresh(wnd1); 
  wrefresh(wnd2);  
  //refresh();
  fresh_all=0;
  continue;
 }
wrefresh(wnd1);
wrefresh(wnd2);*/


ch=navigation(wtxt1, activewindow,wtxt4);
 //wprintw(wtxt4,"next step\n");
//wprintQ(wtxt1,Qsys);wrefresh(wtxt1);getch();

if(ch==pushmessage) {
            sprintf(msg2.mText,"sends_msg");msg2.mType=1;
               wprintw(wtxt3,"SERVER menu push message\n");wrefresh(wtxt3);
            //if ( msgsnd( qId_th2, &msg, sizeof msg.mText,IPC_NOWAIT ) == -1 ) 
            if ( Q_msgsnd( Qmsg, &msg2, 0,0.1 ) <0 )
            {                             // посылаем сигнал на выход нашему потоку обработчика сообщений
            perror( "client: msgsnd failed:" );
       exit( 3 ); //ноувайт воде бы в ошибку должен выпадать,поэтому эксит нельзя, проверим потом
       }
}

if(ch==EXIT_NOW) break;
if (ch==CHANGE_PANEL) {
  activewindow++;
  if(activewindow>3) activewindow=1;
  
  wattron(wnd1, COLOR_PAIR(2));
  wattron(wnd2, COLOR_PAIR(2));
  wattron(wnd3, COLOR_PAIR(2));
  wattron(wnd4, COLOR_PAIR(6));
  
  switch(activewindow)
  {
   case 1:wattron(wnd1, COLOR_PAIR(4));
          break;
   case 2:wattron(wnd2, COLOR_PAIR(4));
          break;
   case 3: 
          wattron(wnd3, COLOR_PAIR(4));
          break;

  }
  box(wnd1, '|', '-');
  box(wnd2, '|', '-');
  box(wnd3, '|', '-');
  box(wnd4, '|', '-');

  wrefresh(wnd1);
  wrefresh(wnd2); 
  wrefresh(wnd3);
  wrefresh(wnd4);
  } 
   
}

/////////////////////////////////////////////////
// завершение работы
wprintQ(msgw,Qsys);
  msg.mType = 1;                
    
     sprintf(msg.mText,"exit_now");
            //if ( msgsnd( qId_th1, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) 
            if ( Qsys_msgsnd( Qsys, &msg, 0,0.1 ) <0 )
            {                             // посылаем сигнал на выход нашему потоку SystemMsg
            perror( "client: msgsnd failed:" );
      // exit( 3 ); //ноувайт воде бы в ошибку должен выпадать,поэтому эксит нельзя, проверим потом
       }
       wprintQ(msgw,Qsys);
       
         msg.mType = 1;                
 //leep(60);
     sprintf(msg.mText,"exit_now");
            //if ( msgsnd( qId_th2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) 
            if ( Q_msgsnd( Qmsg, &msg2, 0,0.1 ) <0 ){                             // посылаем сигнал на выход нашему потоку обработчика сообщений
            perror( "client: msgsnd failed:" );
       exit( 3 ); //ноувайт воде бы в ошибку должен выпадать,поэтому эксит нельзя, проверим потом
       }


 pthread_join(tid[0], NULL);
 pthread_join(tid[1], NULL);


delwin(wtxt1);           //очистка памяти
delwin(wtxt2);
delwin(wtxt3);
delwin(wtxt4);

delwin(wnd1);
delwin(wnd2);
delwin(wnd3);
delwin(wnd4);


wmove(stdscr, 0, 10);  // курсор
printw("Exit by Z key.. Memory was cleared.. >> Press any key to continue...<<");

//refresh();
getch();
endwin();
exit(EXIT_SUCCESS);
}




void Resolve()
{
struct winsize size;
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
C1=size.ws_col/2-2;
C2=size.ws_col/2-3;
//FilesVIEW=size.ws_row-4;
}


void sig_winch(int signo)
{
struct winsize size;
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

resizeterm(size.ws_row, size.ws_col);
}








