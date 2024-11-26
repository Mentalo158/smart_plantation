# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# Third party components
set(EXTRA_COMPONENT_DIRS
    "${CMAKE_SOURCE_DIR}/components/bh1750"
    "${CMAKE_SOURCE_DIR}/components/i2cdev"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Smart_Plantation)
