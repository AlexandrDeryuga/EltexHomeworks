#ifndef CALCLIBS_H
#define CALCLIBS_H

#define FNAME_LENGTH 150

typedef struct calc_function                 //Зададим интерфейс общения с функцией
{
char F_Name[FNAME_LENGTH]; // имя Оператора или ошибка
float A;
float B; 
//int C, D и так далее - это операнды
float Result;   //в зависимости от контекста примет значение - кол-во аргументов в функции,  количество функций в библиотеке
                // либо результат вычисления


} calc_function;


typedef enum FCALC_OPERATIONS {GET_NAME=0, GET_RESULT,GET_LIB_INFO} FCALC_OPERATIONS ;  //

typedef calc_function *  (*pfn_calc_function)(calc_function *Fstruct); //тип - указатель на библиотечную функцию, создадим потом массив указателей


#endif

