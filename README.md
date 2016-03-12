#LibExecutor简介  
利用面向对象思想，将常用Linux API封装为一个基于C++的执行体库程序。该库包含了线程和进程的创建模型、同步模型以及通信模型。 

具体包含：

1. 日志系统的实现，利用缓存提升了写日志的效率；利用注册终止函数atexit解决了程序退出时的日志丢失问题；利用本库的封装的线程同步模型和进程同步模型，实现了线程安全和进程安全的日志系统。
2. 线程的封装，包括线程创建，线程同步（互斥量，条件变量的封装），线程消息循环（包括消息，消息队列的建立，消息循环机制，消息处理机制，名字服务等的封装）。 
3. 进程的封装，包括进程创建的封装，消除了子进程文件描述符继承带来的问题；进程同步的封装（记录锁，共享存储，共享互斥量，共享条件变量，共享事件对象等的封装），进程通信的封装（消息对象的序列化和反序列化框架，命名管道型消息队列，基于命名管道的消息发送机制，基于命名管道的消息接收机制，进入消息循环的方式等的封装）
4. 使用本执行体库，利用map-reduce思想实现了单机多进程协作的WordCount（词频统计）程序。

##开发环境
- Ubuntu 14.04 + Eclipse CDT + GCC 4.8.4   

##开发经验记录  
[这里](https://github.com/bohaoist/LibExcutor/blob/master/RECORD.txt)记录了本人开发过程中的一些想法，遇到的问题和解决办法，以及积累收获到的经验。 
