## 编译测试

```
安装dma驱动
# insmod dma.ko

非dma模式拷贝测试，即使用CPU正常拷贝，可以发现占用了大部分资源，输入 ls 无反应
/test/dma # ./dma-test nodma &                                                          
/test/dma # MEM_CPY_NO_DMA OK                                                           
MEM_CPY_NO_DMA OK                                                                       
MEM_CPY_NO_DMA OK                                                                       
MEM_CPY_NO_DMA OK                                                                       
lsMEM_CPY_NO_DMA OK                                                                     
MEM_CPY_NO_DMA OK                                                                       
MEM_CPY_NO_DMA OK                                                                       
lsMEM_CPY_NO_DMA OK


使用DMA拷贝，输入 ls 立马有反应，从而释放了CPU的压力
/test/dma # ./dma-test dma &                                                            
/test/dma # MEM_CPY_DMA OK                                                              
MEM_CPY_DMA OK                                                                          
MEM_CPY_DMA OK                                                                          
lsMEM_CPY_DMA OK                                                                        
MEM_CPY_DMA OK                                                                          

dma-test  dma.ko    uImage                                                              
/test/dma # MEM_CPY_DMA OK                                                              
MEM_CPY_DMA OK


查看中断请求使用情况
# cat /proc/interrupts
           CPU0
30:       4312         s3c  S3C2410 Timer Tick
32:          0         s3c  s3c2410-lcd
33:          0         s3c  s3c-mci                 // IRQ_DMA0
34:          0         s3c  I2SSDI                  // IRQ_DMA1
35:          0         s3c  I2SSDO                  // IRQ_DMA2
37:         12         s3c  s3c-mci
42:          0         s3c  ohci_hcd:usb1
43:          0         s3c  s3c2440-i2c
51:        777     s3c-ext  eth0
60:          0     s3c-ext  s3c-mci
70:         27   s3c-uart0  s3c2440-uart
71:         36   s3c-uart0  s3c2440-uart
79:          0     s3c-adc  s3c2410_action
80:          0     s3c-adc  s3c2410_action
83:          0           -  s3c2410-wdt
Err:          0
/ #
```


