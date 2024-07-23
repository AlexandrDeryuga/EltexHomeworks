#ifndef CALCLIBS_H
#define CALCLIBS_H

#define FNAME_LENGTH 150

typedef struct calc_function                 //Зададим интерфейс общения с функцией
{
char F_Name[FNAME_LENGTH]; // имя Оператора или ошибка
float A;
float B; 
int count; //в зависимости от контекста примет значение - кол-во аргументов в функции,  количество функций в библиотеке
//int C, D и так далее - это операнды
float Result;   // результат вычисления


} calc_function;


typedef enum FCALC_OPERATIONS {GET_NAME=0, GET_RESULT,GET_LIB_INFO} FCALC_OPERATIONS ;  //

typedef calc_function *  (*pfn_calc_function)(calc_function *Fstruct,FCALC_OPERATIONS operation); //тип - указатель на библиотечную функцию, создадим потом массив указателей

typedef struct LoaderResultStruct 
{
pfn_calc_function * f;
void * lib; 
} LoaderResultStruct;


#endif

