# lua-lfft 
[![Licence](http://img.shields.io/badge/Licence-MIT-brightgreen.svg)](LICENSE)
[![build status](https://github.com/osch/lua-lfft/workflows/test/badge.svg)](https://github.com/osch/lua-lfft/actions/workflows/test.yml)
[![Build status](https://ci.appveyor.com/api/projects/status/v8t6rsf45dwt60pl/branch/master?svg=true)](https://ci.appveyor.com/project/osch/lua-lfft/branch/master)
[![Install](https://img.shields.io/badge/Install-LuaRocks-brightgreen.svg)](https://luarocks.org/modules/osch/lfft)

<!-- ---------------------------------------------------------------------------------------- -->

This [Lua] module provides a binding to [stb_fft], a fast fourier transform library 
suitable for power of 2 and non-power of 2 written by ZhiHan Gao.

All functions in this module are operating on [carray] arrays of *double* or *float* 
element data type. 

Examples:
   * [`example01.lua`](./examples/example01.lua) - demonstrates complex fourier transform
   * [`example02.lua`](./examples/example02.lua) - demonstrates real fourier transform

[Lua]:      https://www.lua.org
[stb_fft]:  https://github.com/cpuimage/stb_fft
[carray]:   https://github.com/osch/lua-carray


<!-- ---------------------------------------------------------------------------------------- -->
