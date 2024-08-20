#include "interface.h"

//int RoomsCount=0;
extern int RoomsCount;
extern RoomFileType Rooms[];
extern int C1,C2, VIEW1, VIEW2,VIEW3,VIEW4 ; //переменные определяющие размеры окон(панелей навигатора) и область прокрутки
extern int fresh_all;
extern int cursor1,cursor2,cursor3,cursor4, selected; 

//extern char ClientMessage[];


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
char s[MAX_CHARS];

if(chCount==0) InputString[0]=0; 
//ch = wgetch(awtxt);
ch=getch();

		switch(ch)
		{	case 9:		// Tab 
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
			        sprintf(s,"server> %s", InputString);
			        AddNewMessage (&Rooms[0], s, 0, NULL, 0);
			        InputString[0]=0; chCount=0;
			        printChat(awtxt, -1, VIEW1, &Rooms[0] ); //отобразить последние сообщения
			        wclear(wtxt);
                                wprintw(wtxt,"%s<[%d]m=%d",InputString,InputString[chCount-1]); wrefresh(wtxt);
			        			        
			        int push_message=10;			        
			        return push_message;
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
			
			case KEY_F(5):
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

wprintw(wtxt,"clints rooms =%d\n",RoomsCount);

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


