#include "calclibs.h"   //стандартизация нашей библиотеки
#include "main.h" 
#include "stdio.h" 

#include "loader.h"

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define  MAXALLF 9    //максимальное колво мат. функций


//enum MENU {O_Add=1,O_Minus,O_Div, O_Mult, BOOK_Exit} mymenu;


void main (int argc, char *argv[])
{

int i=0,j=0,z=0,RealLoadCount=0;
float A=0.0, B=0.0;
int mymenu=0;
       
calc_function F;                      //элемент интерфейса обмена данными с библиотечными функциями
pfn_calc_function   FArray[MAXALLF];      // указатель на массив всех загруженных библиотечных функций     max=10
void *Lib[2];                      // max 2 библиотеки , массив из двух указателей
void *lib=NULL;
void **plib;
plib=&lib;


LoaderResultStruct lrs;


pfn_calc_function  * ptr=NULL;
//calc_function * (*FArray[FCOUNT]) (calc_function *Fstruct, FCALC_OPERATIONS operation);  //массив указателей на функции
//calc_function * (**m) (calc_function *Fstruct, FCALC_OPERATIONS operation)=NULL;

for(i=0;i<MAXALLF;i++) FArray[i]=NULL;

mymenu=1;
printf("\nДинамический калькулятор с подключаемой библиотекой в качестве параметра из командной строки.\n");
z=1;
if(argc<1) {printf("Модульный функционал не подключен, запустите программу с параметрами: LD_LIBRARY_PATH=$PWD ./d8ex1 lib_calc1.so lib_func.so\n");};
while(z<argc)
{lrs=loadRun(argv[z],lib,0,lib);//возьмем наименование библиотеки с аргумента, и подгрузим ее.
                            //аргументов с избытком для демонстрации проблемы
                          //в итоговом варианте если оставлять структуру lrs, достаточно 3 аргумента,
                         //loadRun(argv[z] -имя библиотеки, 1 - флаг удаления, dellib - ранее сохраненная ссылка
 if(lrs.f)  
 {
ptr=lrs.f;   // блок поиграться с проблемой несанкционированного отчищения памяти
lib=lrs.lib;

Lib[z-1]=lrs.lib; // и итоговый вариант
RealLoadCount++; //- сколько реально бибилиотек загрузили 
}

 //printf("x=%d\n",(int)lib);
 if(ptr){
         
         ptr[0](&F,0); //считаем кол-во функций 
         printf("ОСНОВНОЙ МОДУЛЬ - загрузка: %d, Модуль:%s колво функций %d\n", z,F.F_Name,F.count);
         i=1; //текущий счетчик функций модуля
         int Fmodule=F.count;
        
         for (i;i<=Fmodule;i++) 
                        {
                         FArray[j+i-1]=ptr[i]; //сохранение функций z-го модуля 1
                         ptr[i](&F,GET_NAME);printf("%d. %s\n",i,F.F_Name);
                         };
         j=j+i-1;i=0;
        }
        
 z++;  
 if(z>2) break;
}
z=j;
printf("Итого функций загружено: %d\n", z);

//for(i=0;i<MAXALLF;i++) printf("%d: %d\n", i, (int) FArray[i]);

while(1)
 {
 
 printf("\nМеню доступных функций \n");
 printf("============================\n");
 
 
 for (i=0;i<z;i++) {FArray[i](&F,GET_NAME);printf("%i. %s\n",i+1,F.F_Name);}; //распечатка меню  
 printf("%d) Выход\n",i+1);
 
 do {mymenu=GetCommand(z+1);} while (mymenu==0);// пока возьмем старую, для общего случая надо будет переписать
 if(mymenu==(z+1)) break;
 
 FArray[mymenu-1](&F,GET_NAME); //считаем название функции
 printf("\nВведите операнды для операции: %s  через пробел (пример 7.44 2.6)\n",F.F_Name);
 while (!scanf("%f %f",&F.A,&F.B)) {printf("Неверный ввод,(пример 5.578 2.44)\n");getchar();}

 if(FArray[mymenu-1](&F,GET_RESULT)) 
   {
    printf ("\nрезультат операции %s (%f,%f) = %f\n",F.F_Name,F.A,F.B, F.Result); 
   } else 
         {printf("Ошибка: %s\n", F.F_Name);};
  
 } 
 
 

 
#ifndef WIN32
   printf("Удаляем ссылку на библиотеку\n");
   
   while(RealLoadCount--) 
     {
      printf("главный модуль: - команда на удаление , из МасссивСcылокЗагруженныхБиблиотек[%d]\n",RealLoadCount);
      loadRun(NULL,lib,1,Lib[RealLoadCount]); //очистим все сохраненные указатели
                                                      // ну и по хорошему присвоить им NULL   но мы выходим
     }                  
   //loadRun(NULL,lib,1,lib); // а вот это работает!
   
 //  dlclose(lrs.lib);   //странно почему это нет, видимо просто не дает доступ к данным, но дает доступ к функциям
#else
   //FreeLibrary((HINSTANCE)Lib[0]);
#endif


}

unsigned char GetCommand(int z)  //по простому от 1 до 9
{
unsigned char ch=0; 
printf("Ваш выбор (1-%d):\n",z);
ch=getchar();
while (ch=='\n') {ch=getchar();}

while (getchar()!= '\n') {ch=0;};
//getchar();
if((ch>=49)&&(ch<=49+z)) {return(ch-48);};
return 0;
}


void ScanAB(calc_function *Fstruct)
{
printf("Введите A (целое число):");
}





