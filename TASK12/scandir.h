#ifndef SCANDIR_H
#define SCANDIR_H
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

void Scan_Dir(WINDOW * wnd,int o, int winnumber,int signal, int cvisible);  // основная функция
void FillPanelbyDate(int sel ,struct dirent **namelist, char *FullP, char *, WINDOW *wnd,int N,int cvisible); // общий кусок кода для панелей, отображение в окне
void PathUp(char * x);
 int PrepareDir(char *s, char * FullP, struct dirent **namelist,int sel); 
#endif
