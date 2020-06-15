cmake_minimum_required(VERSION 3.10)
project(LF_Codec)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-O3")

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
add_subdirectory(tst)

set(SOURCES
    # src/EncoderParameters.h
    # src/Point4D.h
    # src/Transform.h
    # src/Quantization.h
    # src/LightField.h
    # src/Statistics.h
    # src/LRE.h
    # src/clip.h
    # src/DpcmDC.h
    # src/Time.h
    # src/CAVLC.h
    # src/EncBitstreamWriter.h
    # src/EncBitstreamVLC.h
    # src/EncBitstreamBuffer.h
    # src/Prediction.h
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



############### Project executables ###############
# Default configuration
add_executable(LF_Codec                 src/main.cpp ${SOURCES})
# add_executable(LF_Codec_LUMA            src/main.cpp ${SOURCES})
# add_executable(LF_Codec_QUANT           src/main.cpp ${SOURCES})
# add_executable(LF_Codec_QUANT_LUMA      src/main.cpp ${SOURCES})
# add_executable(LF_Codec_QUANT_SEG       src/main.cpp ${SOURCES})
# add_executable(LF_Codec_QUANT_SEG_LUMA  src/main.cpp ${SOURCES})
# add_executable(LF_Codec_REFERENCE       src/main.cpp ${SOURCES})
# add_executable(LF_Codec_SEG             src/main.cpp ${SOURCES})
# add_executable(LF_Codec_QUANT_SEG_4     src/main.cpp ${SOURCES})
# add_executable(LF_Codec_SEG_4           src/main.cpp ${SOURCES})

add_library(LF_Codec_Lib ${SOURCES})

target_include_directories(LF_Codec PRIVATE src)
target_include_directories(LF_Codec_Lib PRIVATE src)
# target_include_directories(LF_Codec_LUMA PRIVATE src/sim/luma)
# target_include_directories(LF_Codec_QUANT PRIVATE src/sim/quant)
# target_include_directories(LF_Codec_QUANT_LUMA PRIVATE src/sim/quant_luma)
# target_include_directories(LF_Codec_QUANT_SEG PRIVATE src/sim/quant_seg)
# target_include_directories(LF_Codec_QUANT_SEG_LUMA PRIVATE src/sim/quant_seg_luma)
# target_include_directories(LF_Codec_REFERENCE PRIVATE src/sim/reference)
# target_include_directories(LF_Codec_SEG PRIVATE src/sim/seg)
# target_include_directories(LF_Codec_QUANT_SEG_4 PRIVATE src/sim/quant_seg_4)
# target_include_directories(LF_Codec_SEG_4 PRIVATE src/sim/seg_4)


