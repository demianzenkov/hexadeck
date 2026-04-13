######################################
# Hexadeck Bootloader Makefile
######################################

TARGET = hexadeck-bootloader

BUILD_DIRECTORY = build
DEBUG ?= 1
OPTIMIZATION ?= -Os

ifeq ($(DEBUG),1)
  OPTIMIZATION_FLAGS = -Og -g -gdwarf -ggdb
  RELEASE_DIRECTORY = $(BUILD_DIRECTORY)/debug
else
  OPTIMIZATION_FLAGS = $(OPTIMIZATION)
  RELEASE_DIRECTORY = $(BUILD_DIRECTORY)/release
endif

# Paths to shared firmware resources
FW_DIR = ../firmware

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/system_stm32f4xx.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/stm32f4xx_it.c \
Core/Src/usb_otg.c \
Core/Src/syscalls.c \
Core/Src/sysmem.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usb_device.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_desc.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App/usbd_midi_if.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Src/usbd_composite.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/MIDI/Src/usbd_midi.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_core.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ctlreq.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Src/usbd_ioreq.c \
$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target/usbd_conf.c

# C++ sources
CXX_SOURCES = \
Core/Src/main.cpp \
Src/bl_flash.cpp \
Src/bl_protocol.cpp

# ASM sources (reuse firmware's startup file)
ASM_SOURCES = \
$(FW_DIR)/Core/Startup/startup_stm32f446vetx.s

######################################
# Tools
######################################
ARM_PREFIX = arm-none-eabi-

ifdef ARM_GCC_PATH
  CC = $(ARM_GCC_PATH)/$(ARM_PREFIX)gcc
  CXX = $(ARM_GCC_PATH)/$(ARM_PREFIX)g++
  AS = $(ARM_GCC_PATH)/$(ARM_PREFIX)gcc -x assembler-with-cpp
  CP = $(ARM_GCC_PATH)/$(ARM_PREFIX)objcopy
  SZ = $(ARM_GCC_PATH)/$(ARM_PREFIX)size
  DP = $(ARM_GCC_PATH)/$(ARM_PREFIX)objdump
else
  CC = $(ARM_PREFIX)gcc
  CXX = $(ARM_PREFIX)g++
  AS = $(ARM_PREFIX)gcc -x assembler-with-cpp
  CP = $(ARM_PREFIX)objcopy
  SZ = $(ARM_PREFIX)size
  DP = $(ARM_PREFIX)objdump
endif

HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
LSS = $(DP) -h -S

OPENOCD ?= openocd

######################################
# CPU / FPU
######################################
CPU = -mcpu=cortex-m4
FPU = -mfpu=fpv4-sp-d16
FLOAT-ABI = -mfloat-abi=hard
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

######################################
# C/CXX Defines
######################################
C_DEFS = \
-DSTM32F446xx \
-DUSE_HAL_DRIVER

CXX_DEFS = $(C_DEFS)

######################################
# Includes
######################################
# Bootloader's own includes come FIRST to override firmware headers
C_INCLUDES = \
-ICore/Inc \
-IComposite \
-ISrc \
-I$(FW_DIR)/Drivers/CMSIS/Device/ST/STM32F4xx/Include \
-I$(FW_DIR)/Drivers/CMSIS/Include \
-I$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc \
-I$(FW_DIR)/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-I$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/App \
-I$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/COMPOSITE/Inc \
-I$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Class/MIDI/Inc \
-I$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Core/Inc \
-I$(FW_DIR)/Middlewares/Third_Party/AL94_USB_Composite/COMPOSITE/Target

######################################
# Compiler flags
######################################
ASFLAGS = $(MCU) $(C_INCLUDES) $(C_DEFS) $(OPTIMIZATION_FLAGS)

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAGS)
CFLAGS += -Wall -fdata-sections -ffunction-sections
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

CXXFLAGS = $(MCU) $(CXX_DEFS) $(C_INCLUDES) $(OPTIMIZATION_FLAGS)
CXXFLAGS += -fno-exceptions -fno-rtti
CXXFLAGS += -Wall -fdata-sections -ffunction-sections
CXXFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

######################################
# Linker flags
######################################
LDSCRIPT = STM32F446VETX_BOOTLOADER.ld

LIBS = -lc -lm -lnosys
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBS) \
          -Wl,-Map=$(BUILD_DIRECTORY)/$(TARGET).map,--cref \
          -Wl,--gc-sections -Wl,--print-memory-usage

######################################
# Build rules
######################################
# Object files
OBJECTS = $(addprefix $(RELEASE_DIRECTORY)/,$(notdir $(C_SOURCES:.c=.o)))
OBJECTS += $(addprefix $(RELEASE_DIRECTORY)/,$(notdir $(CXX_SOURCES:.cpp=.o)))
OBJECTS += $(addprefix $(RELEASE_DIRECTORY)/,$(notdir $(ASM_SOURCES:.s=.o)))

vpath %.c $(sort $(dir $(C_SOURCES)))
vpath %.cpp $(sort $(dir $(CXX_SOURCES)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# Default target
all: $(RELEASE_DIRECTORY)/$(TARGET).elf $(RELEASE_DIRECTORY)/$(TARGET).hex $(RELEASE_DIRECTORY)/$(TARGET).bin

# C build
$(RELEASE_DIRECTORY)/%.o: %.c Makefile | $(RELEASE_DIRECTORY)
	$(CC) -c $(CFLAGS) $< -o $@

# C++ build
$(RELEASE_DIRECTORY)/%.o: %.cpp Makefile | $(RELEASE_DIRECTORY)
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Assembly build
$(RELEASE_DIRECTORY)/%.o: %.s Makefile | $(RELEASE_DIRECTORY)
	$(AS) -c $(ASFLAGS) $< -o $@

# Link
$(RELEASE_DIRECTORY)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(RELEASE_DIRECTORY)/%.hex: $(RELEASE_DIRECTORY)/%.elf
	$(HEX) $< $@

$(RELEASE_DIRECTORY)/%.bin: $(RELEASE_DIRECTORY)/%.elf
	$(BIN) $< $@

$(RELEASE_DIRECTORY):
	mkdir -p $@

######################################
# Flash
######################################
flash: all
	$(OPENOCD) -f openocd.cfg -c "program $(RELEASE_DIRECTORY)/$(TARGET).elf verify reset exit"

######################################
# Clean
######################################
clean:
	rm -rf $(BUILD_DIRECTORY)

######################################
# Dependencies
######################################
-include $(wildcard $(RELEASE_DIRECTORY)/*.d)

.PHONY: all clean flash
