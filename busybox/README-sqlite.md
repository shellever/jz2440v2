## 移植嵌入式数据库SQLite

```
1. 下载sqlite源码包
$ wget https://www.sqlite.org/2019/sqlite-autoconf-3280000.tar.gz


2. 解压并进入源码目录
$ tar -zxvf sqlite-autoconf-3280000.tar.gz
$ cd sqlite-autoconf-3280000


3. 配置生成Makefile文件
// --host    指定交叉编译工具，一般为arm-linux，arm-linux-gnueabihf，arm-none-linux-gnueabi等
// --prefix  指定编译后生成文件的存放目录，必须是绝对路径
$ export SQLITE_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/sqlite/sqlite-autoconf-3280000
$ mkdir -p $SQLITE_INSTALL_DIR
$ ./configure --host=arm-linux --prefix=$SQLITE_INSTALL_DIR


4. 编译生成sqlite可执行程序等
$ make


5. 安装sqlite程序到指定目录下
$ make install


6. 查看安装完成的sqlite内容
$ ls $SQLITE_INSTALL_DIR
bin  include  lib  share


$ cd $SQLITE_INSTALL_DIR
$ file bin/sqlite3
sqlite3: ELF 32-bit LSB executable, ARM, version 1, dynamically linked, interpreter /lib/ld-, for GNU/Linux 2.4.3, not stripped


7. 复制sqlite编译生成的内容到开发板根文件系统中
$ cp -dpr $SQLITE_INSTALL_DIR/* $LINUX_ARM_ROOT_PATH/output/nfsroot/rootfs/


8. 开发板上测试运行sqlite3命令
.help    查看帮助
.quit    退出sqlite环境
# sqlite3                                                                
SQLite version 3.28.0 2019-04-16 19:49:53                                      
Enter ".help" for usage hints.                                                 
Connected to a transient in-memory database.                                   
Use ".open FILENAME" to reopen on a persistent database.                       
sqlite>


9. 编译sqlite的测试程序
// -I 指定头文件存放目录
// -L 指定动态库存放目录
// -l 指定动态库名称
$ arm-linux-gcc sqlite-test.c -o sqlite-test -L $SQLITE_INSTALL_DIR/lib -I $SQLITE_INSTALL_DIR/include -lsqlite3

开发板上运行测试程序
/test/sqlite # ./sqlite-test test.db "create table person(name varchar(20), age int);"
/test/sqlite # ./sqlite-test test.db "insert into person values('shellever', 18);"
/test/sqlite # ./sqlite-test test.db "insert into person values('linuxfor', 20);"
/test/sqlite # ./sqlite-test test.db "select * from person;"
name = shellever
age = 18

name = linuxfor
age = 20

/test/sqlite #
```


