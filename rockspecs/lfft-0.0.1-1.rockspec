package = "lfft"
version = "0.0.1-1"
local versionNumber = version:gsub("^(.*)-.-$", "%1")
source = {
  url = "https://github.com/osch/lua-lfft/archive/v"..versionNumber..".zip",
  dir = "lua-lfft-"..versionNumber,
}
description = {
  summary = "Fast fourier transform library",
  homepage = "https://github.com/osch/lua-lfft",
  license = "MIT",
  detailed = [[
    This Lua module provides a binding to stb_fft, a fast fourier transform library 
    suitable for power of 2 and non-power of 2 ( https://github.com/cpuimage/stb_fft ).
  ]],
}
dependencies = {
  "lua >= 5.1, <= 5.4",
  "carray"
}
build = {
  type = "builtin",
  modules = {
    lfft = {
      sources = { 
          "src/main.c",
          "src/lfft.c",
          "src/lfft_compat.c",
          "src/calcfloat.c",
          "src/calcdouble.c",
      },
      defines = { "LFFT_VERSION="..versionNumber },
    },
  }
}
