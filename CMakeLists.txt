cmake_minimum_required(VERSION 3.15)

set(This lfcodec)

project(${This} C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS "-O3")


enable_testing()
add_subdirectory(extern/googletest)
add_subdirectory(extern/spdlog)
add_subdirectory(tst)

# List files to library
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
                src/Prediction.cpp
                src/Prediction.h

                # Deprecated Classes
                src/deprecated/Transform.h
                src/deprecated/Transform.cpp
)


# Project executable
add_executable(${This} src/main.cpp)

# Add sources directory to project executable
target_include_directories(${This} PUBLIC src)

# Link library to project binary
target_link_libraries(${This} PUBLIC ${This}lib)

# Link spdlog to project library
target_link_libraries(${This}lib PUBLIC spdlog)
