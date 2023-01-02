cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(gprt-rtow CXX)

# Allow user to specify <project>_ROOT variables
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
  cmake_policy(SET CMP0074 NEW)
endif()

# Enable correct usage of CXX_EXTENSIONS
if (CMAKE_VERSION VERSION_GREATER_EQUAL 3.22)
  cmake_policy(SET CMP0128 NEW)
endif()

add_subdirectory(3rdParty/argparse)
add_subdirectory(3rdParty/gprt)

embed_devicecode(
  OUTPUT_TARGET
    dbl_deviceCode
  SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deviceCode.hlsl
)

add_executable(gprt-rtow src/main.cpp)
target_compile_features(gprt-rtow PUBLIC cxx_std_14)
target_include_directories(gprt-rtow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gprt-rtow PUBLIC dbl_deviceCode argparse gprt::gprt glfw ${OPENGL_gl_LIBRARY})
