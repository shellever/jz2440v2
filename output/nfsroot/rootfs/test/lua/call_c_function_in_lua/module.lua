#!/bin/lua


--[[ 
require的参数对应C库中"luaopen_mylib()"中的"mylib"
C库就放在"module.lua"的同级目录，"require"可以找到。
--]]
local mylib = require("mylib")

-- 通过调用C库中的函数实现
print(mylib.mysin(3.14 / 2))    --> 0.99999968293183
