# 硬件配置说明

## stm32f4discovery 时钟源使用的是 外部8M晶振 ， 如果更换了外部晶振需要修改 ：
-	*1. stm32f4xx.h 中的 #define HSE_VALUE     ((uint32_t)25000000)*
-	*2. system_stm32f4xx.c 中的 #define PLL_M      25*

## 对于在ST官网新下载的STM32 Standard Peripheral Libraries文件， 需要修改 ：
-	*1. stm32f4xx_it.c 中的 #include "main.h" 需要删除*
-	*2. PendSV_Handler 与 SysTick_Handler 由rzyOS提供， 所以需要屏蔽*

## CM3与CM4
-	*1.相对于CM3来说， CM4多了浮点寄存器， 所以在PendSV中需要对浮点寄存器进行保存与恢复*
-	*2.在task初始化的过程中， 对任务堆栈初的始化写入也要注意浮点寄存器的地址位移， 确保首次任务恢复时能正常加载堆栈*

## 汇编
-	*1.windows MDK编译环境与linux GNU编译环境的汇编是不一样的-
