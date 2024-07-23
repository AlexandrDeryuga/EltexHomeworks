#include "calclibs.h"
#include "Fcalc_dl1.h"
#include "stdio.h"
#include "string.h"
#include "loader.h"

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

pfn_calc_function FArray[FCOUNT];

pfn_calc_function * run(void) {
  printString("итак, осваиваем технологию ;=)");
  printString("Hello, world! от динамической библиотеки lib_calc1.so  ,функция run");
  printString("печать идет от библиотеки lib_func.so c помощью функции PrintString из библиотеки  lib_loader.so");
  printString("в дальнейшем эта функция run будет возвращать указатель на массив указателей на свои математические функции");
  
  FArray[0]=Lib_Info_1; //информация о библиотеке 1
  FArray[1]=Fcalc_1;
  FArray[2]=Fcalc_2;
  FArray[3]=Fcalc_3;
  FArray[4]=Fcalc_4;
  return FArray;
}

calc_function * Lib_Info_1(calc_function *Fstruct, FCALC_OPERATIONS operation)
{
if(Fstruct==NULL) return NULL;
Fstruct->Result=FCOUNT-1;           //вернем кол-во мат. функций
strncpy(Fstruct->F_Name,"Fcalc_dl1 v1.0",FNAME_LENGTH);//и имя библиотеки
return Fstruct;
}

calc_function * Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation) //сложение
{
if(Fstruct==NULL) return NULL;
switch (operation)
 {
 case GET_RESULT: Fstruct->Result=Fstruct->A+Fstruct->B;
                  return Fstruct;
 
 case    GET_NAME: strncpy(Fstruct->F_Name,"Сложение",FNAME_LENGTH);
                   return NULL;
 }

}


calc_function * Fcalc_2(calc_function *Fstruct, FCALC_OPERATIONS operation) //вычитание
{
if(Fstruct==NULL) return NULL;
switch (operation)
 {
 case GET_RESULT: Fstruct->Result=Fstruct->A-Fstruct->B;
                  return Fstruct;
 
 case    GET_NAME: strncpy(Fstruct->F_Name,"Вычитание",FNAME_LENGTH);
                   return NULL;
 }

}

calc_function * Fcalc_3(calc_function *Fstruct, FCALC_OPERATIONS operation) //умножение
{
if(Fstruct==NULL) return NULL;
switch (operation)
 {
 case GET_RESULT: Fstruct->Result=Fstruct->A*Fstruct->B;
                  return Fstruct;
 
 case    GET_NAME: strncpy(Fstruct->F_Name,"Умножение",FNAME_LENGTH);
                   return NULL;
 }

}


calc_function * Fcalc_4(calc_function *Fstruct, FCALC_OPERATIONS operation) //деление
{
if(Fstruct==NULL) return NULL;
switch (operation)
 {
 case GET_RESULT: if(Fstruct->B) {Fstruct->Result=(Fstruct->A)/(Fstruct->B);
                                  return Fstruct;
                                  }else
                                   {
                                    strncpy(Fstruct->F_Name,"Деление на ноль!",FNAME_LENGTH);
                                    return NULL;
                                   }
 
 case    GET_NAME: strncpy(Fstruct->F_Name,"Деление",FNAME_LENGTH);
                   return NULL;
 }

}



