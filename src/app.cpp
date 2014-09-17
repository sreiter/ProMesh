//	created by Sebastian Reiter
//	s.b.reiter@gmail.com

#include <QFileInfo>
#include "app.h"

namespace app{

static void CheckPathPermissions(const QDir& dir, QString path)
{
	QFileInfo info(dir, path);
	UG_COND_THROW(!info.isDir(),
				  "ERROR: bad app-user-data path: "
				  << dir.dirName().toLocal8Bit().constData()
				  << path.toLocal8Bit().constData() << " has to be a directory!");

	UG_COND_THROW(!info.isReadable(),
				  "ERROR: app-user-data path has to be readable: "
				  << dir.dirName().toLocal8Bit().constData()
				  << path.toLocal8Bit().constData());

	UG_COND_THROW(!info.isWritable(),
				  "ERROR: app-user-data path has to be writable: "
				  << dir.dirName().toLocal8Bit().constData()
				  << path.toLocal8Bit().constData());
}


QDir UserDataDir()
{
	QDir homeDir = QDir::home();
	QString pathName(".promesh");

	if(!homeDir.exists(pathName)){
		homeDir.mkdir(pathName);
	}

	CheckPathPermissions(homeDir, pathName);

	homeDir.cd(pathName);


	return homeDir;
}

QDir UserScriptDir()
{
	QDir userPath = UserDataDir();
	QString pathName("scripts");

	if(!userPath.exists(pathName)){
		userPath.mkdir(pathName);
	}

	CheckPathPermissions(userPath, pathName);

	userPath.cd(pathName);

	return userPath;
}

}// end of namespace
