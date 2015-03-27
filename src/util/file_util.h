// created by Sebastian Reiter
// s.b.reiter@gmail.com

#ifndef __H__PROMESH_file_util
#define __H__PROMESH_file_util

#include <QString>

///	returns the content of a file as a string.
/**	To read from a resource-file, use the following syntax:
 * \code
 * GetFileContent(":/some-file.txt")
 *\endcode*/
QString GetFileContent(QString filename);

bool FileExists(const QString& dirname);

bool EraseDirectory(QString dirName);

///	recursively copies a directory
void CopyDirectory(QString dirName, QString destDirName);

#endif	//__H__PROMESH_file_util
