#include "TempFiles.h"
#include <QFile>

TempFiles::TempFiles()
{
}

TempFiles::~TempFiles()
{
    foreach (const QString& fileName, files)
    {
	QFile::remove(fileName);
    }
}

void TempFiles::add(const QString& fileName)
{
    files << fileName;
}
