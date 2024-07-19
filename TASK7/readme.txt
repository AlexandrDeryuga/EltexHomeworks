https://github.com/AlexandrDeryuga/EltexHomeworks/tree/main/TASK7

Собрана статическая библиотека:

ar rc calc.a Fcalc_1_add.o Fcalc2_min.o Fcalc_3_mult.o Fcalc4_div.o

Собрана головная программа

gcc d7ex1.c -c
gcc d7ex1.o -o dx7ex1 -L. -l:calc.a

и имеется скрин сборки.

ОПИСАНИЕ:

Создается массив указателей на унифицированные библиотечные функции,  в заголовочном файле описана структура для вазаимодействия с библиотечной функцией, структура содержит строку - наименование операци, и в случае ошибки туда заносится сообщение об ошибке. В структуре хранятся операнды функции, и результат выполнения.

При запуске с параметром GET_NAME, возвращает в структуру имя оператора, при GET_RESULT - инициируется само вычисление.

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
