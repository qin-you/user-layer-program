# User-Mode Programs

用于记录自己在linux平台写的各种应用层程序

## Contents
每个子目录代表多个同类的简单的程序 或 一个较复杂的程序\
每个子目录下有个README对该程序说明\
每个子目录下一般有Makefile文件，便于编译测试

## Brief Description

- echo-server\
[README](echo-server/README)\
完成两台机器上两个进程的简单通信，通过流套接字、tcp协议。server端接收到client端的数据后原样回复给client。 支持并发访问。medium

- uart-test

- net-analyse\
[README](net-analyse/README)\
使用原始套接字实现的一个简易的网络分析器，也就是抓包。需要能轻度使用原始套接字，熟悉各协议头部格式，能从头部中提取所需信息。medium

- ping\
[README](ping/README)\
编写常见的ping程序，要求能ping域名和ip，输出与常规ping基本一致。 需要熟悉封包解包、原始套接字使用、icmp和ip头部格式、信号捕捉等。 编写和调试花了8h左右。hard

- process\
[README](process/README)\
apue书籍学习时写的近20个小程序，练习系统编程常用api，多与进程相关。如内存映射、管道、信号等。easy

- pthread\
[README](pthread/README)\
apue书籍学习时写的线程相关的程序，如生产者消费者模型，多线程交替打印等。easy