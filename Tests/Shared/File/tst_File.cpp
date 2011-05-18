#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QTemporaryFile>
#include "File.h"

class tst_File : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void loadText();
	void loadBytes();
};

void tst_File::loadText()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	const QString fileName = file.fileName();
	file.write("Hello, world!");
	file.close();

	QCOMPARE(File::loadText(fileName), QString("Hello, world!"));
}

void tst_File::loadBytes()
{
	QTemporaryFile file;
	QVERIFY(file.open());
	const QString fileName = file.fileName();
	QByteArray bytes;
	for (int i = 0; i < 256; i++)
		bytes.append((char)i);
	file.write(bytes);
	file.close();

	QCOMPARE(File::loadBytes(fileName), bytes);
}

QTEST_APPLESS_MAIN(tst_File);

#include "tst_File.moc"
