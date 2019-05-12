## 编译测试

```
编译Lua测试程序
$ make && make clean

复制到网络根文件系统中
$ export LUA_TEST_PATH=$ROOTFS_ROOT_PATH/test/lua/call_c_function_in_lua
$ mkdir -p $LUA_TEST_PATH
$ cp -pfd *.so *.lua $LUA_TEST_PATH

查看测试文件
/test/lua/call_c_function_in_lua # ls                                           
module.lua  mylib.so                                                            

运行测试脚本
/test/lua/call_c_function_in_lua # ./module.lua                                 
0.99999968293183    
```


