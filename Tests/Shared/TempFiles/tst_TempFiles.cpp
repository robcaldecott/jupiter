#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QTemporaryFile>
#include <QDir>
#include "TempFiles.h"

class tst_TempFiles : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void add();
};

void tst_TempFiles::add()
{
	// Create temp files
	const int FileCount = 3;

	QFile files[FileCount];
	QString fileNames[FileCount];

	for (int i = 0; i < FileCount; ++i)
	{
		files[i].setFileName(QString("%1/test%2").arg(QDir::tempPath()).arg(i));
		QVERIFY(files[i].open(QIODevice::WriteOnly));
		files[i].write("Test");
		files[i].close();
		fileNames[i] = files[i].fileName();
	}

	{
		// Add each file to the temp list
		TempFiles tempFiles;
		for (int i = 0; i < FileCount; ++i)
			tempFiles.add(fileNames[i]);
	}

	// Check the files have all been removed
	for (int i = 0; i < FileCount; ++i)
		QVERIFY(!QFile::exists(fileNames[i]));
}

QTEST_APPLESS_MAIN(tst_TempFiles);

#include "tst_TempFiles.moc"
