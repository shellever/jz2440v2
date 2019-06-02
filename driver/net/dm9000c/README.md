## 编译测试

```
1. 把dm99000c.c放到内核的drivers/net目录下
2. 修改drivers/net/Makefile文件
//obj-$(CONFIG_DM9000) += dm9dev9000c.o
obj-$(CONFIG_DM9000) += dm99000c.o

3. 重新编译内核
$ make uImage

4. 进入uboot后使用NFS烧写新内核，并启动 (能正常进入系统则说明网卡驱动程序移植成功)

若非NFS启动，则使用下面命令进行测试即可
# ifconfig eth0 192.168.1.17
# ping 192.168.1.1   
```


