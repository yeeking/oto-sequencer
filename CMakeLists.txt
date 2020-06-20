SET(GCC_COVERAGE_COMPILE_FLAGS "-Wno-psabi")

cmake_minimum_required (VERSION 3.00)

# Define the project
project(oto-sequencer C CXX)

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

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
add_executable(testit src/unit_tests.cpp)

# link the main executable to the rapidlib library and pthreads
target_link_libraries(oto-sequencer libs ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(testit libs ${CMAKE_THREAD_LIBS_INIT})

