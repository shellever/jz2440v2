## 编译测试

```
1. 读写文件测试 (开发板操作)
安装内核模块
# insmod ramblock.ko                                           
ramblock: unknown partition table

查看设备文件
# ls -l /dev/ramblock                                           
brw-rw----    1 0        0        254,   0 Jan  1 00:37 /dev/ramblock

格式化磁盘
# mkdosfs /dev/ramblock
mkdosfs 2.11 (12 Mar 2005)                                                     

挂载磁盘
# mount /dev/ramblock /test/flash/ramblock/tmp

查看磁盘挂载信息
# df
Filesystem          1k-blocks      Used Available Use% Mounted on
tmpfs                    30592        0    30592  0% /dev
/dev/ramblock            1004        2      1002  0% /test/flash/ramblock/tmp

读写文件
# echo "hello world" > /test/flash/ramblock/tmp/firstme.txt
# cat /test/flash/rmablock/tmp/firstme.txt
hello world

将整个磁盘制作成镜像文件
# cat /dev/ramblock > /test/flash/ramblock.bin
// 然后在pc上挂载镜像文件(使用-o loop 来指定回环设备)，查看文件内容
// Ubuntu上挂载成功后会自动弹出1.0MB Volume对话框目录，直接查看到内容
$ mkdir ~/workspace/temp/ramblock
$ sudo mount -o loop ramblock.bin ~/workspace/temp/ramblock
$ sudo umount ~/workspace/temp/ramblock
$ rm -rf ~/workspace/temp/ramblock

卸载磁盘
# umount /test/flash/ramblock/tmp

卸载驱动
# rmmod ramblock



2. 读写计数情况测试
# cd /test/flash/ramblock
# insmod ramblock.ko
ramblock:do_ramblock_request read 1
unknown partition table

# lsmod
Module                  Size  Used by    Not tainted
ramblock                2648  0

# mkdosfs /dev/ramblock
mkdosfs 2.11 (12 Mar 2005)
do_ramblock_request read 2
do_ramblock_request read 3
do_ramblock_request write 1
do_ramblock_request write 2
do_ramblock_request write 3
do_ramblock_request write 4
do_ramblock_request write 5

# mount /dev/ramblock tmp
do_ramblock_request read 4
do_ramblock_request read 5
do_ramblock_request read 6
......
do_ramblock_request read 39
do_ramblock_request read 40
do_ramblock_request read 41
do_ramblock_request read 42

# echo "hello world" > tmp/firstme.txt
do_ramblock_request read 43

# sync                              // 使用系统调用来同步写入
do_ramblock_request write 6
do_ramblock_request write 7
do_ramblock_request write 8
do_ramblock_request write 9
do_ramblock_request write 10

# echo "hello ramblock" >> tmp/firstme.txt

# umount tmp/                       // 卸载磁盘时才会执行写入
do_ramblock_request write 11
do_ramblock_request write 12

# mount /dev/ramblock tmp
call do_ramblock_request, r_cnt = 44
call do_ramblock_request, r_cnt = 45
call do_ramblock_request, r_cnt = 46
......
call do_ramblock_request, r_cnt = 80
call do_ramblock_request, r_cnt = 81
call do_ramblock_request, r_cnt = 82
# cat tmp/firstme.txt
call do_ramblock_request, r_cnt = 83
hello world
hello ramblock
# umount tmp
call do_ramblock_request, w_cnt = 13



3. 磁盘分区操作
磁盘容量 = 磁头数 * 柱面数 * 扇区数 * 扇区大小
扇区数 = 磁盘容量 / 磁头数 / 柱面数 / 扇区大小

磁头数 - 即有多少面，这里假设有2面 heads
柱面数 - 即有多少环，这里假设有32环 cylinders
扇区数 - 即一环里有多少个扇区，通过公式计算出来 sectors


创建磁盘分区
# cd /test/flash/ramblock
# insmod ramblock.ko
# fdisk /dev/ramblock      // 指定需要创建分区的磁盘 /dev/ramblock
Device contains neither a valid DOS partition table, nor Sun, SGI or OSF disklal
Building a new DOS disklabel. Changes will remain in memory only,
until you decide to write them. After that the previous content
won't be recoverable.

Warning: invalid flag 0x00,0x00 of partition table 4 will be corrected by w(rit)

Command (m for help): m    // 查看分区操作命令
Command Action
a       toggle a bootable flag
b       edit bsd disklabel
c       toggle the dos compatibility flag
d       delete a partition
l       list known partition types
n       add a new partition                     // 新建一个新分区
o       create a new empty DOS partition table
p       print the partition table               // 打印分区表信息
q       quit without saving changes
s       create a new empty Sun disklabel
t       change a partition's system id
u       change display/entry units
v       verify the partition table
w       write table to disk and exit            // 写入分区表到磁盘，然后退出分区操作
x       extra functionality (experts only)                                     

Command (m for help): n                         // 新建一个新分区
Command action
   e   extended
   p   primary partition (1-4)
p                                               // 分区类型为主分区
Partition number (1-4): 1                       // 分区号为1
First cylinder (1-32, default 1): 1             // 起始柱面为1
Last cylinder or +size or +sizeM or +sizeK (1-32, default 32): 5    // 结束柱面为5

Command (m for help): p                         // 打印分区表信息

Disk /dev/ramblock: 1 MB, 1048576 bytes
2 heads, 32 sectors/track, 32 cylinders
Units = cylinders of 64 * 512 = 32768 bytes

        Device Boot      Start         End      Blocks  Id System
/dev/ramblock1               1           5         144  83 Linux

Command (m for help): n                         // 新建第二个新分区
Command action
   e   extended
   p   primary partition (1-4)
p                                               // 分区类型为主分区
Partition number (1-4): 2                       // 分区号为2
First cylinder (6-32, default 6): Using default value 6            // 起始柱面为默认6
Last cylinder or +size or +sizeM or +sizeK (6-32, default 32): 32  // 结束柱面为默认32

Command (m for help): p                         // 打印分区表信息

Disk /dev/ramblock: 1 MB, 1048576 bytes
2 heads, 32 sectors/track, 32 cylinders
Units = cylinders of 64 * 512 = 32768 bytes

        Device Boot      Start         End      Blocks  Id System
/dev/ramblock1               1           5         144  83 Linux
/dev/ramblock2               6          32         864  83 Linux

Command (m for help): w                         // 写入分区表到磁盘，使之生效，然后退出分区操作
The partition table has been altered!
ramblock: ramblock1 ramblock2

Calling ioctl() to re-read partition table
# ls -l /dev/ramblock*                          // 查看分区信息
brw-rw----    1 0        0        254,   0 Jan  1 02:51 /dev/ramblock
brw-rw----    1 0        0        254,   1 Jan  1 02:51 /dev/ramblock1
brw-rw----    1 0        0        254,   2 Jan  1 02:51 /dev/ramblock2

# mkdir tmp1
# mkdir tmp2
# mkdosfs /dev/ramblock1         // 格式化分区1
mkdosfs 2.11 (12 Mar 2005)
# mkdosfs /dev/ramblock2         // 格式化分区2
mkdosfs 2.11 (12 Mar 2005)
# mount /dev/ramblock1 tmp1      // 挂载分区1到tmp1目录
# mount /dev/ramblock2 tmp2      // 挂载分区2到tmp2目录

# du -h                          // 查看文件或目录大小
16.0k   ./tmp2
4.0k    ./tmp
16.0k   ./tmp1
1.2M    .
```



## 编译安装dosfstools工具

```
1.下载dosfstools源码包
dosfstools-2.11.tar.gz
https://codeload.github.com/dosfstools/dosfstools/tar.gz/v2.11

2. 编译dosfstools源码
$ tar -zxvf dosfstools-2.11.tar.gz   
$ cd dosfstools-2.11   
$ make CC=arm-linux-gcc  

3. 将编译生成的mkdosfs程序复制到开发板 /usr/sbin/ 目录下
$ cp -p mkdosfs $ROOTFS_ROOT_PATH/usr/sbin
```



## 参考文章

[关于系统中没有mkdosfs命令](https://blog.csdn.net/cai_caiyi/article/details/69260966)


