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
CFLAGS+=-g
CFLAGS+=-mlittle-endian
CFLAGS+=-mthumb 
CFLAGS+=-nostdlib
CFLAGS+=-mfloat-abi=hard
CFLAGS+=-mfpu=fpv4-sp-d16
CFLAGS+=-mcpu=cortex-m4

CFLAGS+=-D USE_STDPERIPH_DRIVER
CFLAGS+=-D STM32F40_41xxx
CFLAGS+=-D __FPU_PRESENT=1
CFLAGS+=-D __FPU_USED=1
CFLAGS+=-D GCC_BUILD

#stm32-flash
CFLAGS+=-Wl,-T,../../../arch/arm/STM32F4xx_StdPeriph_Driver/linked/GCC/STM32F40_41xxx/STM32F407VGT6_FLASH.ld

#======================================================================#
#Libraries

#Stm32 libraries
ST_LIB=../../../arch/arm/STM32F4xx_StdPeriph_Driver/FWLIB

#CMSIS libraries
CFLAGS+=-I../../../arch/arm/STM32F4xx_StdPeriph_Driver/CMSIS

#StdPeriph includes
CFLAGS+=-I$(ST_LIB)/inc
CFLAGS+=-I../../../arch/chip/STM32F4xx_StdPeriph_Templates

CFLAGS+=-I../../..//rzyOS_kernel
KERNEL_DIR=../../../rzyOS_kernel

# rzyOS switch
SWITCH_DIR=../../../rzyOS_kernel/rzyOS_switch/gcc

# Driver includes
CFLAGS+=-I./drvice/LED

#======================================================================#
#setup system clock
SRC=../../../arch/chip/STM32F4xx_StdPeriph_Templates/system_stm32f4xx.c
#StdPeriph
SRC+=$(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \

#Major programs
SRC+=./app/main.c \
	./drvice/LED/led.c

SRC+=$(KERNEL_DIR)/*.c
	
SRC+=$(SWITCH_DIR)/rzyOS_switch.o 
# 	$(SWITCH_DIR)/save_and_load_stack.o 
# 	$(SWITCH_DIR)/rzyOS_critical.o 

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
