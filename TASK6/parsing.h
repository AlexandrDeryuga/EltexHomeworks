#ifndef  PARSING_H
#define PARSING H
#include "main.h"

int ScanWord(int param, char *str, int L, int flag, int *ch); //Читает слово из строки ввода, 
                                                             // и Заполняет элемент структуры  *str этим словом
                                                            // либо не заполняет, если введена "*"
void FindAbonent();          //Фильтрация абонентов
int FillStructure(int Index, int param); //сканируем и заполним структуру по индексу введенными данными
char * FindSTR(const char * str1,const char *str2, int L);  //функция поиска подстроки из предыдущего задания
#endif

