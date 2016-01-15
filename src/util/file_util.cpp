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

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "file_util.h"
#include "common/error.h"
#include "common/log.h"

using namespace std;

QString GetFileContent(QString filename)
{
	QFile inputFile(filename);
	inputFile.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream in(&inputFile);
	return in.readAll();
}

bool FileExists(const QString& dirname)
{
	QFileInfo dir(dirname);
	return dir.exists();
}

bool EraseDirectory(QString dirName)
{
	bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = EraseDirectory(info.absoluteFilePath());
            }
            else {
            	QFile file(info.absoluteFilePath());
            	file.setPermissions(QFile::ReadOther | QFile::WriteOther);
				result = file.remove();
                // result = QFile::remove(info.absoluteFilePath());
                if(!result){
                	UG_LOG("Failed to remove file: " << info.absoluteFilePath().toStdString() << endl);
                }
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
        if(!result){
        	UG_LOG("Failed to remove directory: " << dirName.toStdString() << endl);
        }
    }
    return result;
}

void CopyDirectory(QString dirName, QString destDirName)
{
	QFileInfo dirInfo(dirName);
	QFileInfo destDirInfo(destDirName);

	UG_COND_THROW(!destDirInfo.isDir(), "Target is not a directory!");

	QDir destDir(destDirName);

	if(!destDir.mkdir(dirInfo.fileName())){
		UG_THROW("Couldn't create " << dirInfo.fileName().toStdString()
				<< " in directory " << destDirInfo.fileName().toStdString());
	}

	QString newDestDirName = QString(destDirName).append("/").append(dirInfo.fileName());
	QDir dir(dirName);
	QStringList dirs = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);

	foreach(const QString& d, dirs){
		QString childDirName = QString(dirName).append("/").append(d);
		CopyDirectory(childDirName, newDestDirName);
	}

	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Files);
	foreach(const QString& f, files){
		QString fromFile = QString(dirName).append("/").append(f);
		QString toFile = QString(newDestDirName).append("/").append(f);
		if(!QFile::copy(fromFile, toFile)){
			UG_THROW("Couldn't copy file " << fromFile.toStdString()
					 << " to file " << toFile.toStdString());
		}
	}
}