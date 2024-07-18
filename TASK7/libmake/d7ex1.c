#include "calclibs.h"   //стандартизация нашей библиотеки
#include "main.h" 
#include "stdio.h" 


enum MENU {O_Add=1,O_Minus,O_Div, O_Mult, BOOK_Exit} mymenu;


void main ()
{

int i=0,j=0,z=0;
float A=0.0, B=0.0;

mymenu=1;
       
calc_function F;                      //элемент интерфейса обмена данными с библиотечными функциями
pfn_calc_function FArray[FCOUNT];    //Массив библиотечных функций
                                    //
FArray[0]=Fcalc_1;                 // Инициализация массива
FArray[1]=Fcalc_2;
FArray[2]=Fcalc_3;
FArray[3]=Fcalc_4;

F.A=15.0;  // Для тестирования
F.B=3.0;
F.Result=0;
    
while(mymenu!=BOOK_Exit)
 {
 
 printf("\nКалькулятор на статических библиотеках\n\n");
 for (i=1;i<=FCOUNT;i++) {FArray[i-1](&F,GET_NAME);printf("%i. %s\n",i,F.F_Name);}; //распечатка меню  
 printf("%d) Выход\n",i);
 
 while (!GetCommand()) {};// пока возьмем старую, для общего случая надо будет переписать
 
 if(mymenu>FCOUNT) break;
 
 FArray[mymenu-1](&F,GET_NAME); //считаем название функции
 printf("\nВведите операнды для операции: %s  через пробел (пример 7.44 2.6)\n",F.F_Name);
 while (!scanf("%f %f",&F.A,&F.B)) {printf("Неверный ввод,(пример 5.578 2.44)\n");getchar();}

 if(FArray[mymenu-1](&F,GET_RESULT)) 
   {
    printf ("\nрезультат операции %s (%f,%f) = %f\n",F.F_Name,F.A,F.B, F.Result); 
   } else 
         {printf("Ошибка: %s\n", F.F_Name);};
 /*
 switch (mymenu)
  {case O_Add:  Fcalc_1(&F,GET_NAME);
                if(Fcalc_1(&F,GET_RESULT))  break;
  case O_Minus:  Fcalc_2(&F,GET_NAME);
                 if(Fcalc_2(&F,GET_RESULT)) printf ("\nрезультат операции %s (%f,%f) = %f\n",F.F_Name,F.A,F.B, F.Result);break;
  case O_Div:    Fcalc_3(&F,GET_NAME);
                 if(Fcalc_3(&F,GET_RESULT)) printf ("\nрезультат операции %s (%f,%f) = %f\n",F.F_Name,F.A,F.B, F.Result);break;
  case O_Mult:   Fcalc_4(&F,GET_NAME);
                 if(Fcalc_4(&F,GET_RESULT)) printf ("\nрезультат операции %s (%f,%f) = %f\n",F.F_Name,F.A,F.B, F.Result);break;
  }
  */
  
  
  
 }
}

unsigned char GetCommand()
{
unsigned char ch=0; 
printf("Ваш выбор (1-5):\n");
ch=getchar();
while (ch=='\n') {ch=getchar();}

while (getchar()!= '\n') {ch=0;};
//getchar();
if((ch>=49)&&(ch<=53)) {mymenu=ch-48;return(mymenu);};
return 0;
}


void ScanAB(calc_function *Fstruct)
{
printf("Введите A (целое число):");
}





