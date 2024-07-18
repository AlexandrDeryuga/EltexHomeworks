#ifndef DINAMIC_H
#define DINAMIC_H

#include <malloc.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include "stdlib.h"

//#include <alloca.h>

#define STR_LENGTH 10
//#define _DEBUG

typedef struct  {
 char name[STR_LENGTH];
 char second_name[STR_LENGTH];
 char tel[STR_LENGTH];
 } abonent;

int DinamicAddElement(unsigned int number,unsigned short int addition, char initializate);
int DinamicDeleteElement(unsigned int number,unsigned short int addition);

char * DinamicFilterIndexActual(unsigned long clear);// ведет динамический массив индексов, отвечающих условиям фильтра,
                                                    // clear ==0 освобождает память. clear>0 realloc
                                                    


#endif




