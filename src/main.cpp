//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d29

#include <QApplication>
#include <iostream>
#include <clocale>
#include <QFileOpenEvent>
#include "app.h"

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
	pMainWindow->init();

	myApp.setMainWindow(pMainWindow);
	pMainWindow->setWindowTitle("ProMesh 4.0.7");

	//pMainWindow->resize(1024, 768);
	pMainWindow->show();

	setlocale(LC_NUMERIC, "C");

	UG_LOG("ProMesh4 - created by Sebastian Reiter (s.b.reiter@googlemail.com).\n");
	UG_LOG("This version of ProMesh4 is for non-commercial use only. See \"Help->License\" for more information.\n");
	UG_LOG("If you use ProMesh to create geometries for your publications, make sure to cite it!\n");
	UG_LOG("ProMesh uses libGrid, which is part of the ug4 simulation framework.\n");
	UG_LOG("This version of ProMesh4 uses 'tetgen' by Hang Si for tetrahedral mesh generation (in Remeshing/Tetgen).\n");
	UG_LOG("--------------------------------------------------------------\n\n");

	for(int i = 1; i < argc; ++i){
		pMainWindow->load_grid_from_file(argv[i]);
	}

	return myApp.exec();
}
