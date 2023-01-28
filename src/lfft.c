#include <limits.h>

#include "util.h"
#include "lfft.h"
#include "calcfloat.h"
#include "calcdouble.h"

#define CARRAY_CAPI_IMPLEMENT_REQUIRE_CAPI 1
#define CARRAY_CAPI_IMPLEMENT_GET_CAPI     1
#include "carray_capi.h"

/* ============================================================================================ */

const char* const LFFT_CLASS_NAME = "lfft";

/* ============================================================================================ */

struct lfft_cache
{
    const char*               name;
    const carray_capi*        carrayCapi;

    llft_plan_float_complex*  planfc;
    size_t                    planfc_n;

    llft_plan_float_real*     planfr;
    size_t                    planfr_n;

    llft_plan_double_complex* plandc;
    size_t                    plandc_n;

    llft_plan_double_real*    plandr;
    size_t                    plandr_n;
};

typedef struct lfft_cache lfft_cache;

enum lfft_calltype
{
    FFT_C2C,
    IFFT_C2C,
    
    FFT_R2C,
    IFFT_C2R
};

typedef enum lfft_calltype lfft_calltype;

/* ============================================================================================ */

static int Lfft_release(lua_State* L)
{
    int arg = 1;
    
    lfft_cache* udata = (lfft_cache*) luaL_checkudata(L, arg, LFFT_CLASS_NAME);
    if (udata->name != LFFT_CLASS_NAME) {
        return luaL_argerror(L, arg, "lfft expected");
    }
    if (udata->planfc) {
        free(udata->planfc);
        udata->planfc = NULL;
    }
    if (udata->planfr) {
        free(udata->planfr);
        udata->planfr = NULL;
    }

    if (udata->plandc) {
        free(udata->plandc);
        udata->plandc = NULL;
    }
    if (udata->plandr) {
        free(udata->plandr);
        udata->plandr = NULL;
    }
    return 0;
}

/* ============================================================================================ */

static const luaL_Reg LfftMetaMethods[] = 
{
    { "__gc",     Lfft_release },

    { NULL,       NULL } /* sentinel */
};

/* ============================================================================================ */

static lfft_cache* pushNewLfftCache(lua_State* L)
{
    lfft_cache* udata = lua_newuserdata(L, sizeof(lfft_cache)); /* -> udata */
    memset(udata, 0, sizeof(lfft_cache));                       /* -> udata */
    udata->name = LFFT_CLASS_NAME;
    udata->carrayCapi = carray_require_capi(L);
    lua_pushvalue(L, -1);                                       /* -> udata, udata */
    lua_rawsetp(L, LUA_REGISTRYINDEX, LFFT_CLASS_NAME);         /* -> udata */

    luaL_newmetatable(L, LFFT_CLASS_NAME);                      /* -> udata, meta */
    lua_pushstring(L, LFFT_CLASS_NAME);                         /* -> udata, meta, className */
    lua_setfield(L, -2, "__metatable");                         /* -> udata, meta */
    luaL_setfuncs(L, LfftMetaMethods, 0);                       /* -> data, meta */
    lua_setmetatable(L, -2);                                    /* -> udata */
    return udata;
}

static int Lfft_call(lua_State* L, lfft_calltype calltype)
{
    int nargs = lua_gettop(L);
    int arg = 1;

    lfft_cache* lfft = lua_touserdata(L, lua_upvalueindex(1));
    if (!lfft || lfft->name != LFFT_CLASS_NAME) {
        return luaL_error(L, "internal upvalue missing");
    }
    const carray_capi* carrayCapi = lfft->carrayCapi;

    carray_info info;
    const carray* inpArray = (arg <= nargs) ? carrayCapi->toReadableCarray(L, arg, &info)
                                            : NULL;
    if (!inpArray || (info.elementType != CARRAY_FLOAT && info.elementType != CARRAY_DOUBLE)) {
        return luaL_argerror(L, arg, "float carray expected");
    }
    bool isFloat = info.elementType == CARRAY_FLOAT;
    size_t inpCount = info.elementCount; 
    if ((inpCount < 2) || (inpCount % 2)) {
        return luaL_argerror(L, arg, "array length must be multiple of 2");
    }

    size_t outCount;
    if (calltype == FFT_R2C) {
        outCount = inpCount + 2;
    } else if (calltype == IFFT_C2R) {
        if (inpCount <= 2) {
            return luaL_argerror(L, arg, "array length must greater than 2");
        }
        outCount = inpCount - 2;
    } else {
        outCount = inpCount;
    }
    const void*   inpData   = carrayCapi->getReadableElementPtr(inpArray, 0, inpCount);
    const float*  inpFloat  = isFloat ? (const float*)inpData : NULL;
    const double* inpDouble = isFloat ? NULL : (const double*)inpData;
    ;
    arg += 1;
    carray* outArray = (arg <= nargs) ? carrayCapi->toWritableCarray(L, arg, &info)
                                      : NULL;
    if (outArray) {
        if (isFloat) {
            if (info.elementType != CARRAY_FLOAT) {
                return luaL_argerror(L, arg, "float carray expected");
            }
        } else {
            if (info.elementType != CARRAY_DOUBLE) {
                return luaL_argerror(L, arg, "double carray expected");
            }
        }
    }
    if (outArray == inpArray) {
        return luaL_argerror(L, arg, "output array must be different from input array");
    }
    void* outData = NULL;
    if (outArray) {
        lua_pushvalue(L, arg); /* -> outArray */
        outData = carrayCapi->resizeCarray(outArray, outCount, 0);
        if (!outData) {
            lua_pop(L, 1);    /* -> */
        }
    }
    if (!outData) {
        outArray = carrayCapi->newCarray(L, isFloat ? CARRAY_FLOAT : CARRAY_DOUBLE, CARRAY_DEFAULT, outCount, (void**)&outData); /* -> outArray */
    }
    float*  outFloat  = isFloat ? (float*)outData : NULL;
    double* outDouble = isFloat ? NULL : (double*)outData;
    
    if (calltype == FFT_C2C || calltype == IFFT_C2C)
    {
        size_t n = inpCount/2;
        // STB_FFT / STB_IFFT
        {
            if (n < 2) {
                if (isFloat) {
                    outFloat[0] = inpFloat[0];
                    outFloat[1] = inpFloat[1];
                } else {
                    outDouble[0] = inpDouble[0];
                    outDouble[1] = inpDouble[1];
                }
            } else {
                if (isFloat) {
                    llft_plan_float_complex* planfc;
                    if (lfft->planfc && lfft->planfc_n == n) {
                        planfc = lfft->planfc;
                    } else {
                        if (lfft->planfc) {
                            free(lfft->planfc);
                            lfft->planfc = NULL;
                        }
                        planfc = lfft_create_plan_float_complex(n);
                        if (planfc == NULL) {
                            return luaL_error(L, "error allocating memory");
                        }
                        lfft->planfc   = planfc;
                        lfft->planfc_n = n;
                    }
                    if (calltype == FFT_C2C) {
                        lfft_exec_float_fft_c2c(planfc, inpFloat, outFloat);
                    } else {
                        lfft_exec_float_ifft_c2c(planfc, inpFloat, outFloat);
                    }
                } else {
                    llft_plan_double_complex* plandc;
                    if (lfft->plandc && lfft->plandc_n == n) {
                        plandc = lfft->plandc;
                    } else {
                        if (lfft->plandc) {
                            free(lfft->plandc);
                            lfft->plandc = NULL;
                        }
                        plandc = lfft_create_plan_double_complex(n);
                        if (plandc == NULL) {
                            return luaL_error(L, "error allocating memory");
                        }
                        lfft->plandc   = plandc;
                        lfft->plandc_n = n;
                    }
                    if (calltype == FFT_C2C) {
                        lfft_exec_double_fft_c2c(plandc, inpDouble, outDouble);
                    } else {
                        lfft_exec_double_ifft_c2c(plandc, inpDouble, outDouble);
                    }
                }
            }
        }
        if (calltype == IFFT_C2C) {
            if (isFloat) {
                float norm = 1.0f / n;
                for (size_t i = 0; i < n; ++i) {
                    outFloat[2*i]   *= norm;
                    outFloat[2*i+1] *= norm;
                }
            } else {
                double norm = 1.0 / n;
                for (size_t i = 0; i < n; ++i) {
                    outDouble[2*i]   *= norm;
                    outDouble[2*i+1] *= norm;
                }
            }
        }
    }
    else if (calltype == FFT_R2C || calltype == IFFT_C2R)
    {
        size_t n;
        if (calltype == FFT_R2C) {
            n = inpCount;
        } else {
            n = outCount;
        }
        // STB_FFT_R2C / STB_IFFT_C2R
        {
            if (n < 2) {
                if (calltype == FFT_R2C) {
                    if (isFloat) {
                        outFloat[0] = inpFloat[0];
                        outFloat[1] = 0;
                    } else {
                        outDouble[0] = inpDouble[0];
                        outDouble[1] = 0;
                    }
                } else {
                    if (isFloat) {
                        outFloat[0] = inpFloat[0];
                    } else {
                        outDouble[0] = inpDouble[0];
                    }
                }
            } else {
                if (isFloat) {
                    llft_plan_float_real* planfr;
                    if (lfft->planfr && lfft->planfr_n == n) {
                        planfr = lfft->planfr;
                    } else {
                        if (lfft->planfr) {
                            free(lfft->planfr);
                            lfft->planfr = NULL;
                        }
                        planfr = lfft_create_plan_float_real(n);
                        if (planfr == NULL) {
                            return luaL_error(L, "error allocating memory");
                        }
                        lfft->planfr   = planfr;
                        lfft->planfr_n = n;
                    }
                    if (calltype == FFT_R2C) {
                        lfft_exec_float_fft_r2c(planfr, inpData, outData);
                    } else {
                        lfft_exec_float_ifft_c2r(planfr, inpData, outData);
                    }
                } else {
                    llft_plan_double_real* plandr;
                    if (lfft->plandr && lfft->plandr_n == n) {
                        plandr = lfft->plandr;
                    } else {
                        if (lfft->plandr) {
                            free(lfft->plandr);
                            lfft->plandr = NULL;
                        }
                        plandr = lfft_create_plan_double_real(n);
                        if (plandr == NULL) {
                            return luaL_error(L, "error allocating memory");
                        }
                        lfft->plandr   = plandr;
                        lfft->plandr_n = n;
                    }
                    if (calltype == FFT_R2C) {
                        lfft_exec_double_fft_r2c(plandr, inpData, outData);
                    } else {
                        lfft_exec_double_ifft_c2r(plandr, inpData, outData);
                    }
                }
            }
        }
        if (calltype == IFFT_C2R) {
            if (isFloat) {
                float norm = 1.0f / n;
                for (size_t i = 0; i < n; ++i) {
                    outFloat[i] *= norm;
                }
            } else {
                double norm = 1.0 / n;
                for (size_t i = 0; i < n; ++i) {
                    outDouble[i] *= norm;
                }
            }
        }
    }
    else {
        return luaL_error(L, "internal error");
    }
    return 1;
}

/* ============================================================================================ */

static int Lfft_fft_c2c(lua_State* L)
{
    return Lfft_call(L, FFT_C2C);
}

/* ============================================================================================ */

static int Lfft_ifft_c2c(lua_State* L)
{
    return Lfft_call(L, IFFT_C2C);
}

/* ============================================================================================ */

static int Lfft_fft_r2c(lua_State* L)
{
    return Lfft_call(L, FFT_R2C);
}

/* ============================================================================================ */

static int Lfft_ifft_c2r(lua_State* L)
{
    return Lfft_call(L, IFFT_C2R);
}

/* ============================================================================================ */

static const luaL_Reg ModuleClosures[] = 
{
    { "fft_c2c",       Lfft_fft_c2c   },
    { "ifft_c2c",      Lfft_ifft_c2c  },

    { "fft_r2c",       Lfft_fft_r2c   },
    { "ifft_c2r",      Lfft_ifft_c2r  },

    { NULL,            NULL } /* sentinel */
};

/* ============================================================================================ */

static int Lfft_new(lua_State* L)
{
    lua_newtable(L);
    pushNewLfftCache(L);
    luaL_setfuncs(L, ModuleClosures, 1);
    return 1;
}

/* ============================================================================================ */

static const luaL_Reg ModuleFunctions[] = 
{
    { "new",           Lfft_new  },

    { NULL,            NULL } /* sentinel */
};

/* ============================================================================================ */

int lfft_init_module(lua_State* L, int module)
{
    lua_pushvalue(L, module);
        pushNewLfftCache(L);
        luaL_setfuncs(L, ModuleClosures, 1);
        luaL_setfuncs(L, ModuleFunctions, 0);
    lua_pop(L, 1);

    return 0;
}

/* ============================================================================================ */
