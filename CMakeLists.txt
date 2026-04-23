set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
cmake_minimum_required(VERSION 3.20)

# ที่อยู่ของ riscv gcc
set(TOOLPATH  D:/RISCVEmbeddedGCC/bin/riscv-none-embed-)

if (WIN32)
	MESSAGE(STATUS "Now is windows!")
    set(CMAKE_C_COMPILER ${TOOLPATH}gcc.exe)
    set(CMAKE_CXX_COMPILER ${TOOLPATH}g++.exe)
    set(CMAKE_ASM_COMPILER ${TOOLPATH}gcc.exe)
    set(CMAKE_AR ${TOOLPATH}ar.exe)
    set(CMAKE_OBJCOPY ${TOOLPATH}objcopy.exe)
    set(CMAKE_OBJDUMP ${TOOLPATH}objdump.exe)
    set(SIZE ${TOOLPATH}size.exe)
elseif (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS!")
    set(CMAKE_C_COMPILER ${TOOLPATH}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLPATH}g++)
    set(CMAKE_ASM_COMPILER ${TOOLPATH}gcc)
    set(CMAKE_AR ${TOOLPATH}ar)
    set(CMAKE_OBJCOPY ${TOOLPATH}objcopy)
    set(CMAKE_OBJDUMP ${TOOLPATH}objdump)
    set(SIZE ${TOOLPATH}size)
else ()
    MESSAGE(STATUS "Unsupported system!")
endif ()

# ตั้งค่าเวอร์ชันของ cmake
project(ch32v307-ninja C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(EXECUTABLE ${PROJECT_NAME}.elf)

# ตั้ง compile flag
add_compile_options(-march=rv32imafc -mabi=ilp32f -mcmodel=medany -msmall-data-limit=8 -mno-save-restore)
add_compile_options(-fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -fsingle-precision-constant)

# optimization
add_compile_options(-Os)

# warn all
# add_compile_options(-Wall)

# define debug
# add_definitions(-DDEBUG=1)

# add inc
include_directories(APP
                    Core
                    Debug 
                    Peripheral/inc)

# add src
file(GLOB_RECURSE SOURCES 
                "APP/*.c"
                "Core/core_riscv.c" 
                "Debug/debug.c"
                "Peripheral/src/*.c" 
                "Startup/startup_ch32v30x_D8C.S"
                )

# ตั้งค่า linker
set(LINKER_SCRIPT  ${CMAKE_SOURCE_DIR}/Ld/Link.ld)
add_link_options(
                -march=rv32imafc -mabi=ilp32f
	            -nostartfiles 
                -Xlinker --gc-sections  
                -Wl,--print-memory-usage
                -Wl,-Map,${PROJECT_NAME}.map 
                --specs=nano.specs 
                --specs=nosys.specs)
add_link_options(-T ${LINKER_SCRIPT})

# รวมทุกไฟล๋ใน executable
add_executable(${EXECUTABLE}
  ${SOURCES}
  ${KEYBOARD_SOURCES}
  ${LINKER_SCRIPT}
)


set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
file(MAKE_DIRECTORY "${OUTPUT_DIR}")

set(FINAL_NAME "output")
message(STATUS "Final firmware name: ${FINAL_NAME}")

# compile ออกเป็น .hex .bin .lst
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.lst)
add_custom_command(TARGET ${EXECUTABLE} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
        COMMAND ${CMAKE_OBJDUMP} --all-headers --demangle --disassemble $<TARGET_FILE:${PROJECT_NAME}.elf> > ${LST_FILE}
        COMMAND ${SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${HEX_FILE}
                ${OUTPUT_DIR}/${FINAL_NAME}.hex
				
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${BIN_FILE}
                ${OUTPUT_DIR}/${FINAL_NAME}.bin
)
