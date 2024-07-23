#include "func.h"
#include "calclibs.h"
#include <stddef.h>
#include <string.h>
#include <math.h> 

#ifdef __cplusplus           //подсмотрел такой ход, надо разобраться позже
extern "C" {                //без этого динамические библиотеки не смогут правильно скомпоноваться компилятором С++
#endif
/*Просто в C++ можно объявлять функции с одним и тем же именем, но с разной сигнатурой, соответственно в этом случае используется так называемая декорация имён функций, то есть сигнатура функций записывается в ABI. Окружение extern «C» {} нужно для того, чтобы не использовалась эта декорация (тем более, что эта декорация зависит от используемого компилятора).*/
calc_function * (*FArray[FCOUNT]) (calc_function *Fstruct, FCALC_OPERATIONS operation);  //массив указателей на функции

pfn_calc_function * run(void) {
  //pfn_calc_function *m=NULL;// что аналогично строке с низу
  //calc_function * (**m) (calc_function *Fstruct, FCALC_OPERATIONS operation)=NULL;
  //m=FArray;
  
  printString("Hello, world! от динамической библиотеки func.so  ,функция run");
  printString("все аналогично как с основоной библиотекой мат. функций  калькулятора ");
  
  FArray[0]=Lib_Info_1;
  FArray[1]=Fcalc_1;
 
  return FArray;
}

calc_function * Lib_Info_1(calc_function *Fstruct, FCALC_OPERATIONS operation)
{
if(Fstruct==NULL) return NULL;
Fstruct->count=FCOUNT-1;           //вернем кол-во мат. функций
strncpy(Fstruct->F_Name,"Func v1.0",FNAME_LENGTH);//и имя библиотеки
return Fstruct;
}

//далее можем внести свои математические функции


calc_function * Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation) //x^y
{
if(Fstruct==NULL) return NULL;
switch (operation)
 {
 case GET_RESULT: 
    
  //  if(Fstruct->B == 0) Fstruct.Result=1;
   // if(Fstruct->B < 0)  Fstruct.Result= power ( 1.0 / Fstruct->A, Fstruct->B); 
   // if(Fstruct->B > 0)  Fstruct.A * power(B, n - 1);
 
                  Fstruct->Result=  pow(Fstruct->A,Fstruct->B);
                  return Fstruct;
 
 case    GET_NAME: strncpy(Fstruct->F_Name,"Степень числа (A^B)",FNAME_LENGTH);
                   return NULL;
 }

}




