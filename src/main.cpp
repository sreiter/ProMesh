/*
 * Copyright (c) 2008-2015:  G-CSC, Goethe University Frankfurt
 * Copyright (c) 2006-2008:  Steinbeis Forschungszentrum (STZ Ölbronn)
 * Copyright (c) 2006-2015:  Sebastian Reiter
 * Author: Sebastian Reiter
 *
 * This file is part of ProMesh.
 * 
 * ProMesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License version 3 (as published by the
 * Free Software Foundation) with the following additional attribution
 * requirements (according to LGPL/GPL v3 §7):
 * 
 * (1) The following notice must be displayed in the Appropriate Legal Notices
 * of covered and combined works: "Based on ProMesh (www.promesh3d.com)".
 * 
 * (2) The following bibliography is recommended for citation and must be
 * preserved in all covered files:
 * "Reiter, S. and Wittum, G. ProMesh -- a flexible interactive meshing software
 *   for unstructured hybrid grids in 1, 2, and 3 dimensions. In preparation."
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 */

#include <QApplication>
#include <iostream>
#include <clocale>
#include <QFileOpenEvent>
#include "app.h"
#include "util/file_util.h"

//TESTING
#include <QDialog>
#include <QVBoxLayout>
// #include "widgets/tooldlg_oarchive.h"
// #include "widgets/tooldlg_iarchive.h"
// #include "sera_test.h"
// #include <boost/archive/xml_oarchive.hpp>
// #include "widgets/property_widget.h"
// #include "common/error.h"
// #include "common/log.h"
// #include "lib_grid/algorithms/refinement/refinement_projectors_old/cylinder_projector.h"
// #include "lib_grid/algorithms/refinement/projectors/cylinder_projector.h"
// #include "lib_grid/boost_class_serialization_exports.h"

using namespace std;

class MyApplication : public QApplication
{
	public:
		MyApplication(int & argc, char ** argv) :
			QApplication(argc, argv), m_pMainWindow(NULL)	{}

		virtual bool	event ( QEvent * e )
		{
			if(e->type() == QEvent::FileOpen){
				QFileOpenEvent* foe = dynamic_cast<QFileOpenEvent*>(e);
				if(foe){
					QString str = foe->file();
					if(m_pMainWindow){
						if(m_pMainWindow->load_grid_from_file(str.toLocal8Bit().constData()))
						{
							m_pMainWindow->settings().setValue("file-path",
												QFileInfo(str).absolutePath());
						}
					}
					return true;
				}
			}
			return QApplication::event(e);
		}

		void setMainWindow(MainWindow* win){
			m_pMainWindow = win;
		}

	private:
		MainWindow* m_pMainWindow;
};


int main(int argc, char *argv[])
{
	cout.sync_with_stdio(true);

	//QApplication app(argc, argv);
	MyApplication myApp(argc, argv);
	myApp.setQuitOnLastWindowClosed(true);


	QString qss = GetFileContent(":/styles/promesh_style.css");
	QString varsStr = GetFileContent(":/styles/dark_theme_variables.txt");
	// QString qss = GetFileContent("C:\\Users\\sreiter\\projects\\ProMesh\\ProMesh\\styles\\promesh_style.css");
	// QString varsStr = GetFileContent("C:\\Users\\sreiter\\projects\\ProMesh\\ProMesh\\styles\\dark_theme_variables.txt");
	// QString qss = GetFileContent("/home/sreiter/projects/ProMesh/ProMesh/styles/promesh_style.css");
	// QString varsStr = GetFileContent("/home/sreiter/projects/ProMesh/ProMesh/styles/dark_theme_variables.txt");
	QStringList varsList = varsStr.split(QRegularExpression("[\r\n]"),QString::SkipEmptyParts);
	QRegularExpression regVar("\\s*(@\\w+)\\s*(.+)");
	QMap<QString, QString> varMap;
	for(QStringList::Iterator iter = varsList.begin(); iter != varsList.end(); ++iter){
		QRegularExpressionMatch match = regVar.match(*iter);
		if(match.hasMatch()){
			varMap[match.captured(1)] = match.captured(2);
		}
	}
	
	QMapIterator<QString, QString> mapIter(varMap);
	mapIter.toBack();
	while(mapIter.hasPrevious()){
		mapIter.previous();
		qss.replace(mapIter.key(), mapIter.value());
	}

	myApp.setStyleSheet(qss);

/*
	if(!QGLFormat::hasOpenGL())
	{
		cerr << "System has no openGL support!" << endl;
		return 1;
	}
*/
	//UG_SET_DEBUG_LEVEL(LIB_GRID, 2);
//	ug::script::Initialize();
//	ug::script::InitLibGridScript(ug::script::GetLuaState());

	MainWindow* pMainWindow = app::getMainWindow();

	// try{
	// 	SeraTest2 test;
	// 	SeraBase* base = &test;
	// 	test.m_properties.m_pos = ug::vector3(2.2, 1.1, 3.3);
	// 	test.m_int = 1;

	// 	QDialog* dlg = new QDialog(pMainWindow);
	// 	QVBoxLayout* layout = new QVBoxLayout(dlg);
	// 	dlg->setLayout(layout);

	// 	PropertyWidget* pw = new PropertyWidget(dlg);
	// 	layout->addWidget(pw);

	// 	pw->populate(base, "SeraTest");

	// 	dlg->resize(QSize(400, 800));
	// 	dlg->show();


	// 	SeraTest2 testOut;
	// 	pw->retrieve_values(testOut);
	// 	UG_LOG("Retrieved vector " << testOut.m_properties.m_pos << std::endl);
	// 	UG_LOG("Retrieved int " << testOut.m_int << std::endl);
	// }
	// catch(ug::UGError& err){
	// 	UG_LOG("ERROR received during widget serialization:\n");
	// 	for(size_t i = 0; i < err.num_msg(); ++i)
	// 		UG_LOG("  " << err.get_file(i) << ": " << err.get_line(i)
	// 			   << ": " << err.get_msg(i) << "\n");
	// }
	// catch(...){
	// 	UG_LOG("  Unknown error...\n");
	// }


	// try{
	// 	QDialog* dlg = new QDialog(pMainWindow);
	// 	QVBoxLayout* layout = new QVBoxLayout(dlg);
	// 	dlg->setLayout(layout);

	// 	PropertyWidget* pw = new PropertyWidget(dlg);
	// 	layout->addWidget(pw);

	// 	ug::CylinderProjectorNew cylProj;
	// 	ug::RefinementProjector& base = cylProj;
	// 	pw->populate(base, "Projector");

	// 	dlg->resize(QSize(400, 800));
	// 	dlg->show();

	// }
	// catch(ug::UGError& err){
	// 	UG_LOG("ERROR received during widget serialization:\n");
	// 	for(size_t i = 0; i < err.num_msg(); ++i)
	// 		UG_LOG("  " << err.get_file(i) << ": " << err.get_line(i)
	// 			   << ": " << err.get_msg(i) << "\n");
	// }
	// catch(...){
	// 	UG_LOG("  Unknown error...\n");
	// }


//	testing...
	// QDialog* dlg = new QDialog(pMainWindow);
	// ProjectorWidget* pw = new ProjectorWidget(dlg);
	// QVBoxLayout* layout = new QVBoxLayout(dlg);
	// dlg->setLayout(layout);
	// layout->addWidget(pw);
	// dlg->resize(QSize(400, 800));


	pMainWindow->init();

	myApp.setMainWindow(pMainWindow);
	pMainWindow->setWindowTitle(QString("ProMesh ").append(app::GetVersionString()).append("   (www.promesh3d.com)"));

	pMainWindow->show();

	setlocale(LC_NUMERIC, "C");

	for(int i = 1; i < argc; ++i){
		if(pMainWindow->load_grid_from_file(argv[i])){
			pMainWindow->settings().setValue("file-path",
											 QFileInfo(argv[i]).absolutePath());
		}
	}

	// dlg->show();

	#ifdef UG_DEBUG
		UG_LOG("DEBUG MODE\n");
	#endif
	#ifdef UG_ENABLE_DEBUG_LOGS
		UG_LOG("DEBUG LOGS ACTIVE\n");
		UG_SET_DEBUG_LEVEL(ug::LIB_GRID, 1);
	#endif


	return myApp.exec();
}
