cmake_minimum_required(VERSION 3.10)
project(UPnP_Server)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for pupnp, oatpp, and ixml
include_directories(${CMAKE_SOURCE_DIR}/pupnp/ixml/inc)
include_directories(${CMAKE_SOURCE_DIR}/pupnp)
include_directories(${CMAKE_SOURCE_DIR}/pupnp/src/api)
include_directories(${CMAKE_SOURCE_DIR}/oatpp/src)
include_directories(${CMAKE_SOURCE_DIR}/pupnp/upnp/inc)
include_directories(${CMAKE_SOURCE_DIR}/src/file_server)


# Source files
#set(SOURCES
 #   ${CMAKE_SOURCE_DIR}/src/main.cpp
#    ${CMAKE_SOURCE_DIR}/src/MyController.hpp
    
#)

file(GLOB SOURCES "src/App.cpp" "src/file_server/Utils.cpp" "src/controller/UpnpController.cpp")

add_definitions(
        -DEXAMPLE_MEDIA_FOLDER="${CMAKE_CURRENT_LIST_DIR}/src"
)


find_package(PkgConfig REQUIRED)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)


# Add executable
add_executable(UPnP_Server ${SOURCES})

# Find oatpp 
find_package(oatpp REQUIRED)

find_library(IXML_LIB NAMES ixml HINTS /usr/local/lib /usr/lib pupnp/)

# Check if ixml library was found
if(NOT IXML_LIB)
    message(FATAL_ERROR "Could not find the ixml library.")
endif()


# Link libraries
target_link_libraries(UPnP_Server upnp pthread oatpp::oatpp ${IXML_LIB}  ${AVFORMAT_LIBRARIES} ${AVCODEC_LIBRARIES} ${AVUTIL_LIBRARIES} )
