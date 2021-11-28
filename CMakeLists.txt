cmake_minimum_required (VERSION 3.00)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wno-psabi")

# enable gprof
#SET(GCC_COVERAGE_LINK_FLAGS    "-pg")
#SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")

# Define the project
project(oto-sequencer C CXX)

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})


# this adds all h and cpp files from lib folder to the 
# build
# apparently you're not supposed to do this after Cmake 3.12 
# but it works 
file(GLOB ML_LIB_DIR
    "./lib/ml/*.h"
    "./lib/ml/*.cpp"
)
file(GLOB GROVE_LIB_DIR
    "./lib/grove/*.h"
    "./lib/grove/*.cpp"
)


# this adds the pthreads library (g++ -pthread)
FIND_PACKAGE ( Threads REQUIRED )

# build the rapidlib library code
add_library(ml-libs ${ML_LIB_DIR})
add_library(grove-libs ${GROVE_LIB_DIR})

# build the sequencer library
add_library(seq-lib src/Sequencer.cpp)
# build the midi utils
add_library(midi-lib src/MidiUtils.cpp)
# build the seq utils
add_library(sequtil-lib src/SequencerUtils.cpp)

# build the executable
# for normal linux without i2c stuff
add_executable(oto-sequencer src/Main.cpp)
# unit tests
add_executable(testit src/unit_tests.cpp)
# raspi version with i2c
add_executable(oto-sequencer-pi src/MainPi.cpp)

# link the main executable to the rapidlib library and pthreads
target_link_libraries(oto-sequencer seq-lib sequtil-lib midi-lib  -lrtmidi ml-libs ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(testit seq-lib sequtil-lib midi-lib ml-libs ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(oto-sequencer-pi sequtil-lib seq-lib midi-lib ml-libs grove-libs ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(oto-sequencer-pi seq-lib sequtil-lib midi-lib  -lrtmidi ml-libs ${CMAKE_THREAD_LIBS_INIT})

