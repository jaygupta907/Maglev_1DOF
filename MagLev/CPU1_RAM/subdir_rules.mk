################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --fp_mode=relaxed --include_path="C:/Users/jaygu/Desktop/Avishkar Hyperloop 7.0/MagLev" --include_path="C:/Users/jaygu/Desktop/Avishkar Hyperloop 7.0/MagLev/device" --include_path="C:/ti/C2000Ware_4_03_00_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/C2000Ware_4_03_00_00/libraries/calibration/hrpwm/F2837xD/include" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/include" --define=_LAUNCHXL_F28379D --define=DEBUG --define=CPU1 --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/jaygu/Desktop/Avishkar Hyperloop 7.0/MagLev/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

