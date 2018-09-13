
# TCP, Transmission Control Protocol


# SCTP, Stream Control Transmission Protocol
为两个系统之间提供关联, 其它特性:
* 可靠性高
* 保证数据有序
* 流控制
* 全双工
* 基于消息, 有消息长度, 而不是像 TCP 那样的数据流.
* 在系统之间可建立多个连接, 每个连接之间互不影响
* 支持多地址组网, 提高了网络可靠性, 在两个系统或者端点(endpoints)之间建立多个
连接, 走不同的网络, 当一个连接失效后可以切换到另一个连接.

是面向系统的, 而不是指在两个IP之间建立连接, 因为系统可能是多地址的.
