################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./console.obj" \
"./lcd12864.obj" \
"./main.obj" \
"./motion.obj" \
"./parser.obj" \
"./planner.obj" \
"./queue.obj" \
"./spindle.obj" \
"./stepper.obj" \
"./system.obj" \
"./tm4c123gh6pm_startup_ccs.obj" \
"./tokenizer.obj" \
"./uartstdio.obj" \
"C:/ti/TivaWare_C_Series-2.1.2.111/driverlib/ccs/Debug/driverlib.lib" \
"../tm4c123gh6pm.cmd" \
$(GEN_CMDS__FLAG) \
-l"libc.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
Stepper_test.out \

EXE_OUTPUTS__QUOTED += \
"Stepper_test.out" \

BIN_OUTPUTS += \
Stepper_test.hex \

BIN_OUTPUTS__QUOTED += \
"Stepper_test.hex" \


# All Target
all: Stepper_test.out

# Tool invocations
Stepper_test.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number -z -m"Stepper_test.map" --heap_size=0 --stack_size=2048 -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/lib" -i"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/include" --reread_libs --display_error_number --diag_wrap=off --warn_sections --xml_link_info="Stepper_test_linkInfo.xml" --rom_model -o "Stepper_test.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Stepper_test.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.5/bin/armhex"  -o "Stepper_test.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "console.pp" "lcd12864.pp" "main.pp" "motion.pp" "parser.pp" "planner.pp" "queue.pp" "spindle.pp" "stepper.pp" "system.pp" "tm4c123gh6pm_startup_ccs.pp" "tokenizer.pp" "uartstdio.pp" 
	-$(RM) "console.obj" "lcd12864.obj" "main.obj" "motion.obj" "parser.obj" "planner.obj" "queue.obj" "spindle.obj" "stepper.obj" "system.obj" "tm4c123gh6pm_startup_ccs.obj" "tokenizer.obj" "uartstdio.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

