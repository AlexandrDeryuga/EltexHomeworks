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

#include "d13ex2_cl.h"

#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>

#include "dinamic_msg.h"
#include "interface_cl.h"

#define SYSBUF 250
#define BIGBUF 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


int RoomsCount=0;


WINDOW *logw,*abonentw,*msgw;


typedef struct  SystemMsgLister                  // создадим пока пару шаблонов
{  
int tmp;   

}SystemMsgLister;

typedef struct  MsgLister
{  
int tmp;   

}MsgLister;



/////////////////////////////////////////глобальные
struct msgBigBuf {
   long mType;
   char mText[BIGBUF];
};

struct msgbuf {
   long mType;
   char mText[SYSBUF];
};


char * parcer(char *str)  //верный
{
if(str==NULL) return NULL;
int i=0;
while (str[i]!=' ') {
                     if(str[i]==0) return NULL;
                    i++;
                    }
return &str[i+1];
}


RoomFileType Rooms[MAX_ROOMS];  // массив комнат (каждому клиенту своя, и одна общая под номером 0, 1 системные сообщения - лог)

int C1,C2, VIEW1, VIEW2,VIEW3,VIEW4 ; //переменные определяющие размеры окон(панелей навигатора) и область прокрутки
int fresh_all=0;
//char InputMessage[BIGBUF];
int cursor1=0,cursor2=0,cursor3=0,cursor4=0, selected=1; //курсоры прокрутки и выделенный абонент

 unsigned long My_Pid=0;
WINDOW * logw;
char ClientMessage[BIGBUF];

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



// эта функция - поток, будет слушать системные сообщения
void *SystemMsgFunc(SystemMsgLister *dwork)
{  
    
    //  Будем использовать SYSTEM V очереди

char txtBuf[SYSBUF];
   int qId;                                      //идентификаторы очереди
   key_t key;                                   // уникальные ключ для очереди
  
  struct msgbuf msg, buf;                      // одно сообщение на прием и отправку
  struct msqid_ds msgCtlBuf; 
  char str[SYSBUF];
                               
  int PidClient=0;
  
   //pthread_mutex_lock( &mutex ); 
  
  wprintw(logw,"CLIENT: th1 -SystemMsg: started\n");wrefresh(logw);
                                                      // генерируем ключи очереди
   if ( ( key = ftok( "/tmp", 1 ) ) == -1 ) {   
      perror( "client: ftok failed:" );
      exit( 1 );
   }
   
                                                           //создаем очереди
   if ( ( qId = msgget( key, IPC_CREAT | 0666 ) ) == -1 ) {  
      perror( "client: Failed to create message queue:" );
      exit( 2 );
   }
    
    
    //pthread_mutex_lock( &mutex );
      msg.mType = 1;  //тип сообщений - "регистрация" от клиента [мой_pid Name]  
                      //[мой_PID- приход нового нового участника [pid_клиента Name]
 
       sprintf( msg.mText, "%lu Client[%lu]", (long)getpid(),(long)getpid());         // отправить запрос регистрации
       My_Pid= (long)getpid();    
       wprintw(logw,"pid=%ld\n",My_Pid); wrefresh(logw);
       // wrefresh(logw);
   
   if ( msgsnd( qId, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
      perror( "client: msgsnd failed:" );
      exit( 3 );
   }
   
   
 msg.mType = getpid();
 unsigned long abonent_pid;
   //pthread_mutex_lock( &mutex ); 
  wprintw(logw,"SystemMsg:  hear %ld\n", msg.mType);
   wrefresh(logw);
    pthread_mutex_unlock( &mutex ); 
 
 
 int debug=0;
 
 while(1){ // pthread_mutex_lock( &mutex ); 
           //wprintw(logw,"SystemMsg:Rooms=%d", RoomsCount); wrefresh(logw);
           // pthread_mutex_unlock( &mutex ); 
          if ( msgrcv( qId, &buf, sizeof msg.mText, msg.mType, 0 ) == -1 )   //
     { perror( "client: msgrcv failed:" );}
   else
      { //получен новый участник   или команда      [pid Name]
        //pthread_mutex_lock( &mutex );
        if(0==strcmp("exit_now",buf.mText)) { break;}
        if(0==strcmp("print_debug",buf.mText)) { pthread_mutex_lock( &mutex ); 
                                                debug++;
                                                wprintw(logw,"CLIENT: th1 -SystemMsg: Debug= %d  RoomsCount= %d\n",debug,RoomsCount);
                                                wrefresh(logw);
                                                 pthread_mutex_unlock( &mutex );
                                                 continue;
                                               } 
       pthread_mutex_lock( &mutex ); 
        sscanf(buf.mText,"%lu %s",&abonent_pid,str);
        wprintw(logw,"CLIENT: th1 -SystemMsg: %lu %s\n",abonent_pid,str);wrefresh(logw);
        RoomsCount++;
        if(-1!=AddNewMessage(&Rooms[RoomsCount-1],NULL,1,str,0)){Rooms[RoomsCount-1].id=abonent_pid;}; //Добавляем клиента и новую его комнату 
         printClients(abonentw,VIEW2);
         
       pthread_mutex_unlock( &mutex );
      } 
       sleep(0.3);  
  }
   
   
   //pthread_mutex_lock( &mutex ); 
 
   //pthread_mutex_unlock( &mutex );
   
      if ( msgctl( qId, IPC_RMID, &msgCtlBuf ) == -1 ) {             //удаляем очередь
      perror( "client: msgctl failed:" );
      exit( 4 );
   }
   
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// эта функция - поток, будет слушать сообщения от клиентов
void *MsgFunc(MsgLister *dwork)
{  

char Buf[BIGBUF];
   int qId2;                                      //идентификаторы очереди
   key_t key2;                                   // уникальные ключ для очереди
  
  struct msgBigBuf msg, buf;                      // одно сообщение на прием и отправку
  struct msqid_ds msgCtlBuf; 
  char str[BIGBUF];
                               
  unsigned long PidClient=0;
  unsigned long id=0;
  char *x;
              
              // генерируем ключи очереди сообщений
   if ( ( key2 = ftok( "/tmp", 2 ) ) == -1 ) {   
      perror( "client: ftok failed:" );
      exit( 1 );
   }
   
                                                           //создаем очереди
   if ( ( qId2 = msgget( key2, IPC_CREAT | 0666 ) ) == -1 ) {  
      perror( "client: Failed to create message queue:" );
      exit( 2 );
   }
   
   
    
    while (1){
       msg.mType =getpid();   //тип сообщений - слушать те которые только мне 
        wprintw(logw,"CLIENT th2> wait message on [%d]",msg.mType);wrefresh(logw);
       if ( msgrcv( qId2, &buf, sizeof msg.mText, msg.mType, 0 ) == -1 )   //
      {perror( "client: msgrcv failed:" );sleep(5);}
   else
      { 
      //printf( "client: Получено сообщение от сервера = %s\n", buf.mText );
       pthread_mutex_lock( &mutex );
       wprintw(logw,"CLIENT th2> message incoming:%s\n",buf.mText);wrefresh(logw);
      // wprintw(msgw,"CLIENT th2> message incoming2:%s\n",buf.mText);wrefresh(msgw);
       sleep(1);

      if(0==strcmp("exit_now",buf.mText)) break; 
   //   if(0==strcmp("send_last",buf.mText))// нет, пусть это сделает основной поток
   
           // получили сообщение от сервера типа [Откого_id message]  Откого_id - это общий чат но в будущем могут быть и приваты
      strcpy(str,buf.mText);
      sscanf(str,"%lu %s",&id,str);
      x=parcer(buf.mText); 
      if(!id) id=1; // общая комната у нас 0, ей назначена 1ая очередь
      //sprintf(str,"%s> %s\n",FindAbonent(id)->Name,x);  уже придет в таком виде, это задача сервера
       
      
      AddNewMessage(&Rooms[0],x,0,NULL,0); // засунем в указанную в соообщении комнату  наше сообщение
      printChat(msgw, -1, VIEW1, &Rooms[0] );
      //wprintw(msgw,"Room Name=%d  %z",Room[0].Name, 
      wprintw(msgw, "testpoint");
      wrefresh(msgw);sleep(1);
      pthread_mutex_unlock( &mutex );
  }
  
  
  sleep(0.5);
 }   
    
   //pthread_mutex_lock( &mutex ); 
   //
   
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

//AddNewMessage (&Rooms[0] , NULL, 1,"COMMON_ROOM",0);    // construct 0 добавить, 1 - просто создать, -1  - очистить память, private=0 -это абонент, показать в списке
//AddNewMessage (&Rooms[1] , NULL, 1,"SYSLOG",1);    // construct 0 добавить, 1 - просто создать, -1  - очистить память, private=1, это чат с абонентом,                      
                                                  //не дублировать отображение


RoomsCount=0;
 
 


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

logw=wtxt3;
abonentw=wtxt2;
msgw=wtxt1;

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

//////////////////////////////////////////////////////////////////////////////////////ТЕСТ СЕКЦИЯ ДИНАМИЧЕСКОГО МОДУЛЯ
/*char TEXT[45000];char TEXT2[45000];TEXT[0]=0;TEXT2[0]=0;
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

char Buf[SYSBUF];
   int qId_th1,qId_th2;
   key_t key_th1,key_th2; 
   struct msgbuf msg, buf;
   struct msqid_ds msgCtlBuf;

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
   }

  //  wprintw(wtxt1,"Start THREADS..\n");
  //  wprintw(wtxt2,"Start THREADS..\n");
    wprintw(wtxt3,"Start THREADS..\n");
  //  wprintw(wtxt4,"Start THREADS..\n");
    
    wrefresh(wtxt1);
    wrefresh(wtxt2);
    wrefresh(wtxt3);
    wrefresh(wtxt4);
    refresh();
   // getch();



pthread_create(&tid[0], NULL, (void*)SystemMsgFunc,(void*)&SML_data);  // Создадим поток
pthread_create(&tid[1], NULL, (void*)MsgFunc,(void*)&ML_data);  // Создадим поток


msg.mType = getpid();
 sprintf(msg.mText,"print_debug");  
         if ( msgsnd( qId_th1, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
      perror( "server: msgsnd failed:" );
      exit( 3 );
   }
   





///////////////////////////////////////////////////////////////
// тело основного потока
//sleep(1);

//wprintw(wtxt1,"RoomsCount=%d",RoomsCount);wrefresh(wtxt1);
sleep(1);


pthread_mutex_lock( &mutex ); 

printChat(wtxt1, 0, 5, &Rooms[0] );
printClients(wtxt2,VIEW2);


int pushmessage=10;

int roomcheck=0;
//pthread_mutex_lock( &mutex ); 
roomcheck=RoomsCount;
pthread_mutex_unlock( &mutex );

/*
  msg.mType = getpid();
 sprintf(msg.mText,"print_debug");  
         if ( msgsnd( qId_th1, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
      perror( "server: msgsnd failed:" );
      exit( 3 );
   }*/


//roomcheck=RoomsCount;
activewindow=1;
while(1)
{



 pthread_mutex_lock( &mutex ); 
 printClients(wtxt2, VIEW2 );
 printChat(wtxt1, -1, VIEW1, &Rooms[0] );
if(RoomsCount!=roomcheck)
{RoomsCount=roomcheck;
 printClients(wtxt2, VIEW2 );
}
pthread_mutex_unlock( &mutex ); 
/*if(fresh_all) {
  box(wnd1, '|', '-');
  box(wnd2, '|', '-');

  wrefresh(wnd1); 
  wrefresh(wnd2);  
  //refresh();
  fresh_all=0;
  continue;
 }*/
wrefresh(wnd1);
wrefresh(wnd2);


ch=navigation(wtxt1, activewindow,wtxt4);

if(ch==pushmessage){
                            // отправим сразу в очередь на сервер
                        
                        msg.mType = 1;
                        sprintf(msg.mText,"%ld 0 %s",getpid(),ClientMessage);
                        ClientMessage[0]=0;
                         if ( msgsnd( qId_th2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
                         perror( "server: msgsnd failed:" );
                         exit( 3 );
                
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
   msg.mType = getpid();
  sprintf(msg.mText,"exit_now");  
         if ( msgsnd( qId_th1, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
      perror( "server: msgsnd failed:" );
      exit( 3 );
   }
           if ( msgsnd( qId_th2, &msg, sizeof msg.mText, IPC_NOWAIT ) == -1 ) {
      perror( "server: msgsnd failed:" );
      exit( 3 );
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




//////////////////////////////////////////////////////////////////////////interface cl

void Panel(WINDOW *wnd, char *s)
{
char tmp[2000];
strcpy(tmp,s); 
strcat(tmp,"                                                                                                       ");
WINDOW *subwnd,*subwndtxt;
subwnd = derwin(wnd, 8, 40, 3, 10);
subwndtxt = derwin(wnd, 6, 38, 4, 11);
wattron(subwnd, COLOR_PAIR(2));
wattron(subwndtxt, COLOR_PAIR(2));
box(subwnd, '|', '-');
wprintw(subwndtxt,"  %s \n", tmp);
box(subwnd, '|', '-');
wrefresh(subwndtxt);
wrefresh(wnd);
delwin(subwndtxt);
delwin(subwnd);
fresh_all=1;
wrefresh(wnd);//wnd
refresh();
//getch();
}

int navigation(WINDOW *awtxt,int aw,WINDOW *wtxt)
{int ch;
//enum{UP,DOWN,RIGHT,LEFT};

static int chCount=0;

static char InputString[MAX_CHARS];


if(chCount==0) InputString[0]=0; 
//ch = wgetch(awtxt);
ch=getch(); 

pthread_mutex_lock( &mutex );           /// RETURN не использовать изза мьютекса!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		switch(ch)
		{	case 9:		// Tab 
				pthread_mutex_unlock( &mutex );
				return CHANGE_PANEL;
				break;
			case 263:     //del удалить последний введенный символ
			case 127:
			        if(chCount>0){
			        chCount=chCount-1;InputString[chCount]=0;
			        
			        }
			        //return S_STAY;
			        break;
                        case 126:		
			case 331:     //ins  отправить строку 
			        if(chCount>0) InputString[chCount-3]=0; //срежем лишнее с инсерта
			      
			        //AddNewMessage (&Rooms[0], InputString, 0, NULL, 0);
			        
			        if(chCount>0){strcpy(ClientMessage,InputString);}
			        
			        InputString[0]=0; chCount=0;
			        printChat(awtxt, -1, VIEW1, &Rooms[0] ); //отобразить последние сообщения
			        
			        wclear(wtxt);
                                wprintw(wtxt,"%s<[%d]",InputString,InputString[chCount-1]); wrefresh(wtxt);
			        
			        int pushmessage=10;
			        pthread_mutex_unlock( &mutex );
			        return pushmessage;
			        break;
						
         		case KEY_UP:
         		case 65:
         		                if(aw==1) cursor1--;
         		                if(aw==2) selected--;
         		                if(aw==3) cursor3--;
         		                 chCount=chCount-2;
         		                 if(chCount>1) InputString[chCount]=0; //срежем лишнее с клавиш навигации
         		                 
            		       
				break;
                       
			case KEY_DOWN:
			case 66:
			
		    	                if(aw==1) cursor1++;
		    	                if(aw==2) selected++;
         		                if(aw==3) cursor3++;
         		                chCount=chCount-2;
         		                 if(chCount>1) InputString[chCount]=0; //срежем лишнее с клавиш навигации
			        
				break;
			case 10:	// Enter 
			        if(chCount<MAX_CHARS-1) {
		           InputString[chCount]='\n';
		           chCount++;
		           InputString[chCount]=0;
			} 
				//wprintw(wtxt,"\n");
				//return S_STAY;
				break;
			case KEY_F(1):
		
		wmove(stdscr, 0, 10);  //  установить курсор
                printw(">> Press any key to continue...<<");
		
                Panel(awtxt, "          Help   screen:            Scroll window: cusors in acive window,Tab - select window,                  Enter - for new line in mesenger,     Ins - -send message                   F5 for exit,        F1 this help");
                wrefresh(awtxt);
                getch();
                                      
			              
			return S_STAY;
			break;
			
			//case KEY_F(5):
			case 83:
			            return EXIT_NOW;
			            
			            break;
			default:
			                                         //отобразить в окно
			
		        if(chCount<(MAX_CHARS-1)) {
		           InputString[chCount]=(char)ch;
		           chCount++;
		           InputString[chCount]=0;
		           
			}
			wclear(wtxt);
			//wprintw(wtxt,"%s",InputString); 
			//wprintw(wtxt,"%c[%d]",ch,ch);  
			wrefresh(wtxt);
			          
			
		}  
/*		
 if(chCount<(MAX_CHARS-1)) {
		           InputString[chCount]=(char)ch;
		           chCount++;
		           InputString[chCount]=0;		
}*/		

wclear(wtxt);
wprintw(wtxt,"%s<[%d]",InputString,InputString[chCount-1]); 
//wprintw(wtxt,"%s[%d]",InputString,InputString[chCount-1]);
wrefresh(wtxt);
//wrefresh(awtxt);

pthread_mutex_unlock( &mutex );
return S_STAY;
//return EXIT_NOW;
}


void PrintMessage()  //просто заготовка
{
WINDOW * wnd;
wattron(wnd, COLOR_PAIR(2));
wnd = newwin(3,40, 10, 40);
wattron(wnd, COLOR_PAIR(2));

//wrefresh(wnd);
getch();
delwin(wnd);        
  refresh();
}


void printChat(WINDOW *wtxt, int cursor, int rows, RoomFileType *RoomMSG )   //cursor=-1 говорит что мы хотим видеть последнии сообщения
{
wclear(wtxt);
int i=0,j=0;
j=RoomMSG->msg_counter;
if(rows<RoomMSG->msg_counter) {i=cursor;j=cursor+rows;}

if(-1==cursor){
                j=RoomMSG->msg_counter; i=j-VIEW1;
                if(i<0) i=0;
               }   
//i=0;j=RoomMSG->msg_counter;

while (i < j)
{
wprintw(wtxt,"%s\n", (RoomMSG->ptr+RoomMSG->Messages[i]));
i++;
}

//for(i=0;i<Rooms[0].msg_counter;i++) wprintw(wtxt1,"%s\n", (Rooms[0].ptr+Rooms[0].Messages[i]));
wrefresh(wtxt);
refresh();
}

void printClients(WINDOW *wtxt, int rows )
{
wclear(wtxt);
int i=0,j=0;
wattron(wtxt, COLOR_PAIR(1));
j=RoomsCount;//кол-во абонентов/чатов храним в глобальной переменной

//wprintw(wtxt,"Client rooms =%d\n",RoomsCount);

if(rows<RoomsCount) {i=cursor2;j=cursor2+rows;}

if(selected<1) selected=1;
if(selected>RoomsCount) selected=RoomsCount;
if(cursor2>VIEW2) cursor2=VIEW2;
while (i < j)
{


if (i==(selected-1)) {
wattron(wtxt, COLOR_PAIR(6));
wprintw(wtxt,"%s[%lu]\n",Rooms[i].Name,Rooms[i].id);
wattron(wtxt, COLOR_PAIR(1));}
 else wprintw(wtxt,"%s[%lu]\n",Rooms[i].Name,Rooms[i].id);

i++;
}
wrefresh(wtxt);
refresh();
}












