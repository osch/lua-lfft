local lfft   = require("lfft")
local carray = require("carray")

local function printf(...)
    io.write(string.format(...))
end

local function printlnComplex(a, i)
    printf("%d: %.3f", i, a:get(2*i-1))
    local img = a:get(2*i)
    printf(" %s i%.3f\n", (img < 0 and "-" or "+"), math.abs(img))
end
    

local TYPES = {
    "float",
    "double"
}
local R = 10
local N = 65536
local PRINT = false
local EPS = 0.000001

for _, t in ipairs(TYPES) do
    for r = 1, R do
        local a1 = carray.new(t, N)
        
        for i = 1, N do
            a1:set(i, math.random())
        end
        
        if PRINT then
            printf("\na1:\n")
            for i = 1, N do
                printf("%d: %f\n", i, a1:get(i))
            end
        end
        
        local a2 = lfft.fft_r2c(a1)
        
        if PRINT then
            printf("\na2:\n")
            for i = 1, a2:len()/2 do
                printlnComplex(a2, i)
            end
        end
        
        local a3 = lfft.ifft_c2r(a2)
        
        if PRINT then
            printf("\na3:\n")
            for i = 1, N do
                printf("%d: %.2f\n", i, a3:get(i))
            end
        end
        
        for i = 1, N do
            assert(math.abs(a3:get(i) - a1:get(i)) < EPS, string.format("error: %d: %f, %f", i, a3:get(i), a1:get(i)))
        end
    end
end
print("FINISHED test02")
