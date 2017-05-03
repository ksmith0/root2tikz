project(GressOutput CXX)
cmake_minimum_required(VERSION 2.8)

#Set the C++ standard to c++11.
set(CMAKE_CXX_STANDARD 11)

#Enable the use of runtime paths on Mac OSX.
set(CMAKE_MACOSX_RPATH ON)

#if user does not specify prefix we assign it to a default value.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	message(STATUS "Install Prefix not specified.")
	file(MAKE_DIRECTORY install)
	get_filename_component(INSTALL_DIR ${CMAKE_SOURCE_DIR}/install REALPATH)
	set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR} CACHE PATH "Install Prefix" FORCE)
endif()
message(STATUS "Installing to ${CMAKE_INSTALL_PREFIX}")

#Find ROOT and setup the correct directories.
find_package(ROOT REQUIRED)
include(${ROOT_USE_FILE})

#Add the local include directory.
include_directories(include)

#Add the source directory.
add_subdirectory(source)

