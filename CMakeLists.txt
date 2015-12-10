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

cmake_minimum_required(VERSION 2.8.11)
project(P_PROMESH4)


# Some options which allow to control makefile generation
option(DEBUG "If enabled a debug build is performed - this slows down performance." OFF)
option(UG_ROOT_PATH "The path to ug4. Has to contain the ugbase and lib directories.")
option(TETGEN_PATH "The path to tetgen. Has to contain the static tet library.")
option(QT_CMAKE_PATH "Folder containing cmake-modules for the chosen architecture.")
option(USE_WEBKIT "Enable or disable QtWebKit support. If enabled, ProMesh's help will be displayed in an internal window." OFF)


set(ProMeshSRC	src/app.cpp
				src/main.cpp
				src/main_window.cpp
				src/main_window_input.cpp
				src/scene_inspector.cpp
				src/scene_item_model.cpp
				src/delegates.cpp
				src/clip_plane_widget.cpp
				src/color_widget.cpp
				src/undo.cpp
				src/rclick_menu_scene_inspector.cpp
				src/view3d/view3d.cpp
				src/view3d/camera/quaternion.cpp
				src/view3d/camera/model_viewer_camera.cpp
				src/view3d/camera/matrix44.cpp
				src/view3d/camera/basic_camera.cpp
				src/view3d/camera/arc_ball.cpp
				src/scene/lg_object.cpp
				src/scene/lg_scene.cpp
				src/scene/lg_tmp_methods.cpp
				src/scene/plane_sphere.cpp
				src/scene/scene_interface.cpp
				src/tools/tool_frac_to_layer.cpp
				src/tools/tool_manager.cpp
				src/tools/standard_tools.cpp
				src/tools/tool_dialog.cpp
				src/tools/selection_tools.cpp
				src/tools/mark_tools.cpp
				src/tools/grid_generation_tools.cpp
				src/tools/coordinate_transform_tools.cpp
				src/tools/topology_tools.cpp
				src/tools/info_tools.cpp
				src/tools/subset_tools.cpp
				src/tools/camera_tools.cpp
				src/tools/fracture_tools.cpp
				src/tools/refinement_tools.cpp
				src/tools/registry_tools.cpp
				src/tools/remeshing_tools.cpp
				src/tools/script_tools.cpp
				src/util/file_util.cpp
				src/widgets/coordinates_widget.cpp
				src/widgets/double_slider.cpp
				src/widgets/extendible_widget.cpp
				src/widgets/file_widget.cpp
				src/widgets/icon_tab_widget.cpp
				src/widgets/matrix_widget.cpp
				src/widgets/tool_browser_widget.cpp
				src/widgets/truncated_double_spin_box.cpp
				src/widgets/widget_container.cpp
				src/widgets/widget_list.cpp)


if(USE_WEBKIT)
	add_definitions(-DPROMESH_USE_WEBKIT)
	set(ProMeshSRC ${ProMeshSRC} src/widgets/help_browser.cpp)
endif(USE_WEBKIT)



# Qt5 with OpenGL support is required.
# in order to use a custom installation of QT, specify QT_CMAKE_PATH when running
# cmake for the first time for your build (e.g. "...pathToQt/5.4/gcc_64/lib/cmake/")
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(QT_CMAKE_PATH)
	FIND_PACKAGE(Qt5Widgets REQUIRED PATHS ${QT_CMAKE_PATH}/Qt5Widgets)
	FIND_PACKAGE(Qt5OpenGL REQUIRED PATHS ${QT_CMAKE_PATH}/Qt5OpenGL)
	if(USE_WEBKIT)
		FIND_PACKAGE(Qt5WebKitWidgets REQUIRED PATHS ${QT_CMAKE_PATH}/Qt5WebKitWidgets)
	endif(USE_WEBKIT)
else(QT_CMAKE_PATH)
	FIND_PACKAGE(Qt5Widgets REQUIRED)
	FIND_PACKAGE(Qt5OpenGL REQUIRED)
	if(USE_WEBKIT)
		FIND_PACKAGE(Qt5WebKitWidgets REQUIRED)
	endif(USE_WEBKIT)
endif(QT_CMAKE_PATH)


FIND_PACKAGE(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

if(DEBUG)
	message(STATUS "INFO: Debug mode. (Enable release-mode by defining -DDEBUG=OFF)")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
	add_definitions(-DUG_DEBUG)
else(DEBUG)
	message(STATUS "INFO: Release mode. (Enable debug-mode by defining -DDEBUG=ON)")
	# Enable these flags for a fast ublas algebra
	
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DBOOST_UBLAS_NDEBUG -O3 -Wall -funroll-loops -ftree-vectorize -msse3 -mssse3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DBOOST_UBLAS_NDEBUG")
	if(GENERATE_DEBUG_SYMBOLS)
		message(STATUS "INFO: Generating debug symbols (-g) (Disable by defining -DGENERATE_DEBUG_SYMBOLS=OFF)")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif(GENERATE_DEBUG_SYMBOLS)
	remove_definitions(-DUG_DEBUG)
endif(DEBUG)

if(NOT UG_ROOT_PATH)
	message(FATAL_ERROR "ERROR: You have to specify UG_ROOT_PATH! Add an option -DUG_ROOT_PATH=path-to-ug4")
endif(NOT UG_ROOT_PATH)

if(NOT TETGEN_PATH)
	message(FATAL_ERROR "ERROR: You have to specify TETGEN_PATH! Add an option -DTETGEN_PATH=path-to-tetgen")
endif(NOT TETGEN_PATH)

message(STATUS "INFO: TETGEN_PATH was set to: " ${TETGEN_PATH})

message(STATUS "INFO: USE_WEBKIT: " ${USE_WEBKIT})


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
	set(PROMESH_LIBS ${PROMESH_LIBS} ${TETGEN_LIBS})
endif(TETGEN_LIBS-NOTFOUND)

include_directories(${UG_ROOT_PATH}/ugcore/ugbase)
include_directories(${UG_ROOT_PATH}/plugins/ProMesh)
include_directories(${UG_ROOT_PATH}/externals/BoostForUG4)
link_directories(${UG_ROOT_PATH}/lib)

# On some unix systems (e.g. ubuntu 12.04) glu.h is not automatically included.
# We thus set the define to additionally include glu.h
# This may have to be improved for broader compatibility
if(UNIX)
	if(APPLE)
		add_definitions(-DPROMESH_INCLUDE_OPENGL_GLU)
	else(APPLE)
		add_definitions(-DPROMESH_INCLUDE_GL_GLU)
	endif(APPLE)
elseif(WIN32)
	add_definitions(-DPROMESH_INCLUDE_GL_GLU)
endif(UNIX)


# resources
set(ProMeshRESOURCES	ProMesh.qrc
						docs/html-resources.qrc)
QT5_ADD_RESOURCES(ProMeshRESOURCES_RCC ${ProMeshRESOURCES})

SET(allSources ${ProMeshSRC} ${ProMeshRESOURCES_RCC})



if(UNIX)
	if(APPLE)
		#SET(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.5.sdk)
		#SET(CMAKE_OSX_DEPLOYMENT_TARGET 10.5)
		ADD_EXECUTABLE(ProMesh4 MACOSX_BUNDLE ${allSources})
	else(APPLE)
		ADD_EXECUTABLE(ProMesh4 ${allSources})
	endif(APPLE)
else(UNIX)
	if(MINGW)
		# resource compilation for MINGW
		ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o
							COMMAND windres.exe -I${CMAKE_SOURCE_DIR}/deploy_data -i${CMAKE_SOURCE_DIR}/deploy_data/ProMeshIcon.rc 
                             -o ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o )
		ADD_EXECUTABLE(ProMesh4 WIN32 ${allSources} ProMeshIcon.o)
	else(MINGW)
		ADD_EXECUTABLE(ProMesh4 WIN32 ${allSources} ${CMAKE_SOURCE_DIR}/deploy_data/ProMeshIcon.rc)
	endif(MINGW)
endif(UNIX)

set(PM_LIBS ${OPENGL_LIBRARIES} Qt5::OpenGL Qt5::Widgets grid_s tet)

if(USE_WEBKIT)
	set(PM_LIBS ${PM_LIBS} Qt5::WebKitWidgets)
endif(USE_WEBKIT)

TARGET_LINK_LIBRARIES(ProMesh4 ${PM_LIBS})
