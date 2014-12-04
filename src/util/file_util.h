// created by Sebastian Reiter
// s.b.reiter@gmail.com

#ifndef __H__PROMESH_file_util
#define __H__PROMESH_file_util

#include <QString>
#include <QFile>
#include <QTextStream>

///	returns the content of a file as a string.
/**	To read from a resource-file, use the following syntax:
 * \code
 * GetFileContent(":/some-file.txt")
 *\endcode*/
inline QString GetFileContent(QString filename)
{
	QFile inputFile(filename);
	inputFile.open(QIODevice::ReadOnly);
	QTextStream in(&inputFile);
	return in.readAll();
}

#endif	//__H__PROMESH_file_util
