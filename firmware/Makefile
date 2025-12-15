######################################
# inculde lvgl makefile
######################################
LVGL_PATH ?= ${shell pwd}/Middlewares/Third_Party/lvgl
include $(LVGL_PATH)/lvgl.mk

######################################
# target
######################################
TARGET = hexadeck


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Os


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/gpio.c \
Core/Src/freertos.c \
Core/Src/adc.c \
Core/Src/dma.c \
Core/Src/spi.c \
Core/Src/tim.c \
Core/Src/usb_otg.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/stm32f4xx_hal_timebase_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_adc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
Core/Src/system_stm32f4xx.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Src/usbd_composite.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/MIDI/Src/usbd_midi.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_midi_if.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_core.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ctlreq.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ioreq.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target/usbd_conf.c \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Src/usbd_cdc_acm.c \
Core/Src/sysmem.c \
Core/Src/syscalls.c \
User/Modules/display/lv_lcd_custom_mipi.c \
User/Modules/ui/images.c \
User/Modules/ui/screens.c \
User/Modules/ui/styles.c \
User/Modules/ui/ui.c \

C_SOURCES += $(CSRCS)

CPP_SOURCES =  \
Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_cdc_acm_if.cpp \
Core/Src/main.cpp \
Core/Src/stm32f4xx_it.cpp \
User/Modules/bootloader/bootloader.cpp \
User/Modules/display/display.cpp \
User/Modules/nvs/nvs.cpp \
User/Tasks/task_acm.cpp \
User/Tasks/task_buttons.cpp \
User/Tasks/task_encoder.cpp \
User/Tasks/task_lvgl.cpp \
User/Tasks/task_midi.cpp \
User/Tasks/task_os.cpp 

CPP_SOURCES += $(CXXSRCS)

# ASM sources
ASM_SOURCES =  \
Core/Startup/startup_stm32f446vetx.s

ASM_SOURCES += $(ASRCS)

# ASMM sources
ASMM_SOURCES = 



#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CXX = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F446xx


# AS includes
AS_INCLUDES =  \
-ICore/Inc \
-IComposite \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IMiddlewares/Third_Party \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/MIDI/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IComposite \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/MIDI/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target \
-IMiddlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/CDC_ACM/Inc \
-IMiddlewares/Third_Party \
-IUser/Modules/bootloader \
-IUser/Modules/display \
-IUser/Modules/nvs \
-IUser/Modules/protocols \
-IUser/Modules/ui \
-IUser/Modules/state \
-IUser/Tasks


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ASFLAGS += $(AFLAGS)

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F446VETX_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################

# --- Improved object file handling for subdirectories ---
# List of objects, preserving subdirectory structure
OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES)) \
           $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(CPP_SOURCES)) \
           $(patsubst %.s,$(BUILD_DIR)/%.o,$(ASM_SOURCES)) \
           $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASMM_SOURCES))

# Pattern rules for building objects in subdirectories
$(BUILD_DIR)/%.o: %.c Makefile
	mkdir -p $(dir $@)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(basename $@).lst $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile
	mkdir -p $(dir $@)
	$(CXX) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile
	mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: %.S Makefile
	mkdir -p $(dir $@)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***