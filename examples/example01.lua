local lfft   = require("lfft")
local carray = require("carray") -- see https://github.com/osch/lua-carray

local N = 8

local a1 = carray.new("double", 2*N)

for i = 1, N do
    a1:set(2*i-1, i)
    a1:set(2*i,   0)
end

print("Input:")
for i = 1, N do
    print(string.format("%d: real = %6.3f, imag = %6.3f", i, a1:get(2*i-1), a1:get(2*i)))
end

local a2 = lfft.fft_c2c(a1)

print("Output:")
for i = 1, a2:len()/2 do
    print(string.format("%d: real = %6.3f, imag = %6.3f", i, a2:get(2*i-1), a2:get(2*i)))
end

local a3 = lfft.ifft_c2c(a2)

print("Inverse:")
for i = 1, a3:len()/2 do
    print(string.format("%d: real = %6.3f, imag = %6.3f", i, a3:get(2*i-1), a3:get(2*i)))
end
