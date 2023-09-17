

# Ref: https://www.reddit.com/r/embedded/comments/qkcfjt/cmake_setup_for_project_with_stm32_how_to_switch/
# Ref: https://gist.github.com/eugene-babichenko/3118042a23d2082bfba624f3f2039843
# Ref: https://stackoverflow.com/questions/36122358/visual-studio-code-c-include-path

cmake_minimum_required(VERSION 3.13)


# Tool Chain Setup
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-gcc-toolchain.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Target set to debug version.")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Target set to release.")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project Setup
project( model1 C CXX ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)



# Customed Source File Setting
set( startup_script ${CMAKE_SOURCE_DIR}/cfg/startup_stm32f411xe.S)

set( cmsis_script   ${CMAKE_SOURCE_DIR}/cfg/system_stm32f4xx.c)

set( config_include ${CMAKE_SOURCE_DIR}/cfg)
set( lvgl_include   ${CMAKE_SOURCE_DIR}/lib/lvgl)

set( config_define -DSTM32F411xE -DUSE_HAL_DRIVER)
set( lvgl_define -DLV_USE_DEV_VERSION)

set( LV_CONF_PATH   ${CMAKE_SOURCE_DIR}/cfg/lv_conf.h)
add_subdirectory( lib/lvgl              ${CMAKE_SOURCE_DIR}/build/lvgl/build)




# Select the native compile PORT
# set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
# Select the cross-compile PORT
# if (CMAKE_CROSSCOMPILING)
#     message("Compile in ISS mode")
#     set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
# endif()

message("Compile in ISS mode")
set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set( FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
set( freertos_include               ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/include
                                    ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/portable/GCC/ARM_CM4F)



# FetchContent_MakeAvailable(freertos_kernel)
# target_compile_definitions(freertos_config INTERFACE ${definitions})
# target_compile_options(freertos_config INTERFACE ${options})



set( cpu_params         -mcpu=cortex-m4
                        -mthumb
                        -mfpu=fpv4-sp-d16
                        -mfloat-abi=hard)

include_directories( BEFORE SYSTEM      ${CMAKE_SOURCE_DIR}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Include
                                        ${CMAKE_SOURCE_DIR}/lib/STM32CubeF4/Drivers/CMSIS/Core/Include
                                        ${CMAKE_SOURCE_DIR}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Inc
                                        ${config_include}
                                        ${lvgl_include}
                                        ${freertos_include}
                                        ${CMAKE_SOURCE_DIR}
                                        ${CMAKE_SOURCE_DIR}/app
                                        ${CMAKE_SOURCE_DIR}/app/component/time
                                        ${CMAKE_SOURCE_DIR}/app/component/util
                                        ${CMAKE_SOURCE_DIR}/bsp
                                        ${CMAKE_SOURCE_DIR}/cmn)

aux_source_directory( ${CMAKE_SOURCE_DIR}/app/component/util    source_app_util)
aux_source_directory( ${CMAKE_SOURCE_DIR}/app/component/time    source_app_time)
aux_source_directory( ${CMAKE_SOURCE_DIR}/app/component/trace   source_app_trace)
aux_source_directory( ${CMAKE_SOURCE_DIR}/bsp     source_bsp)
aux_source_directory( ${CMAKE_SOURCE_DIR}/cmn     source_cmn)
aux_source_directory( ${CMAKE_SOURCE_DIR}/lib/STM32CubeF4/Drivers/STM32F4xx_HAL_Driver/Src  source_stm32hal)


add_definitions( ${config_define} ${lvgl_define})
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)


add_executable( ${PROJECT_NAME}.elf                     ${CMAKE_SOURCE_DIR}/main.c
                                                        ${CMAKE_SOURCE_DIR}/main_dma.c 
                                                        ${startup_script}
                                                        ${cmsis_script}
                                                        ${source_app_util}
                                                        ${source_app_time}
                                                        ${source_app_trace}
                                                        ${source_cmn}
                                                        ${source_bsp}
                                                        ${source_stm32hal})

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE      ${config_define})

target_compile_options( ${PROJECT_NAME}.elf PRIVATE     ${cpu_params}
                                                        -fdata-sections
                                                        -ffunction-sections
                                                        -Wall
                                                        -Wno-pedantic
                                                        $<$<CONFIG:Debug>:-Og>)

target_link_options(${PROJECT_NAME}.elf PRIVATE          -T${CMAKE_SOURCE_DIR}/cfg/STM32F411CEUX_FLASH.ld
                                                        ${cpu_params}
                                                        -specs=nano.specs
                                                        -specs=nosys.specs
                                                        -lc
                                                        -lm
                                                        -lnosys
                                                        -Wl,-Map=${PROJECT_NAME}.map,--cref
                                                        -Wl,--gc-sections)



add_library(freertos_config INTERFACE)
add_subdirectory( lib/FreeRTOS-Kernel   ${CMAKE_SOURCE_DIR}/build/freertos/build)

target_compile_options( freertos_config     INTERFACE     ${cpu_params}
                                                        -fdata-sections
                                                        -ffunction-sections
                                                        -Wall
                                                        -Wno-pedantic
                                                        $<$<CONFIG:Debug>:-Og>)

target_include_directories(freertos_config SYSTEM
INTERFACE
    ${CMAKE_SOURCE_DIR}/cfg
)

target_compile_definitions(freertos_config
    INTERFACE
    projCOVERAGE_TEST=0
)
                                                        
target_link_libraries( ${PROJECT_NAME}.elf      freertos_kernel  
                                                lvgl)


set(HEX_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.hex)
set(BIN_FILE ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.bin)
add_custom_command(     TARGET  ${PROJECT_NAME}.elf POST_BUILD
                        COMMAND ${CMAKE_OBJCOPY} -Oihex   $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
                        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
                        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")



  