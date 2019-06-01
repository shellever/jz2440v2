## 编译测试

```
编译内核模块及测试程序
$ make && make test && make clean


安装驱动模块
# insmod proc-zmsg.ko                                     
# insmod debug-zprintk.ko                                     


查看已安装模块信息
# lsmod                                                        
Module                  Size  Used by    Not tainted                            
debug_zprintk           2884  0                                                 
proc_zmsg               4368  1 debug_zprintk


卸载驱动模块
# rmmod debug_zprintk                                         
# rmmod proc_zmsg                                        


测试程序
# ./debug-zprintk-test.out                                    


查看打印日志
# ls -l /proc/zmsg
-r--------    1 0        0               0 Jan  1 00:05 /proc/zmsg              

# cat /proc/zmsg                                               
[debug] call debug_zprintk_init                                                 
[debug] call debug_zprintk_init: kmalloc success                                
[debug] call debug_zprintk_init: misc_register success                          
[debug] call debug_zprintk_open                                                 
[debug] call debug_zprintk_read                                                 
[debug] call debug_zprintk_write                                                
[debug] call debug_zprintk_ioctl                                                
[debug] call debug_zprintk_release                                              
                                                                                
```


