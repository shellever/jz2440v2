## 移植web服务器boa (待验证)

```
1. 下载boa源码包
$ wget http://www.boa.org/boa-0.94.13.tar.gz

2. 安装依赖工具
$ sudo apt-get install bison flex
$ sudo apt-get install byacc

3. 解压并进入源码目录
$ tar -zxvf boa-0.94.13.tar.gz
$ cd boa-0.94.13/src

4. 配置生成Makefile文件
$ ./configure

5. 修改相关源文件
5.1 修改Makefile，使用交叉编译工具
$ vi Makefile
CROSS_COMPILE = arm-linux-
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)gcc -E

5.2 修改头文件compat.h，去掉宏连接符号##
$ vi compat.h +120
#define TIMEZONE_OFFSET(foo) foo->tm_gmtoff

5.3 修改源文件boa.c，注释掉下面几行代码(暂时未修改)
$ vi boa.c +225
/*if (setuid(0) != -1) {
    DIE("icky Linux kernel bug!");
}*/

5.4 修改头文件defines.h，更改boa配置文件搜索路径(默认即可)
$ vi defines.h +30
#define SERVER_ROOT "/etc/boa"

6. 编译生成boa可执行程序
$ make

7. 查看boa文件属性
$ file boa

8. 去掉boa可执行程序中的调试信息
$ arm-linux-strip boa

9. 创建boa服务器运行环境
$ export BOA_INSTALL_DIR=$LINUX_ARM_ROOT_PATH/output/busybox/boa/boa-0.94.13
$ mkdir -p $BOA_INSTALL_DIR/sbin
$ mkdir -p $BOA_INSTALL_DIR/usr/lib/boa
$ mkdir -p $BOA_INSTALL_DIR/etc
$ mkdir -p $BOA_INSTALL_DIR/var/log/boa
$ mkdir -p $BOA_INSTALL_DIR/var/www/cgi-bin
$ cp -fp boa $BOA_INSTALL_DIR/sbin/
$ cp -fp boa_indexer $BOA_INSTALL_DIR/usr/lib/boa/
$ cp -fp /etc/mime.types $BOA_INSTALL_DIR/etc/      // 直接使用主机上的mime配置文件即可
$ cp -fp ../boa.conf $BOA_INSTALL_DIR/etc/boa/

10. 修改boa配置文件boa.conf
$ vi $BOA_INSTALL_DIR/etc/boa/boa.conf
Port  80                                     // 服务访问端口
User  0 
Group 0 
#Listen 192.68.0.5
ErrorLog   /var/log/boa/error_log            // 错误日志
AccessLog  /var/log/boa/access_log           // 访问日志
DocumentRoot /var/www                        // HTML文档的主目录
UserDir public_html 
DirectoryIndex index.html                    // 默认访问网页文件
DirectoryMaker /usr/lib/boa/boa_indexer
KeepAliveMax 1000                            // 一个连接所允许的HTTP持续作用请求最大数目
KeepAliveTimeout 10                          // HTTP持续作用中服务器在两次请求之间等待的时间数，以秒为单位
MimeTypes /etc/mime.types                    // 指明mime.types文件位置
DefaultType text/plain                       // 文件扩展名没有或未知的话，使用的缺省MIME类型
CGIPath /bin:/usr/bin:/usr/local/bin         // 提供CGI程序的PATH环境变量值
Alias /doc /usr/doc                          // 为路径加上别名
ScriptAlias /cgi-bin/ /var/www/cgi-bin/      // 输入站点和CGI脚本位置

11. 创建index.html文件
$ vi $BOA_INSTALL_DIR/var/www/index.html
<html>
<body>
    <h3>hello world and hello boa!</h3><br/>
    <a href="/cgi-bin/test.cgi">goto cgi page</a>
</body>
</html>

12. 创建cgi解析程序
$ vi $BOA_INSTALL_DIR/var/www/cgi-bin/test.c
#include <stdio.h>

int main(int argc, char *argv[])
{  
    printf("Content-type:text/html\n\n");
    printf("<html><body>");
    printf("<font style=\"color:red; font-size:30px;\">Hello, CGI!</font><br/>");
    printf("<a href=\"/index.html\">return index.html</a>");
    printf("</body></html>");

    return 0;
}

13. 编译cgi测试程序
$ cd $BOA_INSTALL_DIR/var/www/cgi-bin
$ arm-linux-gcc -o test.cgi test.c

14. 运行boa服务器(开发板上)
使用-c选项来指定boa.conf配置文件存放目录(或者修改defines.h中的ServerRoot也可以)
//# /sbin/boa -c /etc/boa &   // 默认为/etc/boa
# /sbin/boa &

15. 浏览器测试
输入开发板上的ip地址，访问默认的index.html页面
```


## 参考文章

[BOA服务器移植](https://blog.csdn.net/feit2417/article/details/84777523)

