

file(GLOB_RECURSE BSP_SRC_LIST CONFIGURE_DEPENDS    "${PRJ_TOP}/bsp/*.h" 
                                                    "${PRJ_TOP}/bsp/*.cpp" 
                                                    "${PRJ_TOP}/bsp/*.c" )
                                                    

list( APPEND SRC_LIST ${BSP_SRC_LIST})


set( BSP_INC_DIR "")
GET_SUBDIR( BSP_INC_DIR ${PRJ_TOP}/bsp)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/bsp)
list(APPEND INC_DIR_LIST ${BSP_INC_DIR})