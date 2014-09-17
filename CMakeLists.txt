cmake_minimum_required(VERSION 2.6)

####
# ProMesh4
####

project(P_PROMESH4)

# Some options which allow to control makefile generation
option(DEBUG "If enabled a debug build is performed - this slows down performance." OFF)
option(UG_ROOT_PATH "The path to ug4. Has to contain the ugbase and lib directories.")
option(TETGEN_PATH "The path to tetgen. Has to contain the static tet library.")

# Qt4 with OpenGL support is required.
# in order to use a custom installation of QT4, execute
# "export PATH=path_to_your_qmake:$PATH"
# before calling cmake for the first time for your build
FIND_PACKAGE(Qt4 REQUIRED)

SET(QT_USE_QTOPENGL TRUE)

FIND_PACKAGE(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

#We'll use ugs internal boost version
#BOOST is required (at least version 1.40)
#FIND_PACKAGE(Boost 1.40.0 REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})


if(DEBUG)
	message(STATUS "INFO: Debug mode. (Enable release-mode by defining -DDEBUG=OFF)")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")
	add_definitions(-DUG_DEBUG)
else(DEBUG)
	message(STATUS "INFO: Release mode. (Enable debug-mode by defining -DDEBUG=ON)")
	# Enable these flags for a fast ublas algebra
	
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DBOOST_UBLAS_NDEBUG -O3 -Wall -funroll-loops -ftree-vectorize -msse3 -mssse3")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DBOOST_UBLAS_NDEBUG -O3 -Wall -funroll-loops -ftree-vectorize")
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

include_directories(${UG_ROOT_PATH}/ugbase)
include_directories(${UG_ROOT_PATH}/plugins/experimental/promesh)
include_directories(${UG_ROOT_PATH}/externals/boost_1_56_0)
link_directories(${UG_ROOT_PATH}/lib)

# On some unix systems (e.g. ubuntu 12.04) glu.h is not automatically included.
# We thus set the define to additionally include glu.h
# This may have to be improved for broader compatibility
if(UNIX AND NOT APPLE)
	add_definitions(-DPROMESH_INCLUDE_GLU)
endif(UNIX AND NOT APPLE)

# Now add the sources
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
				src/tools/dialog_components/matrix_widget.cpp
				src/tools/dialog_components/file_widget.cpp
				src/widgets/extendible_widget.cpp
				src/widgets/icon_tab_widget.cpp
				src/widgets/tool_browser_widget.cpp
				src/widgets/widget_container.cpp
				src/widgets/widget_list.cpp
				src/widgets/double_slider.cpp)

# Only header containing Q_OBJECTS have to be listed here
set(ProMeshHEADERS	src/view3d/view3d.h
					src/view3d/camera/camera.h
					src/view3d/renderer3d_interface.h
					src/scene/scene_interface.h
					src/view3d/renderer3d_interface.h
					src/scene/scene_template.h
					src/scene/scene_template_impl.hpp
					src/scene/lg_object.h
					src/scene/lg_scene.h
					src/main_window.h
					src/scene/lg_include.h
					src/scene/plane_sphere.h
					src/scene_inspector.h
					src/scene_item_model.h
					src/delegates.h
					src/clip_plane_widget.h
					src/color_widget.h
					src/QDebugStream.h
					src/app.h
					src/tools/tool_manager.h
					src/QDebugStream.h
					src/tools/standard_tools.h
					src/tools/tool_dialog.h
					src/view3d/camera/vec_math.h
					src/view3d/camera/quaternion.h
					src/view3d/camera/matrix44.h
					src/view3d/camera/arc_ball.h
					src/tools/tool_frac_to_layer.h
					src/tools/tools_util.h
					src/undo.h
					src/rclick_menu_scene_inspector.h
					src/tools/tool_coordinates.h
					src/tools/dialog_components/matrix_widget.h
					src/tools/dialog_components/file_widget.h
					src/widgets/extendible_widget.h
					src/widgets/icon_tab_widget.h
					src/widgets/tool_browser_widget.h
					src/widgets/widget_container.h
					src/widgets/widget_list.h
					src/widgets/double_slider.h)

# resources
set(ProMeshRESOURCES	ProMesh.qrc)

# The ug4 libraries
#set(UG_LIBRARIES ugscript ugbridge grid node_tree registry common)
set(UG_LIBRARIES grid_s)

QT4_WRAP_CPP(ProMeshHEADERS_MOC ${ProMeshHEADERS})
QT4_ADD_RESOURCES(ProMeshRESOURCES_RCC ${ProMeshRESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

SET(allSources ${ProMeshSRC} ${ProMeshHEADERS_MOC} ${ProMeshRESOURCES_RCC})

if(UNIX)
	if(APPLE)
		#SET( CMAKE_OSX_ARCHITECTURES ppc;i386 )
		SET( CMAKE_OSX_ARCHITECTURES i386 )
		SET( CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.5.sdk )
		SET( CMAKE_OSX_DEPLOYMENT_TARGET 10.5 )
		ADD_EXECUTABLE(ProMesh4 MACOSX_BUNDLE ${allSources})
	else(APPLE)
		ADD_EXECUTABLE(ProMesh4 ${allSources})
	endif(APPLE)
else(UNIX)
	if(MINGW)
		# resource compilation for MinGW
		ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o
							COMMAND windres.exe -I${CMAKE_SOURCE_DIR}/deploy_data -i${CMAKE_SOURCE_DIR}/deploy_data/ProMeshIcon.rc 
                             -o ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o )
		ADD_EXECUTABLE(ProMesh4 WIN32 ${allSources} ProMeshIcon.o)
	endif(MINGW)
endif(UNIX)

TARGET_LINK_LIBRARIES(ProMesh4 ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${UG_LIBRARIES} tet)
