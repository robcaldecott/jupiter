#include "File.h"
#include <QtGlobal>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

/// Load the contents of a file into a string, assume UTF-8 encoding
QString File::loadText(const QString& fileName)
{
    Q_ASSERT(fileName != "");

    QString str;

    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	str = stream.readAll();
    }

    return str;
}

/// Load the contents of a file into a QByteArray
QByteArray File::loadBytes(const QString& fileName)
{
    Q_ASSERT(fileName != "");

    QByteArray bytes;
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
	bytes = file.readAll();
    }

    return bytes;
}
