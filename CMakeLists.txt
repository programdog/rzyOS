cmake_minimum_required(VERSION 3.10)
project(rzyOS)

set(CMAKE_BUILD_TYPE "Release")

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
set(CMAKE_SIZE arm-none-eabi-size)

add_definitions(
-DUSE_STDPERIPH_DRIVER
-DSTM32F40_41xxx
)

set(__FPU_PRESENT 1)
set(__FPU_USED 1)

set(MCU_FLAGS "-mlittle-endian -mthumb -nostdlib -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant -finline-functions -Wdouble-promotion -std=gnu99")
set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "-O0 -g2 -ggdb")
set(CMAKE_C_FLAGS_RELEASE "-Os -ffunction-sections -fdata-sections -Wl,--gc-sections")
#cmake -DCMAKE_BUILD_TYPE=Debug ..
#cmake -DCMAKE_BUILD_TYPE=Release ..

set(ST_LIB arch/arm/STM32F4xx_StdPeriph_Driver/FWLIB)


include_directories(
	${ST_LIB}/inc
	arch/arm/STM32F4xx_StdPeriph_Driver/CMSIS
	arch/chip/STM32F4xx_StdPeriph_Templates
	rzyOS_kernel
	rzyOS_kernel/rzyOS_switch/gcc
	rzyOS_kernel/rzyOS_memory
	rzyOS_lib
	src/board/stm32f4discovery/drvice/LED
)

# add_library(
# 	arch/chip/STM32F4xx_StdPeriph_Templates/system_stm32f4xx.c
# )

# add_library(
# 	ST_LIB/src/misc.c
# 	ST_LIB/src/src/stm32f4xx_rcc.c
# 	ST_LIB/src/stm32f4xx_gpio.c
# )

# add_library(
# # 	app/main.c
# 	src/board/stm32f4discovery/drvice/LED/led.c
# )

file(GLOB RZYOS_KERNEL_DIR rzyOS_kernel/*.c)
file(GLOB RZYOS_LIB_DIR rzyOS_lib/*.c)
file(GLOB SWITCH_DIR rzyOS_kernel/rzyOS_switch/gcc/*.c)
file(GLOB MEMORY_DIR rzyOS_kernel/rzyOS_memory/*.c)
file(GLOB STARTUP arch/arm/STM32F4xx_StdPeriph_Driver/startup/gcc_ride7/startup_stm32f40_41xxx.s)

set_property(SOURCE ${STARTUP} PROPERTY LANGUAGE C)

add_library(${CMAKE_PROJECT_NAME}
	${STARTUP}
	arch/chip/STM32F4xx_StdPeriph_Templates/system_stm32f4xx.c

	${ST_LIB}/src/misc.c
	${ST_LIB}/src/stm32f4xx_rcc.c
	${ST_LIB}/src/stm32f4xx_gpio.c

	src/board/stm32f4discovery/drvice/LED/led.c

	${RZYOS_KERNEL_DIR}
	${RZYOS_LIB_DIR}
	${SWITCH_DIR}
	${MEMORY_DIR}
)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/arch/arm/STM32F4xx_StdPeriph_Driver/linked/GCC/STM32F40_41xxx/STM32F407VGT6_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS
"-nostartfiles -T${LINKER_SCRIPT} -Wl,-Map=${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map,--cref -Wl,--gc-sections"
)


add_executable(${CMAKE_PROJECT_NAME}.elf src/board/stm32f4discovery/app/main.c)

target_link_libraries(${CMAKE_PROJECT_NAME}.elf
	${CMAKE_PROJECT_NAME}
)

set(ELF_FILE ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.elf)
set(HEX_FILE ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin)


add_custom_command(TARGET "${PROJECT_NAME}.elf" POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -Obinary ${ELF_FILE} ${BIN_FILE}
	COMMAND ${CMAKE_OBJCOPY} -Oihex  ${ELF_FILE} ${HEX_FILE}
	COMMENT "Building ${PROJECT_NAME}.bin and ${PROJECT_NAME}.hex"

	COMMAND ${CMAKE_COMMAND} -E copy ${HEX_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.hex"
	COMMAND ${CMAKE_COMMAND} -E copy ${BIN_FILE} "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin"

	COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex 
)
