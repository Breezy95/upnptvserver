cmake_minimum_required(VERSION 3.20)

set(project_name upnptvserver)
project(${project_name})

set(LIBDIR "/usr/local/lib/upnp")
set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_PREFIX_PATH "/usr/include/upnp")
find_package(LibXml2 REQUIRED)
add_subdirectory(oatpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GUPNP_AV REQUIRED gupnp-av-1.0)
pkg_check_modules(upnp REQUIRED libupnp)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GUPNP_DLNA REQUIRED gupnp-dlna-2.0)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0 gstreamer-pbutils-1.0)


# Include directories for gupnp-av and pupnp
include_directories(
    ${CMAKE_SOURCE_DIR}/src/controller
    ${CMAKE_SOURCE_DIR}/src/file_server
    ${CMAKE_SOURCE_DIR}/src/DTO
    ${CMAKE_SOURCE_DIR}/src/GObjects
    
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${LIBXML2_INCLUDE_DIR} ${GLIB_INCLUDE_DIRS} ${GOBJECT_INCLUDE_DIRS} ${GSTREAMER_INCLUDE_DIRS})
include_directories(${GUPNP_AV_INCLUDE_DIRS} ${upnp_INCLUDE_DIR} ${GUPNP_DLNA_INCLUDE_DIRS} "/usr/local/include/gupnp-dlna-2.0/libgupnp-dlna/metadata"  )
link_directories(${GUPNP_AV_LIBRARY_DIRS} "/usr/local/lib" ${upnp_INCLUDE_DIR} ${GUPNP_DLNA_INCLUDE_DIRS} "/usr/local/include/gupnp-dlna-2.0/libgupnp-dlna/metadata" ${GSTREAMER_INCLUDE_DIRS} )

file(GLOB SOURCES "src/App.cpp" "src/controller/UpnpController.cpp" "src/file_server/Utils.cpp")
add_executable(${project_name}.out ${SOURCES})

add_library(my_dlna_information src/GObjects/my_dlna_information.c)

target_link_options(${project_name}.out PRIVATE "-Wl,-rpath" ${LIBDIR})
target_include_directories(${project_name}.out PRIVATE ${upnp_INCLUDE_DIR})
target_link_libraries(${project_name}.out
    PUBLIC oatpp  
    PUBLIC upnp
    my_dlna_information
    ${GUPNP_AV_LIBRARIES} 
    ${LIBXML2_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GUPNP_DLNA_LIBRARIES}
    ${GSTREAMER_LIBRARIES}
    ixml
    avformat
  avcodec
  avutil
  avdevice
  avfilter
  swscale
  swresample
    pthread
)

