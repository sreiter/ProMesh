// created by Sebastian Reiter
// all rights reserved
// s.b.reiter@googlemail.com

#ifndef RCLICK_MENU_SCENE_INSPECTOR_H
#define RCLICK_MENU_SCENE_INSPECTOR_H

#include <QMenu>
#include "scene_inspector.h"

class RClickMenu_SceneInspector : public QWidget
{
	Q_OBJECT

	public:
		RClickMenu_SceneInspector(SceneInspector * sceneInspector);

		void exec(const QPoint& p);

	protected slots:
		void rename();
		void showAllSubsets();
		void hideAllSubsets();
		void printSubsetContents();

	private:
		QMenu*			m_menu;
		SceneInspector*	m_sceneInspector;

		QAction*	m_actRename;
		QAction*	m_actShowAllSubsets;
		QAction*	m_actHideAllSubsets;
		QAction*	m_actPrintSubsetContents;
		//QAction*	m_actHideOtherSubsets;
};

#endif // RCLICK_MENU_SCENE_INSPECTOR_H
