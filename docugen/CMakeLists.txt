# Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
# Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
# Copyright (c) 2006-2015:  Sebastian Reiter
# Author: Sebastian Reiter
#
# This file is part of ProMesh.
# 
# ProMesh is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 3 (as published by the
# Free Software Foundation) with the following additional attribution
# requirements (according to LGPL/GPL v3 §7):
# 
# (1) The following notice must be displayed in the Appropriate Legal Notices
# of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
# 
# (2) The following bibliography is recommended for citation and must be
# preserved in all covered files:
# "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
#   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.

cmake_minimum_required(VERSION 2.6)

project(ProMeshDocuGen)

add_definitions(-DPROMESH_ROOT_PATH="${CMAKE_SOURCE_DIR}/../")
if(UNIX AND NOT CYGWIN)
	add_definitions(-DUNIX)
elseif(WIN32 OR CYGWIN)
	add_definitions(-DWINDOWS)
else(UNIX AND NOT CYGWIN)
	message(STATUS "YOUR OS MAY NOT BE FULLY SUPPORTED (NOT UNIX???). File functions may be not working.")
	add_definitions(-DUNIX)
endif(UNIX AND NOT CYGWIN)

set(Boost_USE_STATIC_LIBS   ON)
FIND_PACKAGE( Boost 1.48 COMPONENTS serialization REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

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



include_directories(${UG_ROOT_PATH}/ugcore/ugbase)
include_directories(${UG_ROOT_PATH}/plugins/experimental/promesh)
link_directories(${UG_ROOT_PATH}/lib)

add_executable(docugen docugen.cpp)
target_link_libraries(docugen ${LIBS} ${Boost_LIBRARIES})
