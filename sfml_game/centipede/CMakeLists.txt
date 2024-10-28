# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Lab1 VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(OPENAL_LIBRARY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/libs-msvc/x64/openal32.lib)

# Add source files
file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/code/*.cpp)
# Add header files
file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/code/*.h)

# Add the executable
add_executable(Lab1 ${SOURCES} ${HEADERS})

include_directories(${PROJECT_SOURCE_DIR}/../SFML/include)

link_directories(${PROJECT_SOURCE_DIR}/../SFML/lib)

# Link the executable to the libraries in the lib directory
target_link_libraries(Lab1 PUBLIC sfml-graphics sfml-system sfml-window ${OPENAL_LIBRARY})

set_target_properties(
     Lab1 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${COMMON_OUTPUT_DIR}/bin"
)

# Copy DLLs using file(COPY ...)
file(COPY ${PROJECT_SOURCE_DIR}/../SFML/extlibs/bin/x64/openal32.dll
     DESTINATION "${COMMON_OUTPUT_DIR}/bin")
file(COPY ${PROJECT_SOURCE_DIR}/fonts
     DESTINATION "${COMMON_OUTPUT_DIR}/bin")
file(COPY ${PROJECT_SOURCE_DIR}/graphics
     DESTINATION "${COMMON_OUTPUT_DIR}/bin")
file(COPY "${PROJECT_SOURCE_DIR}/low res"
     DESTINATION "${COMMON_OUTPUT_DIR}/bin")
#file(COPY ${PROJECT_SOURCE_DIR}/sound
#     DESTINATION "${COMMON_OUTPUT_DIR}/bin")