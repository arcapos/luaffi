local ffi = require 'ffi'
local unix = require 'unix'


local libc = unix.dlopen('/usr/lib64/libc-2.28.so', 'lazy')
local t = libc.puts
local puts = ffi.prep_cif(t, 'int32', 'pointer')
print('puts', puts('hello, world'))

-- ffi.prep_cif(libc.exit, 'void', 'int64')(2)

local f, status = ffi.prep_cif(libc.getpid, 'int32')
print('getpid', f())
os.execute('ps')

local libm = unix.dlopen('/usr/lib64/libm-2.28.so', 'lazy')
local floor, status = ffi.prep_cif(libm.floor, 'double', '*double')

print(status, 'floor', floor(3.5))

local scanf, status = ffi.prep_cif_var(libc.scanf, 1, 'pointer', '*double',
    '*double')

local x, y = scanf('%f %f')
