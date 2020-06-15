cmake_minimum_required(VERSION 3.15)

add_subdirectory(axis_flip_tests)
add_subdirectory(block_compare_tests)
add_subdirectory(block_segmentation_tests)
add_subdirectory(block_tests)
add_subdirectory(old_api_comparison)

# set(This lfcodec-tests)

# set(Sources
#     OldAPITest.cpp
#     BlockSegmentation.cpp
#     AxisFlipTest.cpp
# )

# add_executable(${This} ${Sources})
# target_include_directories(${This} PUBLIC
#     ../src
# )
# target_link_libraries(${This} PUBLIC
#     gtest_main
#     lfcodec
# )

# add_test(
#     NAME ${This}
#     COMMAND ${This}
# )