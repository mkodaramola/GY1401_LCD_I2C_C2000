################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-2081829713: ../c2000.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2031/ccs/utils/sysconfig_1.25.0/sysconfig_cli.bat" --script "C:/Users/mkoda/workspace_ccstheia/LCD_I2C/c2000.syscfg" -o "syscfg" -s "C:/ti/c2000/C2000Ware_6_00_01_00/.metadata/sdk.json" -d "F28004x" -p "F28004x_100PZ" -r "F28004x_100PZ" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-2081829713 ../c2000.syscfg
syscfg/board.h: build-2081829713
syscfg/board.cmd.genlibs: build-2081829713
syscfg/board.opt: build-2081829713
syscfg/board.json: build-2081829713
syscfg/pinmux.csv: build-2081829713
syscfg/c2000ware_libraries.cmd.genlibs: build-2081829713
syscfg/c2000ware_libraries.opt: build-2081829713
syscfg/c2000ware_libraries.c: build-2081829713
syscfg/c2000ware_libraries.h: build-2081829713
syscfg/clocktree.h: build-2081829713
syscfg: build-2081829713

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-c2000_22.6.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C" --include_path="C:/ti/c2000/C2000Ware_6_00_01_00" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/device" --include_path="C:/ti/c2000/C2000Ware_6_00_01_00/driverlib/f28004x/driverlib/" --include_path="C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-c2000_22.6.2.LTS/include" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/includes" --define=DEBUG --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


