#include "calclibs.h"
#include "stdio.h"
#include "string.h"


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




