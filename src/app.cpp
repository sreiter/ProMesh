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
#include <QFileInfo>
#include <QTextStream>
#include "app.h"
#include "util/file_util.h"
#include "common/math/ugmath.h"

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

QDir AppDir()
{
	return QDir(QApplication::applicationDirPath());
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

std::vector<QDir> CustomUserScriptDirs()
{
	std::vector<QDir> vCustomUserScriptDirs;

	if(UserDataDir().exists("custom_user_script_dirs")){
		QFile inFile((app::UserDataDir().absoluteFilePath("custom_user_script_dirs")));

		if (inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
		   QTextStream inStr(&inFile);

		   while(!inStr.atEnd())
		   {
			   QString line = inStr.readLine();
			   if(!line.isEmpty()){
				   QDir currentCustomUserScriptDir;
				   currentCustomUserScriptDir.setPath(line);

			   //  Path permission check
				   QString pathName(currentCustomUserScriptDir.dirName());
				   currentCustomUserScriptDir.cdUp();
				   CheckPathPermissions(currentCustomUserScriptDir, pathName);

				   currentCustomUserScriptDir.cd(pathName);

				   vCustomUserScriptDirs.push_back(currentCustomUserScriptDir);
			   }
		   }

		   inFile.close();
		}
	}

	return vCustomUserScriptDirs;
}

QDir UserTmpDir()
{
	QDir userPath = UserDataDir();
	QString pathName("tmp");

	if(!userPath.exists(pathName)){
		userPath.mkdir(pathName);
	}

	CheckPathPermissions(userPath, pathName);

	userPath.cd(pathName);

	return userPath;
}

QDir UserHelpDir()
{
	QDir userPath = UserDataDir();
	QString pathName("help");

	if(!userPath.exists(pathName)){
		userPath.mkdir(pathName);
	}

	CheckPathPermissions(userPath, pathName);

	userPath.cd(pathName);

	return userPath;
}

QDir ProMeshTmpDir()
{
	QDir tmpPath(QDir::tempPath());
	if(!tmpPath.exists("ProMesh"))
		tmpPath.mkdir("ProMesh");
	tmpPath.cd("ProMesh");
	return tmpPath;
}

/// returns a unique temporary file name placed in ProMeshTmpDir
QString TmpFileName(const QString& prefix, const QString& suffix)
{
	return TmpFileName(ProMeshTmpDir(), prefix, suffix);
}

/// returns a unique temporary file name placed in the given directory
QString TmpFileName(const QDir& dir, const QString& prefix, const QString& suffix)
{
	for(int i = 0; i < 1000; ++i){
		QString filename = dir.path();
		filename.append(QDir::separator())
				.append(prefix)
				.append(QString::number(ug::urand<int>(100000, 999999)))
				.append(suffix);
		if(!QFile::exists(filename))
			return filename;
	}

	UG_THROW("No temporary file could be created.");
	return QString("");
}


QString GetVersionString()
{
	return GetFileContent(QString(":/version.txt"));
}

}// end of namespace
