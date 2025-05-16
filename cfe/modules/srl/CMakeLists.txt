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

find_library(GPIOD_LIB gpiod REQUIRED)
# Must enter the keyword `PRIVATE`
target_link_libraries(srl PRIVATE ${GPIOD_LIB})