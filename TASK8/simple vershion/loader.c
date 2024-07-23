#include "loader.h"
#include "func.h"
#include <stdio.h>
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

pfn_calc_function * loadRun(const char * const s) {// имя библиотеки, ячейку для заполнения, любое значение
   void * lib;
   void (*fun)(void);
   pfn_calc_function *ptr=NULL;
#ifndef WIN32
   lib = dlopen(s, RTLD_LAZY);
#else
   lib = LoadLibrary(s);
#endif
   if (!lib) {
     printf("cannot open library '%s'\n", s);
     return NULL;
   }
#ifndef WIN32
   fun = (pfn_calc_function (*)(void))dlsym(lib, "run");
#else
   fun = (pfn_calc_function (*)(void))GetProcAddress((HINSTANCE)lib, "run");
#endif
   if (fun == NULL) {
     printf("cannot load function run\n");
   } else {
          ptr=fun();
          }
#ifndef WIN32
   dlclose(lib);
#else
   FreeLibrary((HINSTANCE)lib);
#endif

return ptr;
}

#ifdef __cplusplus
}
#endif
