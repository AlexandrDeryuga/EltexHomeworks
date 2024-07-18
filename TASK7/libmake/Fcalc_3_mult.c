#include "calclibs.h"
#include "stdio.h"
#include "string.h"


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




