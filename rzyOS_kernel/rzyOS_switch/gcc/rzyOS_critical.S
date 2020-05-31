.cpu cortex-m4
.syntax unified
.thumb
.text


.extern currentTask
.extern nextTask
.extern saveAndLoadStackAddr



.global  PendSV_Handler







.global PendSV_Handler
.type PendSV_Handler, %function
PendSV_Handler:
	MRS     R0, PSP

	STMFD   r0!, {r4-r11}				// 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈
	# VSTMDBEQ  r1!, {s16-s31}				// 保存浮点S16-31
	BL      saveAndLoadStackAddr		// 调用函数：参数通过R0传递，返回值也通过R0传递 
	# VLDMIANE  r1!, {s16-s31}				// 恢复浮点S16-31
	LDMFD   r0!, {r4-r11}				// 从下一任务的堆栈中，恢复R4~R11

	MSR     PSP, R0
	MOV     LR, #0xFFFFFFED				// 指明返回异常时使用PSP。注意，这时LR不是程序返回地址
	BX      LR




