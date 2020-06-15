cmake_minimum_required(VERSION 3.10)
project(LF_Codec)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-O3")
# Use the syntax $ENV{VAR} to read environment variable VAR. See also the set() command to set ENV{VAR}.
# add_compile_definitions(OPENCV_VERSION=${OpenCV_VERSION})
message(STATUS "TRANSF_QUANT: $ENV{TRANSF_QUANT}")
message(STATUS "QUANTIZATION: $ENV{QUANTIZATION}")
message(STATUS "DPCM_DC: $ENV{DPCM_DC}")
message(STATUS "STATISTICS_LOCAL: $ENV{STATISTICS_LOCAL}")
message(STATUS "STATISTICS_GLOBAL: $ENV{STATISTICS_GLOBAL}")
message(STATUS "STATISTICS_TIME: $ENV{STATISTICS_TIME}")
message(STATUS "TRACE_TRANSF: $ENV{TRACE_TRANSF}")
message(STATUS "TRACE_QUANT: $ENV{TRACE_QUANT}")
message(STATUS "TRACE_LRE: $ENV{TRACE_LRE}")
message(STATUS "LFCODEC_USE_PREDICTION: $ENV{LFCODEC_USE_PREDICTION}")
message(STATUS "LFCODEC_FORCE_DCT_NON_LUMA: $ENV{LFCODEC_FORCE_DCT_NON_LUMA}")
message(STATUS "LFCODEC_TRANSFORM_HISTOGRAM: $ENV{LFCODEC_TRANSFORM_HISTOGRAM}")

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

enable_testing()
add_subdirectory(extern/googletest)
add_subdirectory(test)

set(SOURCES
    src/EncoderParameters.cpp
    src/Point4D.cpp
    src/Transform.cpp
    src/Quantization.cpp
    src/LightField.cpp
    src/Statistics.cpp
    src/LRE.cpp
    src/DpcmDC.cpp
    src/Time.cpp
    src/CAVLC.cpp
    src/EncBitstreamWriter.cpp
    src/EncBitstreamVLC.cpp
    src/EncBitstreamBuffer.cpp
    src/Prediction.cpp
    src/deprecated/OldTransform.cpp)


add_executable(LF_Codec src/main.cpp ${SOURCES})
add_library(lfcodec ${SOURCES})

target_include_directories(LF_Codec PRIVATE src)
target_include_directories(lfcodec PRIVATE src)



