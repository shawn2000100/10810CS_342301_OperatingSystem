# MP2_report_37

## Team Member & Contributions

 * ## **資應碩二 107065522 陳子潔**
 
 * ## **數學大四 105021127 徐迺茜**

| 工作項目   | 分工            |
| ---------- | --------------- |
| Trace Code | 陳子潔 & 徐迺茜 |
| 功能實作   | 陳子潔          |
| 功能測試   | 徐迺茜          |
| 問題回答   | 陳子潔 & 徐迺茜 |

---

## 1. Trace code 
* Starting from “threads/kernel.cc, Kernel::ExecAll()”, “threads/thread.cc, thread::Sleep”, until “machine/mipssim.cc, Machine::Run()” is called for executing the first instruction from the user program



### threads/kernel.cc  Kernel::ExecAll()




### threads/thread.cc  thread::Sleep



### machine/mipssim.cc  Machine::Run()




---


## 2. Implement page table in NachOS
### Verification: 
![](https://i.imgur.com/o0anKcl.jpg)


### addrspace.h

### addrspace.c

### kernel.h

### kernel.c



---


## 3. Explain how NachOS creates a thread(process), load it into memory and place it into scheduling queue as requested in Part II-1 Your explanation on the functions along the code path should at least cover answer for the questions below


### How Nachos allocates the memory space for new thread(process)?


### How Nachos initializes the memory content of a thread(process), including loading the user binary code in the memory?


### How Nachos creates and manages the page table? 


### How Nachos translates address? 


### How Nachos initializes the machine status (registers, etc) before running a thread(process) 


### Which object in Nachos acts the role of process control block


### When and how does a thread get added into the ReadyToRun queue of Nachos CPU scheduler?
ff
55


----

## Reference
1. [向 NachOS 4.0 作業進發 (1) (實作好幫手!!!)](https://morris821028.github.io/2014/05/24/lesson/hw-nachos4/?fbclid=IwAR06r7ZH28w_hDLS4-h5Yjge63SZxq2VDtv28Rpa9JKhF51jTH3RlGM1wNk)
2. [OS::NachOS::HW1](http://blog.terrynini.tw/tw/OS-NachOS-HW1/)
3. [CSE120/Nachos中文教程.pdf (讚!!!)](https://github.com/zhanglizeyi/CSE120/blob/master/Nachos%E4%B8%AD%E6%96%87%E6%95%99%E7%A8%8B.pdf)
4. [C++：哪些變數會自動初始化？](https://www.itread01.com/content/1550033287.html?fbclid=IwAR1lsuTWlDjVVTe_V2ot1z7-Nf2oKj5XEsE63mdPrLQ2Bp6wlGcuxCWn9aI)
5. [C/C++ 中的 static, extern 的變數](https://medium.com/@alan81920/c-c-%E4%B8%AD%E7%9A%84-static-extern-%E7%9A%84%E8%AE%8A%E6%95%B8-9b42d000688f)
6. [C 語言程式的記憶體配置概念教學](https://blog.gtwang.org/programming/memory-layout-of-c-program/)
7. [列舉（Enumeration）](https://openhome.cc/Gossip/CppGossip/enumType.html)
8. [[C++]關於Callback Function](http://gienmin.blogspot.com/2013/03/ccallback-function.html)
9. [[教學]C/C++ Callback Function 用法/範例](http://dangerlover9403.pixnet.net/blog/post/83880061-%5B%E6%95%99%E5%AD%B8%5Dc-c++-callback-function-%E7%94%A8%E6%B3%95-%E7%AF%84%E4%BE%8B-(%E5%85%A7%E5%90%ABfunctio))
10. [虛擬函式（Virtual function）](https://openhome.cc/Gossip/CppGossip/VirtualFunction.html)
11. [深入理解C++中public、protected及private用法](https://www.jb51.net/article/54224.htm)
12. [C++中this指针的理解](https://blog.csdn.net/ljianhui/article/details/7746696)
13. [UNIX v6的进程控制块proc结构体和user结构体](https://www.suntangji.me/2017/12/18/proc%E7%BB%93%E6%9E%84%E4%BD%93%E5%92%8Cuser%E7%BB%93%E6%9E%84%E4%BD%93/)
14. [如何與 GitHub 同步筆記](https://hackmd.io/c/tutorials-tw/%2Fs%2Flink-with-github-tw)