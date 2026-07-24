################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
app/%.obj: ../app/%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-c2000_22.6.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C" --include_path="C:/ti/c2000/C2000Ware_6_00_01_00" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/device" --include_path="C:/ti/c2000/C2000Ware_6_00_01_00/driverlib/f28004x/driverlib/" --include_path="C:/ti/ccs2031/ccs/tools/compiler/ti-cgt-c2000_22.6.2.LTS/include" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/hw/includes" --define=DEBUG --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="app/$(basename $(<F)).d_raw" --include_path="C:/Users/mkoda/workspace_ccstheia/LCD_I2C/CPU1_RAM/syscfg" --obj_directory="app" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


