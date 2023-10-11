

file(GLOB_RECURSE APP_SRC_LIST CONFIGURE_DEPENDS    "${PRJ_TOP}/app/*.h" 
                                                    "${PRJ_TOP}/app/*.cpp" 
                                                    "${PRJ_TOP}/app/*.c" )


list( APPEND SRC_LIST ${APP_SRC_LIST})


set( APP_INC_DIR "")
GET_SUBDIR( APP_INC_DIR ${PRJ_TOP}/app)

list(APPEND INC_DIR_LIST ${PRJ_TOP}/app)
list(APPEND INC_DIR_LIST ${APP_INC_DIR})