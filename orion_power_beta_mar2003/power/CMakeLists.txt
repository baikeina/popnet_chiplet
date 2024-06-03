#功率库的编译脚本

#参考
#https://blog.csdn.net/qq_34796146/article/details/108877159
#https://qa.1r1g.com/sf/ask/706016181/

cmake_minimum_required(VERSION 3.0)

project(orion_power)

set(DEFS -DPOWER_TEST -DPARM_TECH_POINT=18 -DVdd=1.8 -DPARM_Freq=1e9)
set(CFLAGS -I. -I../library ${DEFS} -Wno-deprecated -O3)
set(AR ar)
set(RANLIB ranlib)

include_directories(../library)
include_directories(.)
set(CMAKE_C_FLAGS "-DPOWER_TEST -DPARM_TECH_POINT=18 -DVdd=1.8 -DPARM_Freq=1e9 -Wno-deprecated")

set(ARRAY_MODEL SIM_power_array_l.c SIM_power_array_m.c SIM_power_cam.c)
set(ROUTER_MODEL SIM_power_router.c SIM_router_power.c)
set(ALU_MODEL SIM_power_ALU.c)
set(OTHER_MODEL SIM_power_misc.c SIM_power_permu.c)
set(MISC_SRC SIM_power_util.c SIM_power_time.c)
set(SRCS ${ARRAY_MODEL} ${ROUTER_MODEL} ${ALU_MODEL} ${OTHER_MODEL} ${MISC_SRC})
set(HEADERS SIM_power.h SIM_power_ALU.h SIM_power_array.h SIM_power_array_internal.h SIM_power_misc.h SIM_power_misc_internal.h SIM_power_permu.h SIM_power_router.h SIM_power_router_internal.h SIM_router_power.h)

set(LIB libpower.a)

set(CMAKE_C_STANDARD 11)
#set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_BUILD_TYPE RELEASE)

add_library(orion_power STATIC ${SRCS})
