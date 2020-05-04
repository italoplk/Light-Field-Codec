cmake_minimum_required(VERSION 3.15)


set(This lfcodec-tests)

set(Sources
    TransformContextTest.cpp
    OrthogonalContextMixinTest.cpp
    OldAPITest.cpp
    BlockTest.cpp
    BlockCompareTest.cpp
)

add_executable(${This} ${Sources})
target_include_directories(${This} PUBLIC
    ../src
)
target_link_libraries(${This} PUBLIC
    gtest_main
    lfcodeclib
    spdlog::spdlog
)

add_test(
    NAME ${This}
    COMMAND ${This}
)