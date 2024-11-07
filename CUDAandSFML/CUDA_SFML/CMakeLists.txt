# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the required CUDA version (optional)
set(CMAKE_CUDA_STANDARD 11)

# Project name
project(cuda_sfml_app LANGUAGES CXX CUDA)

# Include SFML
include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Set CUDA architecture
#set(CUDA_ARCHITECTURES 52)

# Specify the target executable
add_executable(cuda_sfml_app src/main.cpp src/cuda_kernels.cu src/cli_parser.cpp)

#add header only add_library
add_library(clipp INTERFACE)
# SYSTEM keyword prevents warnings from external libraries
target_include_directories(clipp SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/third-party)

# Link SFML libraries
target_link_libraries(cuda_sfml_app clipp sfml-graphics sfml-window sfml-system)

# Specify C++11 standard
#set_property(TARGET cuda_sfml_app PROPERTY CXX_STANDARD 17)

set_target_properties(
    cuda_sfml_app PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)
# Compile Info
set_target_properties(cuda_sfml_app PROPERTIES CUDA_ARCHITECTURES "50;72")
target_compile_features(cuda_sfml_app PUBLIC cxx_std_11)
target_compile_features(cuda_sfml_app PUBLIC cuda_std_11)