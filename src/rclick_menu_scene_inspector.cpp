// created by Sebastian Reiter
// all rights reserved
// s.b.reiter@googlemail.com

#include "rclick_menu_scene_inspector.h"
#include "app.h"
#include "tools/tool_dialog.h"

using namespace std;

RClickMenu_SceneInspector::
RClickMenu_SceneInspector(SceneInspector * sceneInspector) :
			QWidget(sceneInspector), m_sceneInspector(sceneInspector)
{
	m_menu = new QMenu(this);

//	populate the menu
	m_actRename = new QAction(tr("Rename"), this);
	connect(m_actRename, SIGNAL(triggered()), this, SLOT(rename()));
	m_menu->addAction(m_actRename);
	
	m_actShowAllSubsets = new QAction(tr("Show All Subsets"), this);
	connect(m_actShowAllSubsets, SIGNAL(triggered()), this, SLOT(showAllSubsets()));
	m_menu->addAction(m_actShowAllSubsets);

	m_actHideAllSubsets = new QAction(tr("Hide All Subsets"), this);
	connect(m_actHideAllSubsets, SIGNAL(triggered()), this, SLOT(hideAllSubsets()));
	m_menu->addAction(m_actHideAllSubsets);

	m_actPrintSubsetContents = new QAction(tr("Print Subset Contents"), this);
	connect(m_actPrintSubsetContents, SIGNAL(triggered()), this, SLOT(printSubsetContents()));
	m_menu->addAction(m_actPrintSubsetContents);
//	m_actHideOtherSubsets = new QAction(tr("Hide Other Subsets"), this);
}

void RClickMenu_SceneInspector::
exec(const QPoint& p){
	m_menu->exec(p);
}

void RClickMenu_SceneInspector::rename()
{
//	create a tool dialog with a single text-input box.
	this->close();
	LGObject* obj = app::getActiveObject();
	if(obj){
		ToolWidget* widget = new ToolWidget("rename", this,
										  NULL, IDB_OK | IDB_CANCEL);

		QDialog* dlg = new QDialog(NULL);
		QVBoxLayout* layout = new QVBoxLayout(dlg);
		dlg->setLayout(layout);
		layout->addWidget(widget);

		string curName = obj->name();
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1)
			curName = obj->get_subset_name(si);
		
		widget->addTextBox("name:", curName.c_str());
		
		if(dlg->exec()){
			curName = widget->to_string(0).toStdString();
			if(si != -1)
				obj->set_subset_name(si, curName.c_str());
			else
				obj->set_name(curName.c_str());
			m_sceneInspector->refreshView();
		}
		delete dlg;
	}
}

void RClickMenu_SceneInspector::
showAllSubsets(){
	int objInd = m_sceneInspector->getActiveObjectIndex();
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			m_sceneInspector->showSubset(objInd, i);
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}

void RClickMenu_SceneInspector::
printSubsetContents()
{
	LGObject* obj = dynamic_cast<LGObject*>(m_sceneInspector->getActiveObject());
	int si = m_sceneInspector->getActiveSubsetIndex();
	if(obj && (si != -1)){
		PrintElementNumbers(obj->get_subset_handler().get_geometric_objects_in_subset(si));
	}
}

void RClickMenu_SceneInspector::
hideAllSubsets(){
	int objInd = m_sceneInspector->getActiveObjectIndex();
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			m_sceneInspector->hideSubset(objInd, i);
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}
