#ifndef FCALC_DL1_H
#define FCALC_DL1_H

#define FCOUNT 5  //всего 5 функцций

calc_function * Lib_Info_1(calc_function *Fstruct, FCALC_OPERATIONS operation); //эта вернет инфу о библиотеке
                                                                                //и будет первой в массиве указателейгтсАА

calc_function * Fcalc_1(calc_function *Fstruct, FCALC_OPERATIONS operation);   //определим 4 математических функции
calc_function * Fcalc_2(calc_function *Fstruct, FCALC_OPERATIONS operation);   // operation=GET_NAME вернет 
calc_function * Fcalc_3(calc_function *Fstruct, FCALC_OPERATIONS operation);   // в указатель Fstruct - имя операции
calc_function * Fcalc_4(calc_function *Fstruct, FCALC_OPERATIONS operation);  // или operation=GET_RESULT - резултьтат
                                                                              // возращаем NULL - если результат не готов
                                                                              // иначе ссылку на Fstruct
                                                                              
extern void printString(const char * const s);





#endif

