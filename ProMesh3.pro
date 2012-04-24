QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
CONFIG += x86 \
    ppc \
    warn_off

# ./../ug4/trunk/lib \
LIBS += -L../ug4/trunk/lib \
    -L/Users/sreiter/libs/tetgen1.4.3 \
		-LC:/libs/tetgen1.4.3 \
    -lugscript \
    -lugbridge \
    -lgrid \
    -lnode_tree \
    -lregistry \
    -lcommon \ # -ldiscretization \
    -ltet
INCLUDEPATH += ../ug4/trunk/ugbase

# This really shouldn't be an explicit path. However, I wasn't able
# to find a way to access environment variables here.
# It seems that variables from .bash_profile etc. are not loaded
# in the qmake environment, so they can't be accessed using
# $$(...) or $$system(...).
INCLUDEPATH += /Users/sreiter/libs/boost_1_41_0
INCLUDEPATH += /Users/sreiter/libs/boost_1_43_0
INCLUDEPATH += /Users/sreiter/libs/tetgen1.4.3
INCLUDEPATH += C:/libs/boost_1_44_0
INCLUDEPATH += C:/libs/tetgen1.4.3

DEFINES += TETLIBRARY
QT += opengl
HEADERS += view3d/view3d.h \
    view3d/camera/camera.h \
    view3d/renderer3d_interface.h \
    scene/scene_interface.h \
    view3d/renderer3d_interface.h \
    scene/scene_template.h \
    scene/scene_template_impl.hpp \
    scene/lg_object.h \
    scene/lg_scene.h \
    main_window.h \
    scene/lg_include.h \
    scene/plane_sphere.h \
    scene_inspector.h \
    scene_item_model.h \
    delegates.h \
    clip_plane_widget.h \
    color_widget.h \
    QDebugStream.h \
    app.h \
    tools/tool_manager.h \
    QDebugStream.h \
    tools/standard_tools.h \
    tools/tool_dialog.h \
    view3d/camera/vec_math.h \
    view3d/camera/quaternion.h \
    view3d/camera/matrix44.h \
    view3d/camera/arc_ball.h \
    tools/tool_frac_to_layer.h \
    tools/triangle_fill_sweep_line_impl.hpp \
    tools/triangle_fill_sweep_line.h \
    tools/tools_util.h \
    undo.h \
    rclick_menu_scene_inspector.h \
    tools/tool_coordinates.h
SOURCES += view3d/view3d.cpp \
    main.cpp \
    view3d/camera/quaternion.cpp \
    view3d/camera/model_viewer_camera.cpp \
    view3d/camera/matrix44.cpp \
    view3d/camera/basic_camera.cpp \
    view3d/camera/arc_ball.cpp \
    scene/lg_object.cpp \
    scene/lg_scene.cpp \
    main_window.cpp \
    scene/lg_tmp_methods.cpp \
    scene/plane_sphere.cpp \
    scene_inspector.cpp \
    scene_item_model.cpp \
    delegates.cpp \
    clip_plane_widget.cpp \
    color_widget.cpp \
    tools/tool_manager.cpp \
    scene/scene_interface.cpp \
    tools/standard_tools.cpp \
    tools/tool_dialog.cpp \
    tools/selection_tools.cpp \
    tools/mark_tools.cpp \
    tools/optimization_tools.cpp \
    tools/grid_generation_tools.cpp \
    tools/coordinate_transform_tools.cpp \
    tools/topology_tools.cpp \
    tools/info_tools.cpp \
    tools/subset_tools.cpp \
    tools/camera_tools.cpp \
    tools/fracture_tools.cpp \
    tools/triangle_fill_sweep_line.cpp \
    undo.cpp \
    tools/refinement_tools.cpp \
    rclick_menu_scene_inspector.cpp
RESOURCES += ProMesh3.qrc
FORMS += 
OTHER_FILES += recent_changes.txt
