#include "dinamic.h"




 
abonent *ABOOK=NULL;    // Секция глобальных переменных
unsigned int count=0;   // заменим наши константы ранее определенные через define переменными что бы не менять код.
unsigned int MAX=0;   // заметим, что теперь по сути count =MAX, но оставим пока для совместимости, 
                      //и для общего случая, вдруг потом мы захотим выделять память блоками
                      //По логике MAX=максимальное кол-во абонентов (ограничение массива), будет использоваться для выделения памяти
                      //count - более высокоуровневое понятие, это текущее кол-во абонентов в справочнике
                      

int DinamicAddElement(unsigned int number,unsigned short int addition,char inicializate) //если надо память на i-ый элемент массива
{                                                               //норм, если вернем >=0

 static long  RecSelected=0;    // Пространство допустимых индексов 0> <RecSelected, 
                                //выделенно памяти R_Selected элементов

if (inicializate) RecSelected=0;

#ifdef _DEBUG
printf("запрошено памяти %d  памяти на начало процедуры %ld\n",number,RecSelected);
#endif

 abonent *A=NULL; 
if(NULL==ABOOK) RecSelected=0;   
if(MAX) if (number<=(MAX+1)) return 0; // память уже выделена   2<max
if(RecSelected<0) {printf("Отрицательный индекс, -point36\n"); //Защита
                            if(ABOOK) free(ABOOK); 
                            DinamicFilterIndexActual(0);exit(1);
                            return -1;
                            }

if(RecSelected<=0){  //это первый элемент malloc(); RecSelcted==0  , inicializate появился позже
            A = (abonent*) calloc(addition,sizeof(abonent));          // динамическое выделение памяти на addition элементов
  if (A==NULL) 
              {
              perror("-point41-"); printf("\n Cannot allocate memory\n");     
              if(ABOOK) {printf("\n point 53- warning\n");free(ABOOK);};
              DinamicFilterIndexActual(0);exit (1);    // если выделение памяти не выполнилось, завершить программу
              }
             RecSelected=addition; //Допустимы все индексы 0> <RecSelected
             MAX=RecSelected-1; //максимальное колво абонентов - кол-во выделенной памяти -1
             ABOOK=A;
             return 1;// выделили успешно
         
        
        }else //RecSelected>0  
        
        {//alloc  , 
           RecSelected=MAX+1;// восстановили RecSelected, Допустимы все индексы 0> <RecSelected
           #ifdef _DEBUG2 
           printf("MAX: %d  восстановили RecSelected %d\n",MAX,RecSelected);
           #endif
           if(ABOOK==NULL) {printf("Ссылка на нулевой указатель, -point62\n"); //Защита
                            DinamicFilterIndexActual(0); exit(1);
                            return -1;
                            }
                            
            A = (abonent*) realloc(ABOOK,sizeof(abonent)*(RecSelected+addition));
            if (A==NULL) 
              {
              perror("-point70-");
              printf("\n Cannot allocate memory\n");
               
              if(ABOOK) free(ABOOK);
              DinamicFilterIndexActual(0);exit (1);  // если выделение памяти не выполнилось, завершить программу с освобождением
              }
        
        ABOOK=A;
       
        // к этому моменту ранее заполненны массив включая запись под парсинг стал списком абонентов, поэтому 
        // вновь выделенную память заполняем нулями, запись под парсинг теперь это count+1   
        
        for(int ii=1;ii<addition;ii++)for(int kk=0;kk<STR_LENGTH;kk++) 
        {ABOOK[RecSelected+ii].name[kk]=0;
         ABOOK[RecSelected+ii].second_name[kk]=0;
         ABOOK[RecSelected+ii].tel[kk]=0;  
        }
        
        RecSelected+=addition;
        MAX=RecSelected-1; // кол-во абонентов
        return 1;
        //MAX++;
        }
printf("Никогда не должен сработать"); return -2; //  Никогда не должен сработать
}

int DinamicDeleteElement(unsigned int number,unsigned short int addition)   // норм, если вернули >=0
{int x=0;                                                                  //number кол-во элементов памяти(строк), которые мы хотим видеть в массиве
abonent *A=NULL;
x=MAX+1-addition;

#ifdef _DEBUG
printf("MAX=%d x=%d number=%d\n",MAX,x,number);
#endif

if(x<0) return 0; // не нужно ничего делать
if(0==x) {
          if(NULL==ABOOK) {printf("Ссылка на нулевой указатель, -point97\n"); //Защита
                           DinamicFilterIndexActual(0);exit(1);//не забудем освободить память
                          }
            printf("point100 - warning");  //по логике у нас всегда одна запись для парсинга            
          if(ABOOK) free(ABOOK);           //но для наглядности отчистим память 
          ABOOK=NULL;MAX=0;
          DinamicAddElement(1,addition,1);//и сразу создадим элемент под парсинг, указав что это первый запуск
         }
  if(number<=x)       //пример 5<=(5+1-2)   тогда realloc
    {                             
     A = (abonent*) realloc(ABOOK,sizeof(abonent)*x);
            if (A==NULL) 
              {
              printf("\n Cannot allocate memory\n");
              perror("-point60-");
              
               
              if(ABOOK)free(ABOOK);
              DinamicFilterIndexActual(0); exit (1);    // если выделение памяти не выполнилось, завершить программу с освобождением
              }
        ABOOK=A;       
        MAX=x-1; // уменьшили на блок элементов
        
        #ifdef _DEBUG
        printf("(MAX=%d x=%d number=%d)\n",MAX,x,number);
        #endif
        
        return 1; 
    }
}

char * DinamicFilterIndexActual(unsigned long clear)
{
static char *Ptr=NULL;
char * newPtr=NULL;
if(!clear) if(Ptr) {free(Ptr);Ptr=NULL;return Ptr;}; //будем джентельменами, освободим память если просят
if(MAX<=0) return NULL;
if(Ptr) {
          newPtr = (char*) realloc(Ptr,sizeof(char)*MAX);
          if (newPtr==NULL) 
              {
              perror("-point130-");
              printf("\n Cannot allocate memory\n");
               
              if(Ptr)free(Ptr); exit (1);    // если выделение памяти не выполнилось, завершить программу с освобождением
              return NULL; // либо вернем NULL, если закоментим строчку выше
              }
              Ptr=newPtr;
              for(int i=0;i<MAX;i++) *(Ptr+i)=0;
              return Ptr;

        } else
        {
        newPtr = (char *) calloc(MAX,sizeof(char));
        if (newPtr==NULL) 
              {
              perror("-point144-");
              printf("\n Cannot allocate memory\n");
               
              if(Ptr)free(Ptr); exit (1);    // если выделение памяти не выполнилось, завершить программу с освобождением
              return NULL; // либо вернем NULL, если закоментим строчку выше
              }
              Ptr=newPtr;
              for(int i=0;i<MAX;i++) *(Ptr+i)=0;
              return Ptr;
        }



}

