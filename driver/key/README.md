## 按键引脚说明

```
KEY
S2 - GPF0  - EINT0
S3 - GPF2  - EINT2
S4 - GPG3  - EINT11
S5 - GPG11 - EINT19
```


## 目录结构说明

```
key-simple      // 简单读取
key-interrupt   // 中断方式读取
key-poll        // poll超时查询读取
key-fasync      // 异步通知读取
key-mutex       // 互斥阻塞读取
key-timer       // 定时器防抖动
key-input       // 输入子系统
```
