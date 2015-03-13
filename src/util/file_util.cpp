// created by Sebastian Reiter
// s.b.reiter@gmail.com

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "file_util.h"
#include "common/error.h"
#include "common/log.h"

QString GetFileContent(QString filename)
{
	QFile inputFile(filename);
	inputFile.open(QIODevice::ReadOnly);
	QTextStream in(&inputFile);
	return in.readAll();
}

bool FileExists(const QString& dirname)
{
	QFileInfo dir(dirname);
	return dir.exists();
}

void EraseDirectory(QString dirName)
{
	QDir dir(dirName);
	dir.removeRecursively();
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