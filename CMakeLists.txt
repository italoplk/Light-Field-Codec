cmake_minimum_required(VERSION 3.15)

set(This lfcodec)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
#set(CMAKE_CXX_FLAGS "-O3")


enable_testing()
add_subdirectory(googletest)
add_subdirectory(spdlog)


add_library(${This}lib STATIC
                src/EncoderParameters.cpp
                src/EncoderParameters.h
                src/Point4D.cpp
                src/Point4D.h
                src/Transform.cpp
                src/Transform.h
                src/Quantization.cpp
                src/Quantization.h
                src/Typedef.h
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
)



add_executable(${This}
                src/main.cpp
                src/EncoderParameters.cpp
                src/EncoderParameters.h
                src/Point4D.cpp
                src/Point4D.h
                src/Transform.cpp
                src/Transform.h
                src/Quantization.cpp
                src/Quantization.h
                src/Typedef.h
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
)
target_link_libraries(${This} PRIVATE spdlog::spdlog)
add_subdirectory(tst)