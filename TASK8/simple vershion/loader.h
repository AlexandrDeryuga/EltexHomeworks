#ifndef LOADER_H
#define LOADER_H

#include "calclibs.h"

#ifdef __cplusplus
extern "C" {
#endif

extern pfn_calc_function * loadRun(const char * const s);

#ifdef __cplusplus
}
#endif

#endif
