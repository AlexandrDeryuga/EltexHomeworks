#ifndef LOADER_H
#define LOADER_H

#include "calclibs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern LoaderResultStruct loadRun(const char * const s,void **, int,  void * dellib);

#ifdef __cplusplus
}
#endif

#endif
