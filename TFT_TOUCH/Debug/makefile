################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := c:/ti/ccsv6/tools/compiler/arm_5.1.6

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./TFT_Touch.obj" \
"./tm4c1294ncpdt_startup_ccs.obj" \
"./uartstdio.obj" \
"./TOUCHinit/TOUCH_TSC2046.obj" \
"./TFTinit/TFT_400x240_OTM4001A_16bit.obj" \
"./EPIinit/EPIinit.obj" \
"../tm4c1294ncpdt.cmd" \
-l"libc.a" \
-l"C:\ti\TivaWare_C_Series-2.1.0.12573\driverlib\ccs\Debug\driverlib.lib" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include TOUCHinit/subdir_vars.mk
-include TFTinit/subdir_vars.mk
-include EPIinit/subdir_vars.mk
-include subdir_rules.mk
-include TOUCHinit/subdir_rules.mk
-include TFTinit/subdir_rules.mk
-include EPIinit/subdir_rules.mk
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
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
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
TFT_TOUCH.out \

EXE_OUTPUTS__QUOTED += \
"TFT_TOUCH.out" \

BIN_OUTPUTS += \
TFT_TOUCH.hex \

BIN_OUTPUTS__QUOTED += \
"TFT_TOUCH.hex" \


# All Target
all: TFT_TOUCH.out

# Tool invocations
TFT_TOUCH.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 --abi=eabi -me -g --gcc --define=ccs="ccs" --define=PART_TM4C1294NCPDT --display_error_number --diag_warning=225 --diag_wrap=off -z -m"TFT_TOUCH.map" --heap_size=0 --stack_size=512 -i"c:/ti/ccsv6/tools/compiler/arm_5.1.6/lib" -i"C:/ti/TivaWare_C_Series-2.1.0.12573/driverlib" -i"c:/ti/ccsv6/tools/compiler/arm_5.1.6/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="TFT_TOUCH_linkInfo.xml" --rom_model -o "TFT_TOUCH.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

TFT_TOUCH.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"c:/ti/ccsv6/tools/compiler/arm_5.1.6/bin/armhex"  -o "TFT_TOUCH.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)$(BIN_OUTPUTS__QUOTED)
	-$(RM) "TFT_Touch.pp" "tm4c1294ncpdt_startup_ccs.pp" "uartstdio.pp" "TOUCHinit\TOUCH_TSC2046.pp" "TFTinit\TFT_400x240_OTM4001A_16bit.pp" "EPIinit\EPIinit.pp" 
	-$(RM) "TFT_Touch.obj" "tm4c1294ncpdt_startup_ccs.obj" "uartstdio.obj" "TOUCHinit\TOUCH_TSC2046.obj" "TFTinit\TFT_400x240_OTM4001A_16bit.obj" "EPIinit\EPIinit.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

