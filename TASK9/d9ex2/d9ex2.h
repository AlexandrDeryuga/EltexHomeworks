#ifndef D9EX2_H
#define D9EX2_H

enum SCAN_OPTIONS {READ, W_FRESH, FREE};
enum SIGNAL_VALUE {S_STAY, S_UP, S_DOWN, S_ENTER,CHANGE_PANEL,EXIT_NOW};


//void Scan_Dir(WINDOW * wnd, int o,int wnumber, int signal,int cvisible);
int navigation(WINDOW *awtxt,int aw);
void sig_winch(int signo);
void Resolve();
void Panel(WINDOW *wnd, char * s);

void PrintMessage();


void sig_winch(int signo);

#endif


