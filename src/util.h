#ifndef LFFT_UTIL_H
#define LFFT_UTIL_H

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* -------------------------------------------------------------------------------------------- */

#if __STDC_VERSION__ >= 199901L
    #include <stdbool.h>
#else
    #if !defined(__GNUC__) || defined(__STRICT_ANSI__)
        #define inline
    #endif 
    #define bool int
    #define true 1
    #define false 0
#endif

/* -------------------------------------------------------------------------------------------- */

/**
 * dllexport
 *
 * see also http://gcc.gnu.org/wiki/Visibility
 */

#define BUILDING_DLL

#if defined _WIN32 || defined __CYGWIN__
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) /* Note: actually gcc seems to also supports this syntax. */
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) /* Note: actually gcc seems to also supports this syntax. */
    #endif
  #endif
  #define DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #pragma GCC visibility push (hidden) 
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif

#define COMPAT53_PREFIX lfft_compat

#include "compat-5.3.h"

/* ============================================================================================ */

#endif /* LFFT_UTIL_H */
