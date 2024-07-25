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

#include "d9ex2.h"
#include "scandir.h"




int C1,C2, FilesVIEW; //переменные определяющие размеры окон(панелей навигатора) и область прокрутки
int fresh_all=0;

int main()
{
char *w1dir,*w2dir;
WINDOW * wnd1,*wnd2;     //окна с рамками
WINDOW * wtxt1,*wtxt2,*awtxt; //подокна под текст
struct winsize size;
initscr();
signal(SIGWINCH, sig_winch);
curs_set(FALSE);
start_color();
//refresh();
int i=0,activewindow=0;

noecho();
Resolve();
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size); //size.ws_row, size.ws_col теперь доступны

init_pair(1, COLOR_WHITE, COLOR_BLACK);  //список Директорий
init_pair(2, COLOR_YELLOW, COLOR_BLUE);  //рамка
init_pair(3, COLOR_GREEN, COLOR_BLACK);   //файлы
init_pair(4, COLOR_BLUE, COLOR_YELLOW);  //курсор
init_pair(5, COLOR_BLUE, COLOR_YELLOW);  //курсор
init_pair(6, COLOR_BLUE, COLOR_GREEN);  //заголовок

wnd1 = newwin(size.ws_row,size.ws_col/2, 0, 0);
wnd2 = newwin(size.ws_row, size.ws_col/2-1, 0, size.ws_col/2+1);
wtxt1= derwin(wnd1, size.ws_row-2,size.ws_col/2-2, 1, 1);
wtxt2= derwin(wnd2, size.ws_row-2,size.ws_col/2-3, 1, 1);
scrollok(wtxt1,TRUE);

wattron(wnd1, COLOR_PAIR(2));
wattron(wnd2, COLOR_PAIR(2));

box(wnd1, '|', '-');
box(wnd2, '|', '-');

awtxt=wtxt1;
//wrefresh();
activewindow=1;


Panel(awtxt, "      Hello, I'm starting!      Navigation: cusors or w/s , Enter,         F5 or z for exit                   F1 this help");
wrefresh(awtxt);
getch();

Scan_Dir(wtxt1,READ,1,S_STAY,0);
Scan_Dir(wtxt2,READ,2,S_STAY,0);
Scan_Dir(wtxt1,W_FRESH,1,S_STAY,1);//сделаем активным
wrefresh(wnd1);
wrefresh(wnd2);  

 
 
 refresh();



i=1;
while(1)
{

if(fresh_all)
             {
box(wnd1, '|', '-');
box(wnd2, '|', '-');

Scan_Dir(wtxt1,W_FRESH,1,S_STAY,0);
Scan_Dir(wtxt2,W_FRESH,2,S_STAY,0);
Scan_Dir(awtxt,W_FRESH,activewindow,S_STAY,1);//сделаем активным
wrefresh(wnd1);
wrefresh(wnd2);  
//refresh();
fresh_all=0;
continue;
             }
wrefresh(wnd1);
wrefresh(wnd2);
 
 if(CHANGE_PANEL==i) 
                     {Scan_Dir(awtxt,W_FRESH,activewindow,S_STAY,0);

                      if(1==activewindow){activewindow=2;awtxt=wtxt2;}
                               else      {activewindow=1;awtxt=wtxt1;}
                      Scan_Dir(awtxt,W_FRESH,activewindow,S_STAY,1);

                      };
                      
 i=navigation(awtxt,activewindow);
       
 if(EXIT_NOW==i)break;
   
}

//wrefresh(wnd1);
//wrefresh(wnd2);

Scan_Dir(wtxt1,READ,1,FREE,0);//чистим за собой память на обе панели
Scan_Dir(wtxt2,READ,2,FREE,0);
delwin(wtxt1);           //очистка памяти
delwin(wtxt2);
delwin(wnd1);
delwin(wnd2);


wmove(stdscr, FilesVIEW, 10);
printw("Exit by Z key.. Memory was cleared.. >> Press any key to continue...<<");

//refresh();
getch();
endwin();
exit(EXIT_SUCCESS);
}

void PrintMessage()
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


void Resolve()
{
struct winsize size;
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);
C1=size.ws_col/2-2;
C2=size.ws_col/2-3;
FilesVIEW=size.ws_row-4;
}


void sig_winch(int signo)
{
struct winsize size;
ioctl(fileno(stdout), TIOCGWINSZ, (char *) &size);

resizeterm(size.ws_row, size.ws_col);
}



int navigation(WINDOW *awtxt,int aw)
{int ch;
//enum{UP,DOWN,RIGHT,LEFT};

keypad (awtxt, true);

   // printw("%i\n", wgetch(curscr));
ch = wgetch(awtxt);
		switch(ch)
		{	case 9:		/* Tab */
				return CHANGE_PANEL;
				break;
			
			
                        case 'W':
                        case 'w':
            		case KEY_UP: 
            		        //wclear(awtxt); 
		  		Scan_Dir(awtxt,W_FRESH,aw,S_UP,1);
				break;
                        
                        case 's':
                        case 'S':
			case KEY_DOWN:
			        //wclear(awtxt);
				Scan_Dir(awtxt,W_FRESH,aw,S_DOWN,1);
				break;
			case 10:	/* Enter */
			        wclear(awtxt);
				Scan_Dir(awtxt,W_FRESH,aw,S_ENTER,1);//при обновлении чиститься память
				wclear(awtxt);
				Scan_Dir(awtxt,READ,aw,S_STAY,1);//поэтому прочтем снова ,с новым каталогом 
				return (S_STAY);
				break;
			case KEY_F(1):
			/*Panel(awtxt, "test msg");
			              wrefresh(awtxt);
			              
                                      wrefresh(awtxt);
                                      refresh();
                                      getch();
                                      Scan_Dir(awtxt,READ,1,S_STAY,1);
                                       wrefresh(awtxt);
                                      refresh();*/
                                      
                Panel(awtxt, "          Help   screen:         Navigation: cusors or w/s , Enter,         F5 or z for exit                   F1 this help");
                wrefresh(awtxt);
                getch();
                                      
			              
			
			break;
			
			case 'z':
			case 'Z':
			case KEY_F(5):
			            return EXIT_NOW;
			            
			            break;
			default:
		}
		
		wrefresh(awtxt);
return S_STAY;
}

void Panel(WINDOW *wnd, char *s)
{
char tmp[2000];
strcpy(tmp,s); 
strcat(tmp,"                                                                                                       ");
WINDOW *subwnd,*subwndtxt;
subwnd = derwin(wnd, 8, 36, C1/2, 10);
subwndtxt = derwin(wnd, 4, 34, 1, 1);
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
