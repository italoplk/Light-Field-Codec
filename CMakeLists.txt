cmake_minimum_required(VERSION 3.10)
project(LF_Codec)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_FLAGS "-O3")

enable_testing()
add_subdirectory(extern/googletest)
add_subdirectory(tst)

set(SOURCES
    src/EncoderParameters.cpp
    src/EncoderParameters.h
    src/Point4D.cpp
    src/Point4D.h
    src/Transform.cpp
    src/Transform.h
    src/Quantization.cpp
    src/Quantization.h
    src/LightField.cpp
    src/LightField.h
    src/Statistics.cpp
    src/Statistics.h
    src/LRE.cpp
    src/LRE.h
    src/clip.h
    src/DpcmDC.cpp
    src/DpcmDC.h
    src/Time.cpp
    src/Time.h
    src/CAVLC.cpp
    src/CAVLC.h
    src/EncBitstreamWriter.cpp
    src/EncBitstreamWriter.h
    src/EncBitstreamVLC.cpp
    src/EncBitstreamVLC.h
    src/EncBitstreamBuffer.cpp
    src/EncBitstreamBuffer.h
    src/Prediction.cpp
    src/Prediction.h
    src/deprecated/OldTransform.cpp)



############### Project executables ###############
# Default configuration
add_executable(LF_Codec                 src/main.cpp ${SOURCES})
add_executable(LF_Codec_LUMA            src/main.cpp ${SOURCES})
add_executable(LF_Codec_QUANT           src/main.cpp ${SOURCES})
add_executable(LF_Codec_QUANT_LUMA      src/main.cpp ${SOURCES})
add_executable(LF_Codec_QUANT_SEG       src/main.cpp ${SOURCES})
add_executable(LF_Codec_QUANT_SEG_LUMA  src/main.cpp ${SOURCES})
add_executable(LF_Codec_REFERENCE       src/main.cpp ${SOURCES})
add_executable(LF_Codec_SEG             src/main.cpp ${SOURCES})
add_executable(LF_Codec_QUANT_SEG_4     src/main.cpp ${SOURCES})
add_executable(LF_Codec_SEG_4           src/main.cpp ${SOURCES})

add_library(LF_Codec_Lib ${SOURCES})

target_include_directories(LF_Codec PRIVATE src/sim/lfcodec)
target_include_directories(LF_Codec_Lib PRIVATE src/sim/reference)
target_include_directories(LF_Codec_LUMA PRIVATE src/sim/luma)
target_include_directories(LF_Codec_QUANT PRIVATE src/sim/quant)
target_include_directories(LF_Codec_QUANT_LUMA PRIVATE src/sim/quant_luma)
target_include_directories(LF_Codec_QUANT_SEG PRIVATE src/sim/quant_seg)
target_include_directories(LF_Codec_QUANT_SEG_LUMA PRIVATE src/sim/quant_seg_luma)
target_include_directories(LF_Codec_REFERENCE PRIVATE src/sim/reference)
target_include_directories(LF_Codec_SEG PRIVATE src/sim/seg)
target_include_directories(LF_Codec_QUANT_SEG_4 PRIVATE src/sim/quant_seg_4)
target_include_directories(LF_Codec_SEG_4 PRIVATE src/sim/seg_4)


