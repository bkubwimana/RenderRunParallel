# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(OpenMPProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OPENAL_LIBRARY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/libs-msvc/x64/openal32.lib)

# Add source files
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/code/*.cpp)
# Add header files
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/code/*.h)

# Add the executable
add_executable(Lab2 ${SOURCES} ${HEADERS})

#add header only add_library
add_library(clipp INTERFACE)
# SYSTEM keyword prevents warnings from external libraries
target_include_directories(clipp SYSTEM INTERFACE "${PROJECT_SOURCE_DIR}/third-party/")


# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/../SFML/include
)

link_directories(
    ${PROJECT_SOURCE_DIR}/lib
    ${PROJECT_SOURCE_DIR}/../SFML/lib
)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Link the executable to the libraries in the lib directory
target_link_libraries(Lab2 PRIVATE clipp sfml-graphics sfml-system sfml-window ${OPENAL_LIBRARY})

# Link OpenMP libraries
if(OpenMP_CXX_FOUND)
    target_link_libraries(Lab2 PUBLIC OpenMP::OpenMP_CXX)
endif()

set_target_properties(
    Lab2 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)

# Copy DLLs using file(COPY ...)
file(COPY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/bin/x64/openal32.dll
     DESTINATION "${COMMON_OUTPUT_DIR}/bin")

