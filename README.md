# linux-data-link-layer

**due 12.9 23:59:59**

[toc]


## TODO
- [ ] 01 乌托邦式的单工协议
- [ ] 02 无错信道的单工停-等协议
- [ ] 03 有错信道上的单工停-等协议
- [ ] 04 1位窗口滑动协议
- [ ] 05 回退N协议
- [ ] 06 选择重传协议


## 代码规范
- xxxx.share.x共享文件命名规范：以发送者的名称作为前缀 

e.g. SNL与SDL的共享文件:
```C
int fd = open("snl.network_datalink.share.1", 0777);
```

## 信号规范 (see common.h)
| 信号名 | 信号量 | 描述 |
| ------ | ------ | ------ | 
| SIG_CHK_ERR | 35 | 校验码错误 |
| SIG_FRAME_ARRIVAL | 36 | 帧到达 |
| SIG_NETWORK_LAYER_READY | 37 | 网络层准备完毕 |
| SIG_ENABLE_NETWORK_LAYER | 38 | 开启网络层 |
| SIG_DISABLE_NETWORK_LAYER | 39 | 关闭网络层 |
| SIG_D2P | 40 | 数据链路层向物理层发送数据 |


# 单工协议栈

## SNL与SDL协议规范
- SNL每次收到38信号，就写入snl.network_datalink.share.x文件，发送计数器加一，然后向SDL发送37信号
- SDL每次调用from_network_layer()，读取相应文件，接受计数器加一，然后向SNL发送38信号


## SDL与SPL协议规范


## SPL与RPL协议规范


## RPL与RDL协议规范


## RDL与RNL协议规范
- RDL每次调用to_network_layer()，写入sdl.network_datalink.share.x文件，然后向RNL发送38信号
- RNL监听38信号，一旦收到38信号，就读取对应的文件，然后写到最终的输出文件中



# 双工协议栈

（很多与单工一样
