#====================================================================#
# @file         Makefile
# @author       renzhongyu
# @version      V1.0
# @copyright    open source
# @date         2020-6
#====================================================================#
#Output files
EXECUTABLE=rzyOS.elf
BIN_IMAGE=rzyOS.bin

#======================================================================#
#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#======================================================================#
#Flags
CFLAGS+=-Wall
CFLAGS+=-Wfatal-errors
CFLAGS+=-Wno-incompatible-pointer-types
# CFLAGS+=-Wno-unused-but-set-variable
# CFLAGS+=-Wno-unused-variable
CFLAGS+=-g
CFLAGS+=-mlittle-endian
CFLAGS+=-mthumb 
# CFLAGS+=-nostdlib
CFLAGS+=-nostartfiles
CFLAGS+=-lgcc -lc -lm --specs=nosys.specs
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-mfloat-abi=hard
# CFLAGS+=-print-multi-lib
CFLAGS+=-mfpu=fpv4-sp-d16
# CFLAGS+=-fsingle-precision-constant -finline-functions -Wdouble-promotion -std=gnu99
# CFLAGS+=-u _scanf_float -u _printf_float -lgcc -lc -lm
CFLAGS+=-u _scanf_float -u _printf_float


CFLAGS+=-D USE_STDPERIPH_DRIVER
CFLAGS+=-D STM32F40_41xxx
CFLAGS+=-D __FPU_PRESENT=1
CFLAGS+=-D __FPU_USED=1

#optimize option
CFLAGS+=-Os
CFLAGS+=-ffunction-sections -fdata-sections -Wl,--gc-sections

#stm32-flash
CFLAGS+=-Wl,-T,../../../arch/arm/STM32F4xx_StdPeriph_Driver/linked/GCC/STM32F40_41xxx/STM32F407VGT6_FLASH.ld

#======================================================================#
#Libraries

#Stm32 libraries
ST_LIB=../../../arch/arm/STM32F4xx_StdPeriph_Driver/FWLIB

#libc
# CFLAGS+=-I../../../libc

#CMSIS libraries
CFLAGS+=-I../../../arch/arm/STM32F4xx_StdPeriph_Driver/CMSIS

#STM32 StdPeriph includes
CFLAGS+=-I$(ST_LIB)/inc
CFLAGS+=-I../../../arch/chip/STM32F4xx_StdPeriph_Templates

#rzyOS kernel includes
CFLAGS+=-I../../../rzyOS_kernel

#rzyOS memory
CFLAGS+=-I../../../rzyOS_kernel/rzyOS_memory

#rzyOS device tree
CFLAGS+=-I../../../rzyOS_kernel/rzyOS_vfs

#rzyOS lib includes
CFLAGS+=-I../../../rzyOS_lib

#rzyOS printf include
CFLAGS+=-I../../../rzyOS_kernel/rzyOS_printf

# Driver includes
CFLAGS+=-I./device/LED
CFLAGS+=-I./device/UART

# BSP include
CFLAGS+=-I./device/BSP


RZYOS_KERNEL_DIR=../../../rzyOS_kernel

RZYOS_LIB_DIR=../../../rzyOS_lib

# rzyOS switch
SWITCH_DIR=../../../rzyOS_kernel/rzyOS_switch/gcc

# rzyOS memory
MEMORY_DIR=../../../rzyOS_kernel/rzyOS_memory

#rzyOS device tree
DEVICE_TREE_DIR+=../../../rzyOS_kernel/rzyOS_vfs

#rzyOS printf
RZYOS_PRINTF_DIR+=../../../rzyOS_kernel/rzyOS_printf

#======================================================================#
#setup system clock
SRC=../../../arch/chip/STM32F4xx_StdPeriph_Templates/system_stm32f4xx.c

#StdPeriph
SRC+=$(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \
	$(ST_LIB)/src/stm32f4xx_usart.c

#Major programs
SRC+=./app/main.c \
	./device/LED/led.c \
	./device/UART/uart.c \
	./device/BSP/buffer.c


#libc
# SRC+=../../../libc/*.c

SRC+=$(RZYOS_KERNEL_DIR)/*.c

SRC+=$(RZYOS_LIB_DIR)/*.c

SRC+=$(SWITCH_DIR)/rzyOS_switch.o 
# SRC+=$(SWITCH_DIR)/save_and_load_stack.o 
# SRC+=$(SWITCH_DIR)/rzyOS_critical.o 

#memory mangement
# SRC+=$(MEMORY_DIR)/*.c
SRC+=$(MEMORY_DIR)/rzyOS_mm1.c

#rzyOS device tree
SRC+=$(DEVICE_TREE_DIR)/*.c

#rzyOS printf
SRC+=$(RZYOS_PRINTF_DIR)/*.c

# syscall
SRC+=syscall/syscall.c

#======================================================================#

#STM32 startup file
STARTUP=../../../arch/arm/STM32F4xx_StdPeriph_Driver/startup/gcc_ride7/startup_stm32f40_41xxx.s

#======================================================================#
#Make rules

#Make all
all:$(BIN_IMAGE)

$(BIN_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = startup_stm32f40_41xxx.o

$(STARTUP_OBJ): $(STARTUP) 
	$(CC) $(CFLAGS) $^ -c $(STARTUP)

$(EXECUTABLE):$(SRC) $(STARTUP_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)
	rm -f *.o
	rm -f $(SWITCH_DIR)/*.o
	
#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000
openocd:
	openocd -f "../commom/openocd.cfg"
gdb:
	arm-none-eabi-gdb -x ../commom/gdb_init.gdb
gdbtui:
	arm-none-eabi-gdb -tui -x ../commom/gdb_init.gdb
#======================================================================
.PHONY:all clean flash
