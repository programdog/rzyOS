# rzyOS create by RenZhongYu[任仲雨]
*rzyOS is an operating system designed with a macro kernel conception*
*rzyOS Version : V1.0*

The scheduling strategy of rzyOS is preemptive priority & time slice rotation,the kernel can be clipped.Support for hardware floating calculate, task stack statistics, cpu usage statistics, most support 32 tasks, rzyOS has semaphore, work queue, mutex, memory management, message queue, event group, support priority inheritance. rzyOS has device tree，support posix interface, you can use open(),write(),read(),close().ioctl() to operate device driver.

rzyOS support construct by makefile and cmake in Linux.And support compile by Keil in Windows.


## Currently supported architecture
```
Cortex-M3
Cortex-M4
```

## Currently supported test platforms
```
stm32f4discovery
MDK simulation
```

## Currently supported develop environment
```
Linux(recommend)
Windows
```

### Linux
```
cmake：
1.mkdir build
2.cd build & cmake ..
3.make
```

```
makefile:
1.cd src/board/stm32f4discovery
2.make
3.make flash
```

### Windows
```
1.git tag change to V1.3
2.enter keil_proj directory
3.select platform(recommend proj_stm32f407)
4.compile
```

-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------


# rzyOS 被 RenZhongYu[任仲雨] 所创造
*rzyOS 是宏内核思想设计的操作系统*
*rzyOS Version : V1.0*

rzyOS 采用抢占式优先级+时间片轮转的调度策略，内核可裁剪。支持硬件浮点，task堆栈统计，CPU使用率统计。最多支持32个task，支持信号量，消息队列，互斥锁，内存管理，工作队列，事件组。支持优先级继承。支持设备树，支持posix接口，可通过open(),write(),read(),close().ioctl()对设备驱动进行操作。

rzyOS 支持linux与windows开发环境，拥有makefile 与 cmake 构建方式，以及window下MDK编译方式


## 目前支持的架构
```
Cortex-M3
Cortex-M4
```


## 目前支持的测试平台
```
stm32f4discovery
MDK simulation
```

## 支持的开发环境
```
Linux(推荐)
Windows
```

### Linux
```
cmake：
1.mkdir build
2.cd build & cmake ..
3.make
```

```
makefile:
1.cd src/board/stm32f4discovery
2.make
3.make flash
```

### Windows
```
1.git tag切换到V1.3
2.进入keil_proj目录
3.选择平台(推荐proj_stm32f407)
4.编译
```
