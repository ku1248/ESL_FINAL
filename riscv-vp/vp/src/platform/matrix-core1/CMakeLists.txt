file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_library(platform-matrix-core1
ethernet.cpp
display.cpp
${HEADERS})

target_include_directories(platform-matrix-core1 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(riscv-vp-matrix-core1
        main.cpp)

target_link_libraries(riscv-vp-matrix-core1 rv32 platform-matrix-core1 platform-common gdb-mc ${Boost_LIBRARIES} ${SystemC_LIBRARIES} pthread)

INSTALL(TARGETS riscv-vp RUNTIME DESTINATION bin)
