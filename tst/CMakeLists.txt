cmake_minimum_required(VERSION 3.15)


set(This lfcodec-tests)

set(Sources
    OldAPITest.cpp
    BlockSegmentation.cpp
    AxisFlipTest.cpp
)

add_executable(${This} ${Sources})
target_include_directories(${This} PUBLIC
    ../src
    ../src/sim/lfcodec
)
target_link_libraries(${This} PUBLIC
    gtest_main
    LF_Codec_Lib
)

add_test(
    NAME ${This}
    COMMAND ${This}
)