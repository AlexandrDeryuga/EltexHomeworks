#include "calclibs.h"
#include "stdio.h"
#include "string.h"


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




