## 移植lua

```
1. 下载lua源码包
$ wget http://www.lua.org/ftp/lua-5.3.5.tar.gz


2. 解压并进入源码目录
$ tar -zxvf lua-5.3.5.tar.gz
$ cd lua-5.3.5


3. 修改lua源码
3.1 顶层Makefile中添加arm-linux目标
$ vi Makefile +40
PLATS= aix bsd c89 freebsd generic linux macosx mingw posix solaris arm-linux

3.2 子目录下src/Makefile中添加arm-linux目标
$ vi src/Makefile +30
PLATS= aix bsd c89 freebsd generic linux macosx mingw posix solaris arm-linux

3.3 子目录下src/Makefile中添加编译arm-linux目标的命令，并添加一个编译开关LUA_USE_ARM_LINUX
$ vi src/Makefile +110
linux:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl -lreadline"

arm-linux:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX -DLUA_USE_ARM_LINUX" SYSLIBS="-Wl,-E -ldl"

3.4 子目录下src/Makefile中添加交叉编译工具链前缀
$ vi src/Makefile +9
CROSS_COMPILE= arm-linux-
CC= $(CROSS_COMPILE)gcc -std=gnu99
AR= $(CROSS_COMPILE)ar rcu
RANLIB= $(CROSS_COMPILE)ranlib

3.5 使用编译开关LUA_USE_ARM_LINUX来去掉readline头文件
$ vi src/luaconf.h
#if defined(LUA_USE_LINUX)
#define LUA_USE_POSIX
#define LUA_USE_DLOPEN		/* needs an extra library: -ldl */
#if !defined(LUA_USE_ARM_LINUX)
#define LUA_USE_READLINE	/* needs some extra libraries */
#endif
#endif


4. 编译lua源码
$ make arm-linux


5. 安装lua程序到指定目录下
$ export LUA_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/lua/lua-5.3.5
$ mkdir -p $LUA_INSTALL_DIR
$ make INSTALL_TOP=$LUA_INSTALL_DIR install


6. 查看安装完成的lua内容
$ ls $LUA_INSTALL_DIR
bin  include  lib  man  share


7. 复制lua编译生成的内容到开发板根文件系统中
$ cp -fdpr $LUA_INSTALL_DIR/* $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/


8. 开发板上测试lua脚本
/test/lua # ./helloworld.lua                                                   
Hello world and hello me!


9. lua补丁文件操作
lua-5.3.5               // 未修改源码目录
lua-5.3.5-arm           // 修改过的源码目录
lua-5.3.5-arm.patch     // 补丁文件

9.1 生成补丁
$ diff -urN lua-5.3.5 lua-5.3.5-arm > lua-5.3.5-arm.patch

9.2 打补丁 (lua-5.3.5-arm.patch和lua-5.3.5位于同一个目录下)
$ cd lua-5.3.5 
$ patch -p1 < ../lua-5.3.5-arm.patch 
```


## 参考文章

- [lua交叉编译到arm-linux上](https://blog.csdn.net/firebolt2002/article/details/79808169)


