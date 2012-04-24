//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m10 d01

#ifndef __H__MAIN_WINDOW__
#define __H__MAIN_WINDOW__

#include <QMainWindow>
#include <QModelIndex>
#include <QSettings>
#include "color_widget.h"
#include "scene/lg_object.h"
#include "view3d/view3d.h"
#include "rclick_menu_scene_inspector.h"
#include "scene/lg_scene.h"
#include "tools/tool_manager.h"

////////////////////////////////////////////////////////////////////////
//	predeclarations
class View3D;
class LGScene;
class ISceneObject;

class QFileDialog;
class QAction;
class QTreeView;
class QToolBar;
class QPushButton;
class QPoint;
class SceneInspector;
class QToolButton;

////////////////////////////////////////////////////////////////////////
///	the main window.
/**
 * This class organizes the toolbars, menus, views and tool-windows
 * of the application.
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT

	protected:
		enum MouseMoveAction{
			MMA_DEFAULT = TT_NONE,
			MMA_GRAB = TT_GRAB,
			MMA_ROTATE = TT_ROTATE,
			MMA_SCALE = TT_SCALE
		};

		enum Axis{
			X_AXIS = 1,
			Y_AXIS = 1<<1,
			Z_AXIS = 1<<2
		};

	public:
		MainWindow();
		~MainWindow();

		void init();

		LGScene* get_scene()	{return m_scene;}

		bool load_grid_from_file(const char* filename);
		bool save_object_to_file(ISceneObject* obj, const char* filename);
        LGObject* create_empty_object(const char* name);
		inline QSettings& settings()	{return m_settings;}

		LGObject* getActiveObject();
		View3D*	getView3D()				{return m_pView;}

	public slots:
		void newGeometry();
		int openFile();///< returns the number of successfully opened files.
		bool saveToFile();
		bool exportToUG3();
		void eraseActiveSceneObject();///< erases the active scene object.
		void showHelp();
		void showRecentChanges();
		void showShortcuts();
		void showLog();

	protected slots:
		void frontDrawModeChanged(int newMode);
		void backDrawModeChanged(int newMode);
		void backgroundColorChanged(const QColor& color);
		void view3dMousePressed(QMouseEvent *event);
		void view3dMouseMoved(QMouseEvent *event);
		void view3dMouseReleased(QMouseEvent *event);
		void view3dKeyReleased(QKeyEvent* event);
		void selectionElementChanged(int newElement);
		void selectionElementChanged(bool enabled);
		void selectionModeChanged(int newMode);
		void elementDrawModeChanged(int newMode);
		void undo();
		void redo();
		void sceneInspectorClicked(QMouseEvent* event);

	protected:
		void closeEvent(QCloseEvent *event);

		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);

		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void keyPressEvent(QKeyEvent* event);

		QToolBar* createVisibilityToolbar();

		template <class TElem>
		void selectElement(LGObject* obj, TElem* elem,
							bool extendSelection);

		uint getLGElementMode();

		void beginMouseMoveAction(MouseMoveAction mma);
		void updateMouseMoveAction();
		void endMouseMoveAction(bool bApply);

	protected:
	//	3d view
		View3D*		m_pView;
		LGScene*	m_scene;

	//	tools
		ColorWidget*	m_bgColor;
		QSettings		m_settings;

	//	state-variables
		int m_selectionElement;
		int	m_selectionMode;
		int m_elementModeListIndex;
		int m_mouseMoveAction;

	//	important for selection etc
		QPoint m_mouseDownPos;
		QPoint m_mouseMoveActionStart;
		LGObject* m_mouseMoveActionObject;///< Only valid if m_mouseMoveAction != MMA_DEFAULT
		unsigned int m_activeAxis;

	//	dialogs
		QFileDialog* m_dlgGeometryFiles;
		SceneInspector* m_sceneInspector;
		QDockWidget* m_pLog;
		RClickMenu_SceneInspector*	m_rclickMenu_SI;
		ToolManager* m_toolManager;
	//	menus
		QMenu*		m_toolsMenu;
	//	actions
		QAction*	m_actNew;
		QAction*	m_actOpen;
		QAction*	m_actSave;
		QAction*	m_actExport;
		QAction*	m_actErase;
		QAction*	m_actExportUG3;
		QAction*	m_actHelpControls;
		QAction*	m_actRecentChanges;
		QAction*	m_actShortcuts;

		QToolButton*	m_tbSelVrts;
		QToolButton*	m_tbSelEdges;
		QToolButton*	m_tbSelFaces;
		QToolButton*	m_tbSelVols;
};

#endif // __H__MAIN_WINDOW__
