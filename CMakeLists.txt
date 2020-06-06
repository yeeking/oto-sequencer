cmake_minimum_required (VERSION 3.10)

# Define the project
project(oto-sequencer C CXX)

# this adds all h and cpp files from lib folder to the 
# build
# apparently you're not supposed to do this after Cmake 3.12 
# but it works 
file(GLOB LIB_DIR
    "./lib/*.h"
    "./lib/*.cpp"
)

# this adds the pthreads library (g++ -pthread)
FIND_PACKAGE ( Threads REQUIRED )

# build the rapidlib library code
add_library(libs ${LIB_DIR})

# build the executable
add_executable(oto-sequencer src/Main.cpp)
# link the main exutable to the rapidlib library and pthreads
target_link_libraries(oto-sequencer libs ${CMAKE_THREAD_LIBS_INIT})
