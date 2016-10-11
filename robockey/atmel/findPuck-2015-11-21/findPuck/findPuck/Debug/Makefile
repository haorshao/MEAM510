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
../findPuck.c \
../m_usb.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
findPuck.o \
m_usb.o

OBJS_AS_ARGS +=  \
findPuck.o \
m_usb.o

C_DEPS +=  \
findPuck.d \
m_usb.d

C_DEPS_AS_ARGS +=  \
findPuck.d \
m_usb.d

OUTPUT_FILE_PATH +=findPuck_6_2.elf

OUTPUT_FILE_PATH_AS_ARGS +=findPuck_6_2.elf

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
	$(QUOTE)D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="findPuck_6_2.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega32u4 -B "D:\Penn Learning\MEAM510\Software\7.0\Packs\atmel\ATmega_DFP\1.0.90\gcc\dev\atmega32u4"  
	@echo Finished building target: $@
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "findPuck_6_2.elf" "findPuck_6_2.hex"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "findPuck_6_2.elf" "findPuck_6_2.eep" || exit 0
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "findPuck_6_2.elf" > "findPuck_6_2.lss"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "findPuck_6_2.elf" "findPuck_6_2.srec"
	"D:\Penn Learning\MEAM510\Software\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "findPuck_6_2.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "findPuck_6_2.elf" "findPuck_6_2.a" "findPuck_6_2.hex" "findPuck_6_2.lss" "findPuck_6_2.eep" "findPuck_6_2.map" "findPuck_6_2.srec" "findPuck_6_2.usersignatures"
	