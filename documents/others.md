## 1. 主题
+ 选择应用程序主题（浅色/深色/跟随系统）。注意，在`Windows`系统下，手动设置日间/夜间主题是无效的，仅建议使用**跟随系统**。
## 2. 数据结构自定义
+ 此选项可以自定义下位机节点发送的**数据帧**中携带的数据结构组成。
+ 受限于`DL-LN3X`模块最大传输长度，仅建议携带最大不超过`16`字节的数据。
+ 如需自定义数据结构，需从默认数据结构中删除一项或多项，再根据实际情况输入数据名称和选择数据类型。
+ 请注意，数据按照**小端序**排列。
+ 下位机如使用结构体组织传感器数据，请务必注意结构体**内存对齐**的问题。
+ 默认数据结构如下：

| 数据 | 数据类型（字节数） | 说明 |
|:----:|:----:|:----:|
| 温度 | `float, 4 Bytes` | 位于第`0`~`3`字节 |
| 湿度 | `float, 4 Bytes` | 位于第`4`~`7`字节 |
| 气体浓度 | `float, 4 Bytes` | 位于第`8`~`11`字节 | 
| 火焰指数 | `float, 4 Bytes` | 位于第`12`~`15`字节 | 