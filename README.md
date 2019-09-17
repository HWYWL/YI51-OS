# YI51-OS
一个基于51单片机的操作系统

开发环境：
- 51内核单片机
- keil uvision5

该操作系统是基于51单片机开发而来，但其不仅仅局限于此，你可以把它移植到其他单片机上，如STM32。严格来说他还不算一个操作系统，只能算是一个微内核，但麻雀虽小五脏俱全，时间片轮转调度、抢占式和合作式任务，该有的基本都有了，其他有需要的可以自己扩充。我在”library”文件夹上放入了51单片机的库函数，如果你不习惯寄存器开发可以用库函数代替，也可以用那里的库函数来添加你想要的功能，好吧代码自己领悟。

	建议代码阅读顺序：config.h  SCH51.h  SCH51.C  main.c  T0_init.h  T0_init.c  TASK.h  task.c 

![](https://i.imgur.com/AsybWIK.jpg)

### 问题建议

- 联系我的邮箱：ilovey_hwy@163.com
- 我的博客：http://www.hwy.ac.cn
- GitHub：https://github.com/HWYWL
