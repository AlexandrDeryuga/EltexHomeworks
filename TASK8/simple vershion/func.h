#include "calclibs.h"
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus           //подсмотрел такой ход, надо разобраться позже
extern "C" {                //без этого динамические библиотеки не смогут правильно скомпоноваться компилятором С++
#endif
/*Просто в C++ можно объявлять функции с одним и тем же именем, но с разной сигнатурой, соответственно в этом случае используется так называемая декорация имён функций, то есть сигнатура функций записывается в ABI. Окружение extern «C» {} нужно для того, чтобы не использовалась эта декорация (тем более, что эта декорация зависит от используемого компилятора).*/

extern void printString(const char * const s);

calc_function * L2Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation);   //определим 2 функции
calc_function * L2Fcalc_2(calc_function *Fstruct, FCALC_OPERATIONS operation);  

#ifdef __cplusplus
}
#endif

#endif
