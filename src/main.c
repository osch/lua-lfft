#include "util.h"
#include "main.h"
#include "lfft.h"

#ifndef LFFT_VERSION
    #error LFFT_VERSION is not defined
#endif 

#define LFFT_STRINGIFY(x) #x
#define LFFT_TOSTRING(x) LFFT_STRINGIFY(x)
#define LFFT_VERSION_STRING LFFT_TOSTRING(LFFT_VERSION)

/* ============================================================================================ */

static const char* const LFFT_MODULE_NAME = "lfft";

/* ============================================================================================ */


DLL_PUBLIC int luaopen_lfft(lua_State* L)
{
    luaL_checkversion(L); /* does nothing if compiled for Lua 5.1 */

    /* ---------------------------------------- */

    int n = lua_gettop(L);
    
    int module      = ++n; lua_newtable(L);

    lua_pushliteral(L, LFFT_VERSION_STRING);
    lua_setfield(L, module, "_VERSION");
    
    lua_checkstack(L, LUA_MINSTACK);
    
    lfft_init_module(L, module);

    lua_newtable(L);                                   /* -> module meta */
    lua_pushstring(L, LFFT_MODULE_NAME);               /* -> module meta, "lfft" */
    lua_setfield(L, -2, "__metatable");                /* -> module meta */
    lua_setmetatable(L, module);                       /* -> */
    
    lua_settop(L, module);

    return 1;
}

/* ============================================================================================ */
