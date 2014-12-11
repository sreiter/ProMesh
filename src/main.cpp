//	created by Sebastian Reiter
//	s.b.reiter@googlemail.com
//	y09 m09 d29

#include <QApplication>
#include <iostream>
#include <clocale>
#include <QFileOpenEvent>
#include "app.h"
#include "util/file_util.h"

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


	myApp.setStyleSheet(GetFileContent(":/styles/promesh_style.txt"));
	//myApp.setStyleSheet(GetFileContent("/home/sreiter/projects/ProMesh/trunk/styles/promesh_style.txt"));

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
	pMainWindow->setWindowTitle("ProMesh 4.2.0*");

	//pMainWindow->resize(1024, 768);
	pMainWindow->show();

	setlocale(LC_NUMERIC, "C");

	for(int i = 1; i < argc; ++i){
		pMainWindow->load_grid_from_file(argv[i]);
	}

	return myApp.exec();
}
