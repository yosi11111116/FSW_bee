##################################################################
#
# cFE Serial Comm. (SRL) module CMake build recipe
#
# Author : HyeokJin Kweon
##################################################################

project (CFE_SRL C)

# Unit Test stub flags
set(ENABLE_UT TRUE)

# Serial Comm. source files
if (ENABLE_UT)
    set(srl_SOURCES
        fsw/src/cfe_srl_basic.c
        fsw/src/cfe_srl_io_stub.c
        fsw/src/cfe_srl_mutex.c
        fsw/src/cfe_srl_handle.c
        fsw/src/cfe_srl_priv.c
    )
else()
    set(srl_SOURCES
    fsw/src/cfe_srl_basic.c
    fsw/src/cfe_srl_io.c
    fsw/src/cfe_srl_mutex.c
    fsw/src/cfe_srl_handle.c
    fsw/src/cfe_srl_priv.c
    )   
endif()

add_library(srl STATIC ${srl_SOURCES})

target_include_directories(srl PUBLIC fsw/inc)
target_link_libraries(srl PRIVATE core_private)

message(STATUS "Processor = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "PROJECT_SOURCE_DIR: ${PROJECT_SOURCE_DIR}")


message(STATUS "GPIOD LIB: ${PROJECT_SOURCE_DIR}/../../../submodules/libgpiod/obc/lib/libgpiod.a")
message(STATUS "GPIOD INCLUDE: ${PROJECT_SOURCE_DIR}/../../../submodules/libgpiod/obc/include")

if (CMAKE_SYSTEM_PROCESSOR	STREQUAL "arm")
    # For OBC (cross-compiled libgpiod)
    target_include_directories(srl PRIVATE ${PROJECT_SOURCE_DIR}/../../../submodules/libgpiod/obc/include)
    target_link_libraries(srl PRIVATE ${PROJECT_SOURCE_DIR}/../../../submodules/libgpiod/obc/lib/libgpiod.a)
else ()
    # For local Linux host
    # link libgpiod.so -> drop this
    #find_library(GPIOD_LIB gpiod REQUIRED)
    # Must enter the keyword `PRIVATE`
    #target_link_libraries(srl PRIVATE ${GPIOD_LIB})

    # link libgpiod.a
    target_include_directories(srl PRIVATE /usr/include)
    target_link_libraries(srl PRIVATE /usr/lib/x86_64-linux-gnu/libgpiod.a)
endif()