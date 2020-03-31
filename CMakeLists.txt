cmake_minimum_required(VERSION 3.15)
project(LF_Codec)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-O3")

add_executable(LF_Codec src/main.cpp src/EncoderParameters.cpp src/EncoderParameters.h src/Point4D.cpp src/Point4D.h src/Transform.cpp src/Transform.h src/Quantization.cpp src/Quantization.h src/Typedef.h src/LightField.cpp src/LightField.h src/Statistics.cpp src/Statistics.h src/LRE.cpp src/LRE.h src/clip.h src/DpcmDC.cpp src/DpcmDC.h src/Time.cpp src/Time.h src/CAVLC.cpp src/CAVLC.h src/EncBitstreamWriter.cpp src/EncBitstreamWriter.h src/EncBitstreamVLC.cpp src/EncBitstreamVLC.h src/EncBitstreamBuffer.cpp src/EncBitstreamBuffer.h src/Node.h src/Node.cpp)