

# Ref: https://www.reddit.com/r/embedded/comments/qkcfjt/cmake_setup_for_project_with_stm32_how_to_switch/


cmake_minimum_required(VERSION 3.13)


# Tool Chain Setup
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-gcc-toolchain.cmake)


# Project Setup
project( model1 C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# set( FREERTOS_CONFIG_FILE_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/cfg")

# add_subdirectory( lib/lvgl                 ${OUTPUTDIR}/lvgl/build)
# add_subdirectory( lib/FreeRTOS-Kernel      ${OUTPUTDIR}/FreeRTOS-Kernel/build)

# add_subdirectory(app)
# aux_source_directory( app source_app)

set( startup_script ${CMAKE_SOURCE_DIR}/lib/STM32CubeF4/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f411xe.s)

set( cpu_params         -mcpu=cortex-m4
                        -mthumb
                        -mfpu=fpv4-sp-d16
                        -mfloat-abi=hard)




add_executable( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/main.c ${startup_script})

target_compile_definitions(${PROJECT_NAME} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F413xx
        )

target_compile_options( ${PROJECT_NAME} PRIVATE
        ${cpu_params}

        -fdata-sections
        -ffunction-sections
        -Wall
        $<$<CONFIG:Debug>:-Og>
        )

target_link_options(${PROJECT_NAME} PRIVATE
        -T${CMAKE_SOURCE_DIR}/cfg/STM32F411CEUX_FLASH.ld
        ${cpu_params}
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

# target_link_directories( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel/build)
# target_link_libraries( ${PROJECT_NAME} freertos_kernel)

  