cmake_minimum_required(VERSION 2.6)

project(ProMeshDocuGen)

set(LIBS grid_s)

# LIBGRID LIBRARY
option(UG_ROOT_PATH "The path to ug4. Has to contain the ugbase and lib directories.")
if(NOT UG_ROOT_PATH)
	message(FATAL_ERROR "ERROR: You have to specify UG_ROOT_PATH! Add an option -DUG_ROOT_PATH=path-to-ug4")
endif(NOT UG_ROOT_PATH)


# TETGEN LIBRARY
option(TETGEN_PATH "The path to tetgen. Has to contain the static tet library.")
if(NOT TETGEN_PATH)
	message(FATAL_ERROR "ERROR: You have to specify TETGEN_PATH! Add an option -DTETGEN_PATH=path-to-tetgen")
endif(NOT TETGEN_PATH)

message(STATUS "INFO: TETGEN_PATH was set to: " ${TETGEN_PATH})

find_library(TETGEN_LIBS NAMES tet PATHS ${TETGEN_PATH})
if(TETGEN_LIBS-NOTFOUND)
	message(FATAL_ERROR "ERROR: TETGEN lib could not be found.")
else(TETGEN_LIBS-NOTFOUND)
	message(STATUS "INFO: Using TETGEN. Please check the TETGEN license!")
# UG requires this define if tetgen is used
	ADD_DEFINITIONS(-DUG_TETGEN)
# Since we want to use tetgen as a library, we have to add the following definition
	ADD_DEFINITIONS(-DTETLIBRARY)
	include_directories(${TETGEN_PATH})
	link_directories(${TETGEN_PATH})
	set(LIBS ${LIBS} ${TETGEN_LIBS})
endif(TETGEN_LIBS-NOTFOUND)



include_directories(${UG_ROOT_PATH}/ugbase)
include_directories(${UG_ROOT_PATH}/plugins/experimental/promesh)
include_directories(${UG_ROOT_PATH}/externals/boost_1_56_0)
link_directories(${UG_ROOT_PATH}/lib)

add_executable(docugen docugen.cpp)
target_link_libraries(docugen ${LIBS})
