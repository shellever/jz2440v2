## 编译测试

```
编译Lua测试程序
$ make && make clean

复制到网络根文件系统中
$ export LUA_TEST_PATH=$ROOTFS_ROOT_PATH/test/lua/call_lua_function_in_c
$ mkdir -p $LUA_TEST_PATH
$ cp -pfd main *.lua $LUA_TEST_PATH

查看测试文件
/test/lua/call_lua_function_in_c # ls                                           
main        mymath.lua                                                          

运行测试程序
/test/lua/call_lua_function_in_c # ./main                                                
The sum is 25 
```


