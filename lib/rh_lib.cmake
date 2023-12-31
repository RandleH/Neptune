
cmake_minimum_required(VERSION 3.14)
include(FetchContent)

MACRO( CHECK_LIB_INTEGRITY lib_dir result)
    if( EXISTS "${lib_dir}" )
        execute_process(
            COMMAND
                git -C ${lib_dir} fsck
            RESULT_VARIABLE
                RES
        )
        message( "- Program returned value ${RES}" )
        if( (NOT EXISTS "${lib_dir}/.git") OR (NOT ${RES} EQUAL 0))
            set( ${result} FALSE)
        else()
            set( ${result} TRUE)
        endif()
    else()
        set( ${result} FALSE)
    endif()
ENDMACRO()





#########################################################################################################
# Library: LVGL
# Website: 
# GitHub: https://github.com/lvgl/lvgl.git
#########################################################################################################
message( STATUS "Check library integrity @ ${PRJ_TOP}/lib/lvgl ..." )
SET( LIB_DIR__LVGL  ${PRJ_TOP}/lib/lvgl)
CHECK_LIB_INTEGRITY( ${LIB_DIR__LVGL} result)
if( ${result} STREQUAL FALSE)
    FetchContent_Declare(   lib_lvgl
                            GIT_REPOSITORY https://github.com/lvgl/lvgl.git
                            GIT_TAG "release/v8.3"
                            SOURCE_DIR ${LIB_DIR__LVGL})
    FetchContent_MakeAvailable(lib_lvgl)                        
endif()


set( LV_CONF_BUILD_DISABLE_EXAMPLES  TRUE)
set( DLV_CONF_BUILD_DISABLE_DEMOS  TRUE)
set( LVGL_MISC_DEFINE       -DLV_CONF_BUILD_DISABLE_EXAMPLES=1
                            -DLV_CONF_BUILD_DISABLE_DEMOS=1
                            -DLV_CONF_PATH=${CMAKE_SOURCE_DIR}/cfg/lv_conf.h
                            )

add_definitions( ${LVGL_MISC_DEFINE})
add_subdirectory( ${LIB_DIR__LVGL} ${PRJ_TOP}/build/lvgl/build )

target_include_directories(lvgl INTERFACE ${PRJ_TOP}/cfg ${PRJ_TOP}/lib/FreeRTOS-Kernel/include)
target_compile_options(lvgl  INTERFACE   ${CMAKE_CXX_FLAGS} ${CPU_FLAG} ${CXX_FLAG} )
target_link_options(lvgl INTERFACE ${LINK_FLAG} ${CPU_FLAG})

# lvgl_demos
# lvgl_example


list( APPEND INC_DIR_LIST   ${LIB_DIR__LVGL})
list( APPEND TARGET_LIST    lvgl)

message( STATUS "LVGL library loaded")


#########################################################################################################
# Library: FreeRTOS-Kernel
# Website: https://www.FreeRTOS.org
# GitHub: https://github.com/FreeRTOS/FreeRTOS-Kernel.git
#########################################################################################################
SET( LIB_DIR__FREERTOS  ${PRJ_TOP}/lib/FreeRTOS-Kernel)
message( STATUS "Check library integrity @ ${LIB_DIR__FREERTOS} ..." )
CHECK_LIB_INTEGRITY( ${LIB_DIR__FREERTOS} result)
if( ${result} STREQUAL FALSE)
    FetchContent_Declare(   freertos_kernel
                            GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
                            GIT_TAG        main #Note: Best practice to use specific git-hash or tagged version 
                            SOURCE_DIR ${LIB_DIR__FREERTOS})
    FetchContent_MakeAvailable(freertos_kernel)
endif()

if (CMAKE_CROSSCOMPILING)
    set( FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
else()
    set( FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
endif()
# message( ${FREERTOS_PORT})

set( FREERTOS_HEAP "4" CACHE STRING "" FORCE)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${PRJ_TOP}/cfg)
target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)
target_compile_options(freertos_config  INTERFACE   ${CMAKE_CXX_FLAGS} ${CPU_FLAG} ${CXX_FLAG} )
target_link_options(freertos_config INTERFACE ${LINK_FLAG} ${CPU_FLAG})


add_subdirectory( ${LIB_DIR__FREERTOS}   ${PRJ_TOP}/build/freertos/build)



list( APPEND INC_DIR_LIST           ${LIB_DIR__FREERTOS}/include
                                    ${LIB_DIR__FREERTOS}/portable/GCC/ARM_CM4F)
list( APPEND TARGET_LIST            freertos_kernel)

message( STATUS "FreeRTOS-Kernel library loaded")


#########################################################################################################
# Library: STM32 HAL
# Website: https://www.st.com
# GitHub: https://github.com/STMicroelectronics/STM32CubeF4.git
#########################################################################################################
SET( LIB_DIR__STM32HAL  ${PRJ_TOP}/lib/STM32CubeF4)
CHECK_LIB_INTEGRITY( ${LIB_DIR__STM32HAL} result)
if( ${result} STREQUAL FALSE)
    FetchContent_Declare(   stm32hal
                            GIT_REPOSITORY https://github.com/STMicroelectronics/STM32CubeF4.git
                            GIT_TAG        master
                            SOURCE_DIR ${LIB_DIR__STM32HAL})
    FetchContent_MakeAvailable(stm32hal)
endif()


set( STM32HAL_MISC_DEFINE   -DSTM32F411xE 
                            -DUSE_HAL_DRIVER )

add_definitions( ${STM32HAL_MISC_DEFINE})
aux_source_directory( ${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Src  SRC_STM32)


list( APPEND        SRC_LIST                ${SRC_STM32})
                                                                 
list( REMOVE_ITEM   SRC_LIST                "${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_tim_template.c"
                                            "${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_msp_template.c"
                                            "${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_alarm_template.c"
                                            "${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_timebase_rtc_wakeup_template.c" 
                                            )


# message( "RANDLEH: ${SRC_LIST}")
list( APPEND        INC_DIR_LIST            ${LIB_DIR__STM32HAL}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
                                            ${LIB_DIR__STM32HAL}/Drivers/CMSIS/Core/Include
                                            ${LIB_DIR__STM32HAL}/Drivers/STM32F4xx_HAL_Driver/Inc )
                                            
