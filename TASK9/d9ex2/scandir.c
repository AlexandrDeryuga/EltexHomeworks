#include "scandir.h"
#include "d9ex2.h"

//void FillPanelbyDate(int sel ,struct dirent **namelist, char *FullP, char *s, char *S, WINDOW *wnd,int N,int cvisible);

extern int C1,C2, FilesVIEW;


 // Функция  ScanDir - это менеджер панелей следит за их состоянием, читает данные и перерисовывает
//
//o - это опция:  считать данные заново, просто перерисовать по данным, 
//                 очистить память    enum SCAN_OPTIONS {READ, W_FRESH, FREE};
// winnumber и wnd соответствуют активному окну
// cvisible говорит о том нужно на панели отрисовать курсор, или погасить его там 

// signal - это связь с интерфейсом : enum SIGNAL_VALUE {S_STAY, S_UP, S_DOWN, S_ENTER,CHANGE_PANEL,EXIT_NOW};
//          куда сместили курсор, сменить активную панель, новых действий нет, войти в директорию

void Scan_Dir(WINDOW * wnd,int o, int winnumber,int signal, int cvisible)
                                       
{   //enum SCAN_OPTIONS(READ, W_FRESH, FREE);

    static struct dirent **namelist1=NULL;
    static struct dirent **namelist2=NULL;
    
    static int LevelUp=0;
    
    static int n1=0,n2=0, sel1=0, sel2=0;
    int n=0;
  static  char StringFileName1[100000];  //массив под текущий путь1
  static  char StringFileName2[100000];  //массив под текущий путь2

  char *pwd=getenv("PWD"); // Получим значение переменной окружения
  
  char FullP[100000];//char PFullP[2000];
  static char *s1=NULL, *s2=NULL,*s3=NULL, *s4=NULL;
    if(s1==NULL) {strcpy(StringFileName1,pwd);s1=StringFileName1;};
    if(s2==NULL) {strcpy(StringFileName2,pwd);s2=StringFileName2;};
    
    struct stat statbuf;
    
    
    switch(signal)
    {              // Блок навигации 
    case S_UP:
      if(n1 && (1==winnumber)&&(sel1>1)) sel1--; 
      if(n2 && (2==winnumber)&&(sel2>1)) sel2--; 
      
    break;
    
    case S_DOWN:
      if(n1 && (1==winnumber)&&(sel1<(n1-1))) sel1++; 
      if(n2 && (2==winnumber)&&(sel2<(n2-1))) sel2++; 
      break;
      
    case S_ENTER:    //блок проваливания в dir
     
     int ok=0;

      if(1==winnumber) ok=PrepareDir(s1,FullP, namelist1,sel1);                         
      if(2==winnumber) ok=PrepareDir(s2,FullP, namelist2,sel2); 
      if(ok==-1) {Panel(wnd,"It is not a directory..        "); getch();return;}
      if(ok==-2) {Panel(wnd,"Cannot open dir!               "); getch();return;}
                                             
     if(ok>0) o=FREE;  // будем отчищать память
    
    case S_STAY:
    default:
    
    }  //end switch SIGNAL
    
    //init_pair(1, COLOR_GREEN, COLOR_BLACK);
    //init_pair(2, COLOR_YELLOW, COLOR_BLUE);
    
    if(FREE==o) //освобождаем память при принудительном освобождении или для новой директории
     {
      if(1==winnumber){while(n1--) free(namelist1[n1]);
                       if(namelist1) {free(namelist1); namelist1=NULL;};
                       };
                       
      if(2==winnumber){while(n2--) free(namelist2[n2]);
                       if(namelist2) {free(namelist2);namelist2=NULL;};
                       };  
      
       return;
      };
    
    switch (winnumber)           //  только при чтении данных, когда есть новые данные 
    {
    case 1:
    
    if( (READ==o)||(!namelist1) )
    {
    if(namelist1) 
    {
    if(1==winnumber){while(n1--) free(namelist1[n1]);
                       if(namelist1) {free(namelist1); namelist1=NULL;};
                    };
    } //это перечитывание , освободим память 
    sel1=1;
    n = scandir(s1, &namelist1, 0, alphasort);
    
    n1=n;
    }
    break;
    
    case 2:
    
    if( (READ==o)||(!namelist2) )
    {
    if(namelist2)  
    {
     if(1==winnumber)
                    {while(n2--) free(namelist2[n2]);
                       if(namelist2) {free(namelist2); namelist2=NULL;};
                    };
    } //это перечитывание , освободим память 
    sel2=1;
    n = scandir(s2, &namelist2, 0, alphasort);
    n2=n;
    }
    
    break;
    
    default: break;
    } //end switch

 wclear(wnd);
 switch (winnumber)           // switch2 -только навигация по готовым данным
    {
    case 1:
     FillPanelbyDate(sel1 ,namelist1,FullP,s1,wnd,n1, cvisible); 
    break;
    
    case 2:
    FillPanelbyDate(sel2 ,namelist2, FullP,s2,wnd,n2, cvisible);   
    break;
    
    default: break;
    } //end switch

 wrefresh(wnd);
}

void FillPanelbyDate(int sel ,struct dirent **namelist, char *FullP, char *FileName, WINDOW *wnd,int N,int cvisible)
{                                                                           
int n=0,i=0;
 struct stat statbuf;

if(namelist) 
    {
    n=N;
   
    if (n < 0) perror("scandir"); 
    else {
              
        strcpy(FullP,FileName);
        strcat(FullP,"/");
        
        wattron(wnd, COLOR_PAIR(6));
         
        wprintw(wnd,"FILES: %d\n",n-1);
        wprintw(wnd,"DIR  : %s\n\n",FullP);
        
         wattron(wnd, COLOR_PAIR(1));
         //wrefresh(wnd);
         i=1;
         if( (sel-(FilesVIEW/2-2))>1)  i=sel-(FilesVIEW/2-2);
         
         while (i<n) 
         {  
            if((i==sel)&&cvisible) wattron(wnd, COLOR_PAIR(4));
            strcpy(FullP,FileName);
            if(strcmp(FullP,"/")) strcat(FullP,"/");
            strcat(FullP,namelist[i]->d_name); //подготовим путь на файл курсора
            if(stat(FullP, &statbuf)==-1) 
                {wprintw(wnd,"<null>");
                }else {if(S_ISDIR(statbuf.st_mode)) wprintw(wnd,"/");};
            
            wprintw(wnd,"%s\n",namelist[i]->d_name);
            wattron(wnd, COLOR_PAIR(1)); 
             if(i>(FilesVIEW-3)) if (i>(sel+(FilesVIEW/2-3))) break;
            i++;
            
         }        
        }
    }
    
    
  } 
    
    
    
    void PathUp(char *x)      //когда мы поняли что необходим возврат 
                              //на уровень выше - то удаляем из пути предыдущую папку
    {
    int i=0;
    if(x) for (i=strlen(x);i>0;i--) if (x[i-1]=='/') break;
                                  
    if((i-1)>0) {x[i-1]=0;} else x[0]=0;   
    if (x[0]==0) strcpy(x,"/");
    return;
    }
    
    int PrepareDir(char *s, char * FullP, struct dirent **namelist,int sel)   //тестирует директория ли это
    {
      struct stat statbuf; int ok=0;
     
     if(strlen(FullP)>99500) exit(1); // не знаю возможно ли это, глубина 388 максимальных имен файлов, но тогда валим в ошибку
     if(strcmp(FullP,"/")){ strcpy(FullP,s);strcat(FullP,"/");}
     strcat(FullP,namelist[sel]->d_name); //подготовим путь на файл курсора
                     
      if(stat(FullP, &statbuf)!=-1) 
            if(S_ISDIR(statbuf.st_mode)){
                         if(strcmp(namelist[sel]->d_name,"..")==0) 
                             {PathUp(s);}
                                   else strcpy(s,FullP);
                        
                        ok=1; // - этот флаг потом скажет что нужно прочитать новую директорию
                        return ok;
                         } 
                         else return -1; // не директория
        return -2;  // ошибка доступа
      }
  
  
