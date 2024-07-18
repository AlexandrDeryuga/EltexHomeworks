#ifndef CALCLIBS_H

#define FCOUNT 4
#define FNAME_LENGTH 150

typedef struct calc_function                 //Зададим интерфейс общения с функцией
{
char F_Name[FNAME_LENGTH];
float A;
float B; 
//int C, D и так далее - это операнды
float Result;

} calc_function;


typedef enum FCALC_OPERATIONS {GET_NAME=0, GET_RESULT} FCALC_OPERATIONS ;  //

typedef calc_function *  (*pfn_calc_function)(calc_function *Fstruct, FCALC_OPERATIONS operation); //тип - указатель на библиотечную функцию, создадим потом массив указателей

calc_function * Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation);   //определим 4 функции
calc_function * Fcalc_2(calc_function *Fstruct, FCALC_OPERATIONS operation);   // operation=GET_NAME вернет 
calc_function * Fcalc_3(calc_function *Fstruct, FCALC_OPERATIONS operation);   // в указатель Fstruct - имя операции
calc_function * Fcalc_4(calc_function *Fstruct, FCALC_OPERATIONS operation);  // или operation=GET_RESULT - резултьтат
                                                                              // возращаем NULL - если результат не готов
                                                                              // иначе ссылку на Fstruct
#endif

