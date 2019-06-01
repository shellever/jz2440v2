## 编译测试

```
//
// 测试3:
// 添加发包函数 (即设置 vnet_dev->hard_start_xmit = virt_net_send_packet;)
// 添加统计信息
// 添加MAC地址
// 添加虚拟网卡驱动程序：将发送过来的ping包直接返回去，使得可以ping通3.3.3.4
//     

# insmod vnet.ko
# ifconfig vnet0 up 
# ifconfig vnet0 3.3.3.3
# ifconfig vnet0
vnet0     Link encap:Ethernet  HWaddr 08:89:89:89:89:11
          inet addr:3.3.3.3  Bcast:3.255.255.255  Mask:255.0.0.0
          UP BROADCAST RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

# ping -c 3 3.3.3.3
PING 3.3.3.3 (3.3.3.3): 56 data bytes
64 bytes from 3.3.3.3: seq=0 ttl=64 time=0.930 ms
64 bytes from 3.3.3.3: seq=1 ttl=64 time=0.489 ms
64 bytes from 3.3.3.3: seq=2 ttl=64 time=0.468 ms

--- 3.3.3.3 ping statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 0.468/0.629/0.930 ms

# ping -c 3 3.3.3.4
PING 3.3.3.4 (3.3.3.4): 56 data bytes
vnet_send_packet cnt = 1
64 bytes from 3.3.3.4: seq=0 ttl=64 time=0.947 ms
vnet_send_packet cnt = 2
64 bytes from 3.3.3.4: seq=1 ttl=64 time=0.521 ms
vnet_send_packet cnt = 3
64 bytes from 3.3.3.4: seq=2 ttl=64 time=0.509 ms

--- 3.3.3.4 ping statistics ---
3 packets transmitted, 3 packets received, 0% packet loss       // 可以ping通3.3.3.4
round-trip min/avg/max = 0.509/0.659/0.947 ms

# ifconfig vnet0
vnet0     Link encap:Ethernet  HWaddr 08:89:89:89:89:11
          inet addr:3.3.3.3  Bcast:3.255.255.255  Mask:255.0.0.0
          UP BROADCAST RUNNING NOARP MULTICAST  MTU:1500  Metric:1
          RX packets:3 errors:0 dropped:0 overruns:0 frame:0
          TX packets:3 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:294 (294.0 B)  TX bytes:294 (294.0 B)

```


