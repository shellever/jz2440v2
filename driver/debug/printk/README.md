## 编译测试

```
编译内核模块及测试程序
$ make && make test


安装驱动模块
/test/debug/printk # insmod debug-printk.ko                                     
[debug] call debug_printk_init                                                  
[debug] call debug_printk_init: kmalloc success                                 
[debug] call debug_printk_init: misc_register success                           


查看已安装模块信息
/test/debug/printk # lsmod                                                      
Module                  Size  Used by    Not tainted                            
debug_printk            2852  0                     


卸载驱动模块
/test/debug/printk # rmmod debug_printk                                         
[debug] call debug_printk_exit


测试程序
/test/debug/printk # ./debug-printk-test.out                                    
[debug] call debug_printk_open                                                  
[debug] call debug_printk_read                                                  
[debug] call debug_printk_write                                                 
[debug] call debug_printk_ioctl                                                 
[debug] call debug_printk_release                                               
```


