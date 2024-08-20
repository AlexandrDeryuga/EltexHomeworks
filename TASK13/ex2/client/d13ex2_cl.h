#ifndef D9EX2_H
#define D9EX2_H
#include "dinamic_msg.h"

enum SCAN_OPTIONS {READ, W_FRESH, FREE};
enum SIGNAL_VALUE {S_STAY, S_UP, S_DOWN, S_ENTER,CHANGE_PANEL,EXIT_NOW};

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//void Scan_Dir(WINDOW * wnd, int o,int wnumber, int signal,int cvisible);
int navigation(WINDOW *awtxt,int aw,WINDOW *wtxt); // активное окно,  номер его и окно куда пишем вводимый текст
void sig_winch(int signo);
void Resolve();
void Panel(WINDOW *awnd, char * s);  

void PrintMessage();
void printChat(WINDOW *wnd, int cursor, int rows, RoomFileType *RoomMSG );//печатает сообщения для комнаты
void printClients(WINDOW *wtxt, int rows );//печатает подключившихся абонентов 


void sig_winch(int signo);

#endif


