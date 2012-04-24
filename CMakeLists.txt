cmake_minimum_required(VERSION 2.6)

####
# ProMesh3
####

project(P_PROMESH3)

# Qt4 with OpenGL support is required.
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTOPENGL TRUE)

FIND_PACKAGE(OpenGL REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR})

#We'll use ugs internal boost version
#BOOST is required (at least version 1.40)
#FIND_PACKAGE(Boost 1.40.0 REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})

# Some options which allow to control makefile generation
option(DEBUG "If enabled a debug build is performed - this slows down performance." OFF)
option(UG_ROOT_PATH "The path to ug4. Has to contain the ugbase and lib directories.")
option(TETGEN_PATH "The path to tetgen. Has to contain the static tet library.")

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
include_directories(${UG_ROOT_PATH}/externals/boost_1_48_0)
link_directories(${UG_ROOT_PATH}/lib)


# Now add the sources
set(ProMeshSRC	main.cpp
				main_window.cpp
				main_window_input.cpp
				scene_inspector.cpp
				scene_item_model.cpp
				delegates.cpp
				clip_plane_widget.cpp
				color_widget.cpp
				undo.cpp
				rclick_menu_scene_inspector.cpp
				view3d/view3d.cpp
				view3d/camera/quaternion.cpp
				view3d/camera/model_viewer_camera.cpp
				view3d/camera/matrix44.cpp
				view3d/camera/basic_camera.cpp
				view3d/camera/arc_ball.cpp
				scene/lg_object.cpp
				scene/lg_scene.cpp
				scene/lg_tmp_methods.cpp
				scene/plane_sphere.cpp
				scene/scene_interface.cpp
				tools/tool_manager.cpp
				tools/standard_tools.cpp
				tools/tool_dialog.cpp
				tools/selection_tools.cpp
				tools/mark_tools.cpp
				tools/grid_generation_tools.cpp
				tools/coordinate_transform_tools.cpp
				tools/topology_tools.cpp
				tools/info_tools.cpp
				tools/subset_tools.cpp
				tools/camera_tools.cpp
				tools/fracture_tools.cpp
				tools/refinement_tools.cpp
				tools/object_tools.cpp
				tools/remeshing_tools.cpp
				tools/dialog_components/matrix_widget.cpp
				tools/dialog_components/file_widget.cpp
				widgets/extendible_widget.cpp
				widgets/icon_tab_widget.cpp
				widgets/tool_browser_widget.cpp
				widgets/widget_container.cpp
				widgets/widget_list.cpp)

# Only header containing Q_OBJECTS have to be listed here
set(ProMeshHEADERS	view3d/view3d.h
					view3d/camera/camera.h
					view3d/renderer3d_interface.h
					scene/scene_interface.h
					view3d/renderer3d_interface.h
					scene/scene_template.h
					scene/scene_template_impl.hpp
					scene/lg_object.h
					scene/lg_scene.h
					main_window.h
					scene/lg_include.h
					scene/plane_sphere.h
					scene_inspector.h
					scene_item_model.h
					delegates.h
					clip_plane_widget.h
					color_widget.h
					QDebugStream.h
					app.h
					tools/tool_manager.h
					QDebugStream.h
					tools/standard_tools.h
					tools/tool_dialog.h
					view3d/camera/vec_math.h
					view3d/camera/quaternion.h
					view3d/camera/matrix44.h
					view3d/camera/arc_ball.h
					tools/tool_frac_to_layer.h
					tools/tools_util.h
					undo.h
					rclick_menu_scene_inspector.h
					tools/tool_coordinates.h
					tools/dialog_components/matrix_widget.h
					tools/dialog_components/file_widget.h
					widgets/extendible_widget.h
					widgets/icon_tab_widget.h
					widgets/tool_browser_widget.h
					widgets/widget_container.h
					widgets/widget_list.h)

# resources
set(ProMeshRESOURCES	ProMesh3.qrc)

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
		ADD_EXECUTABLE(ProMesh3 MACOSX_BUNDLE ${allSources})
	else(APPLE)
		ADD_EXECUTABLE(ProMesh3 ${allSources})
	endif(APPLE)
else(UNIX)
	if(MINGW)
		# resource compilation for MinGW
		ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o
							COMMAND windres.exe -I${CMAKE_SOURCE_DIR}/deploy_data -i${CMAKE_SOURCE_DIR}/deploy_data/ProMeshIcon.rc 
                             -o ${CMAKE_CURRENT_BINARY_DIR}/ProMeshIcon.o )
		ADD_EXECUTABLE(ProMesh3 WIN32 ${allSources} ProMeshIcon.o)
	endif(MINGW)
endif(UNIX)

TARGET_LINK_LIBRARIES(ProMesh3 ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${UG_LIBRARIES} tet)
