# muduo-core
基于 Linux C++11 复刻 Muduo 核心高并发网络库，保留原版 Reactor 架构设计，去除 Boost 依赖，聚焦事件驱动、IO 多路复用核心原理实现。

## 项目概述
本项目严格遵循 Muduo 经典 **One Loop Per Thread** 设计思想，基于 Epoll 实现 IO 多路复用，以 EventLoop、Channel 为核心骨架搭建标准 Reactor 事件驱动模型，实现多线程主从 Reactor 高并发 TCP 网络库，完整复刻原生 Muduo 网络编程核心流程与设计范式。

## 核心核心组件详解
### 1. EventLoop 事件循环
EventLoop 是整个网络库的**核心调度中枢**，每个线程有且仅有一个 EventLoop 实例。
- 负责持续循环调用 Epoll 阻塞等待事件就绪
- 统一分发所有 IO 事件、读写事件、关闭事件
- 维护任务队列，支持跨线程投递任务、异步执行回调
- 绑定线程唯一性，保证事件循环不会跨线程错乱
- 驱动整个 Reactor 模型运转，是所有事件的管理者与调度者

### 2. Channel 事件通道
Channel 是**文件描述符与事件回调的封装载体**，是 Reactor 模型的最小事件单元。
- 封装 fd、可读、可写、异常、关闭事件
- 绑定各类事件对应的回调函数
- 向 EventLoop 注册/注销监听事件
- 不持有 fd 所有权，只做事件描述与回调分发
- Epoll 监听到就绪事件后，由 EventLoop 调度对应 Channel 执行回调

### 3. EpollPoller 封装 Epoll 多路复用
对 Linux 原生 Epoll 系统调用进行面向对象封装，作为 EventLoop 底层事件等待器。
- 封装 epoll_create、epoll_ctl、epoll_wait 核心调用
- 支持文件描述符的事件注册、修改、删除
- 阻塞等待内核 IO 事件就绪，批量返回活跃 Channel
- 兼容 LT 水平触发、ET 边缘触发两种工作模式
- 向上为 EventLoop 提供统一事件查询接口，屏蔽底层系统调用细节

## 整体工作流程
1. EpollPoller 通过 epoll_wait 阻塞等待内核 IO 事件到达；
2. 拿到就绪 fd 与事件，找到对应绑定的 Channel；
3. EventLoop 遍历就绪 Channel，根据事件类型调用注册好的读写/关闭回调；
4. 所有事件都在所属 EventLoop 线程内串行处理，避免多线程竞争；
5. 新连接由 Acceptor 接管，分配给子 Reactor 的 EventLoop 进行后续事件管理。

## 其他模块
- Acceptor：封装服务端 Socket、bind、listen、accept，负责监听接收新连接
- TcpServer：网络库对外入口，管理线程池、连接生命周期、回调注册
- TcpConnection：封装单条 TCP 连接，管理收发、断连、状态流转
- Buffer：应用层读写缓冲区，自动扩容，解决 TCP 粘包问题
- EventLoopThreadPool：事件循环线程池，实现主从 Reactor 负载均衡

## 编译运行
```bash
mkdir build
cd build
cmake ..
make -j
cd ../example
./testserver