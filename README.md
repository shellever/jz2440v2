## 开发环境

```
主机系统：  Ubuntu 16.04 LTS 
开发板型号：jz2440v2
开发工具：  vim (install through quokka-vim)
```


## 目录说明

```
hardware    - 裸板程序
uboot       - 引导程序
kernel      - 内核源码
busybox     - 根文件系统
driver      - 驱动程序
gui         - 图形界面

output      - 编译生成文件
build       - 编译环境配置脚本
tools       - 交叉编译工具等
document    - 相关电子文档
```


## 下载及配置环境

```
// maybe should setup postBuffer 2GB firstly
$ git config --global http.postBuffer 2000000000
$ git clone https://github.com/shellever/jz2440v2.git
$ cd jz2440v2
$ source build/envsetup.sh
```


## 参考链接

- [百问Linux嵌入式论坛](http://bbs.100ask.org/forum.php)
- [Linux kernel coding style](https://www.kernel.org/doc/html/latest/process/coding-style.html)

