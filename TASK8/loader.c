#include "loader.h"
#include "func.h"
#include <stdio.h>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

void printString(const char * const s) {
  printf("libloader: %s\n", s);
}





LoaderResultStruct loadRun(const char * const s,    void ** plib, int free, void * dellib) {
   static void * lib=NULL;
   
   
  pfn_calc_function *(*fun)(void);
   pfn_calc_function * ptr=NULL;    // указатель на массив функций 
   LoaderResultStruct lrs;
   lrs.f=NULL;
   lrs.lib=NULL;

if (free) if(dellib)    // если команда на удаление - удаляем
   {
   #ifndef WIN32
   if(!dlclose(dellib))
   {printf("libloader: Библиотека по ссылке  выгруженна.\n");} 
        else { printf("libloader:ошибка при выгрузке библиотеки");};
  #else
   FreeLibrary((HINSTANCE)dellib);
   #endif
   
   return lrs;
   }

#ifndef WIN32
   lib = dlopen(s, RTLD_LAZY);
#else
   lib = LoadLibrary(s);
#endif
   if (!lib) {
     printf("cannot open library '%s'\n", s);
     return lrs;
   }
#ifndef WIN32

   fun = (pfn_calc_function  *(*)(void))dlsym(lib, "run");
#else
   fun = (pfn_calc_function  * (*)(void))GetProcAddress((HINSTANCE)lib, "run");
#endif
   if (fun == NULL) {
     printf("cannot load function run\n");
   } else {
     ptr=fun();
   }


//printf ("x=%d\n",(int)lib);


lrs.f=ptr;        // это тоже все для игрищь с проблемой несанкционированного удаления
lrs.lib=lib;

plib=&lib;
return lrs;
}

#ifdef __cplusplus
}
#endif
