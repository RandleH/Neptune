

file(GLOB_RECURSE CMN_SRC_LIST CONFIGURE_DEPENDS    "${PRJ_TOP}/cmn/*.h" 
                                                    "${PRJ_TOP}/cmn/*.cpp" 
                                                    "${PRJ_TOP}/cmn/*.c" )
                                                    
list(APPEND SRC_LIST ${CMN_SRC_LIST})

set( CMN_INC_DIR "")
GET_SUBDIR( CMN_INC_DIR ${PRJ_TOP}/cmn)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/cmn)
list(APPEND INC_DIR_LIST ${CMN_INC_DIR})

