#ifndef FILE_H
#define FILE_H

class QString;
class QByteArray;

namespace File
{
	/// Load the contents of a file into a string, assume UTF-8 encoding
	QString loadText(const QString& fileName);
	/// Load the contents of a file into a QByteArray
	QByteArray loadBytes(const QString& fileName);
}

#endif // FILE_H
