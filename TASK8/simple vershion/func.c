#include "func.h"
#include "calclibs.h"
#include <stddef.h>

#ifdef __cplusplus           //подсмотрел такой ход, надо разобраться позже
extern "C" {                //без этого динамические библиотеки не смогут правильно скомпоноваться компилятором С++
#endif
/*Просто в C++ можно объявлять функции с одним и тем же именем, но с разной сигнатурой, соответственно в этом случае используется так называемая декорация имён функций, то есть сигнатура функций записывается в ABI. Окружение extern «C» {} нужно для того, чтобы не использовалась эта декорация (тем более, что эта декорация зависит от используемого компилятора).*/

void run(void) {
  printString("итак, осваиваем технологию ;=)");
  printString("Hello, world! от динамической библиотеки lib_func.so  ,функция run");
  printString("печать идет от библиотеки lib_func.so c помощью функции PrintString из библиотеки  lib_loader.so");
  printString("в дальнейшем эта функция run будет возвращать указатель на массив указателей на свои математические функции");
  
}

//далее можем внести свои математические функции

calc_function * L2Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation)  //определим 2 функции
{
return NULL;
}
calc_function * L2Fcalc_2(calc_function *Fstruct, FCALC_OPERATIONS operation)
{
return NULL;
}

#ifdef __cplusplus
}
#endif
