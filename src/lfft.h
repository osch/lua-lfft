#ifndef LFFT_H
#define LFFT_H

#include "util.h"

/* ============================================================================================ */

extern const char* const LFFT_CLASS_NAME;

/* ============================================================================================ */

typedef struct CarrayUserData CarrayUserData;
struct lfft;

struct CarrayUserData
{
    const char*    className;
    struct lfft* impl;
};

/* ============================================================================================ */

int lfft_push_meta(lua_State* L);

int lfft_init_module(lua_State* L, int module);

/* ============================================================================================ */

#endif /* LFFT_CAPI_IMPL_H */
