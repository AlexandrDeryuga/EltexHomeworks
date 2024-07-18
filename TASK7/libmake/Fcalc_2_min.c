#include "calclibs.h"
#include "stdio.h"
#include "string.h"


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




