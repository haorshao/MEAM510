################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../communication.c \
../findPuck.c \
../localization.c \
../main.c \
../m_bus.c \
../m_rf.c \
../m_usb.c \
../m_wii.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
communication.o \
findPuck.o \
localization.o \
main.o \
m_bus.o \
m_rf.o \
m_usb.o \
m_wii.o

OBJS_AS_ARGS +=  \
communication.o \
findPuck.o \
localization.o \
main.o \
m_bus.o \
m_rf.o \
m_usb.o \
m_wii.o

C_DEPS +=  \
communication.d \
findPuck.d \
localization.d \
main.d \
m_bus.d \
m_rf.d \
m_usb.d \
m_wii.d

C_DEPS_AS_ARGS +=  \
communication.d \
findPuck.d \
localization.d \
main.d \
m_bus.d \
m_rf.d \
m_usb.d \
m_wii.d

OUTPUT_FILE_PATH +=Red.elf

OUTPUT_FILE_PATH_AS_ARGS +=Red.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"D:\Penn Learning\MEAM510\Software\7.0\Packs\atmel\ATmega_DFP\1.0.90\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega32u4 -B "D:\Penn Learning\MEAM510\Software\7.0\Packs\atmel\ATmega_DFP\1.0.90\gcc\dev\atmega32u4" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.9.2
	$(QUOTE)D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Red.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega32u4 -B "D:\Penn Learning\MEAM510\Software\7.0\Packs\atmel\ATmega_DFP\1.0.90\gcc\dev\atmega32u4"  
	@echo Finished building target: $@
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Red.elf" "Red.hex"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Red.elf" "Red.eep" || exit 0
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Red.elf" > "Red.lss"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Red.elf" "Red.srec"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "Red.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Red.elf" "Red.a" "Red.hex" "Red.lss" "Red.eep" "Red.map" "Red.srec" "Red.usersignatures"
	