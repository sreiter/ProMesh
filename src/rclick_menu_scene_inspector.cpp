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

	m_actToggleAllSubsetVisibilities = new QAction(tr("Toggle All Subset Visibilities"), this);
	connect(m_actToggleAllSubsetVisibilities, SIGNAL(triggered()), this, SLOT(toggleAllSubsetVisibilities()));
	m_menu->addAction(m_actToggleAllSubsetVisibilities);

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
		/*ToolWidget* widget = new ToolWidget("rename", this,
										  NULL, IDB_APPLY);

		QDialog* dlg = new QDialog(NULL);
		QVBoxLayout* layout = new QVBoxLayout(dlg);
		dlg->setLayout(layout);
		layout->addWidget(widget);

		string curName = obj->name();
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1)
			curName = obj->get_subset_name(si);
		
		widget->addTextBox("name:", curName.c_str());
		*/
		string curName = obj->name();
		int si = m_sceneInspector->getActiveSubsetIndex();
		if(si != -1)
			curName = obj->get_subset_name(si);

		QDialog* dlg = new QDialog(this);
		dlg->setWindowTitle(tr("rename"));
		QVBoxLayout* layout = new QVBoxLayout(dlg);
		dlg->setLayout(layout);

		QLineEdit* text = new QLineEdit(dlg);
		layout->addWidget(text);
		text->setText(QString::fromUtf8(curName.c_str()));
		text->selectAll();

		QHBoxLayout* hlayout = new QHBoxLayout();
		layout->addLayout(hlayout);

		hlayout->addStretch();

		QPushButton* btnCancel = new QPushButton(dlg);
		btnCancel->setText(tr("Cancel"));
		hlayout->addWidget(btnCancel);
		connect(btnCancel, SIGNAL(clicked()), dlg, SLOT(reject()));

		QPushButton* btnOk = new QPushButton(dlg);
		btnOk->setText(tr("Ok"));
		hlayout->addWidget(btnOk);
		btnOk->setDefault(true);
		connect(btnOk, SIGNAL(clicked()), dlg, SLOT(accept()));

		if(dlg->exec()){
			curName = text->text().toLocal8Bit().constData();
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
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, true);
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
		PrintElementNumbers(obj->get_subset_handler().get_grid_objects_in_subset(si));
	}
}

void RClickMenu_SceneInspector::
hideAllSubsets(){
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, false);
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}

void RClickMenu_SceneInspector::
toggleAllSubsetVisibilities(){
	ISceneObject* obj = m_sceneInspector->getActiveObject();
	if(obj){
		for(int i = 0; i < obj->num_subsets(); ++i){
			obj->set_subset_visibility(i, !obj->subset_is_visible(i));
		}
		obj->visuals_changed();
		m_sceneInspector->refreshView();
	}
}
