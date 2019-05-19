## 编译测试

```
编译内核模块及测试程序
$ make test && make clean

执行测试程序
# ./watchdog-test.out 5
open device /dev/watchdog success
set watchdog timeout: 5 (s)
sleep 4 (s)
keepalive once
sleep 6 (s)         // 超时前未进行喂狗，系统自动重启
```

