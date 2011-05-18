#include "TextTests.h"
#include "Text.h"

namespace
{
    QString generateData(const int rows, const int cols, const int pages, const bool ff)
    {
	QString str;

	for (int page = 0; page < pages; ++page)
	{
	    for (int row = 0; row < rows; ++row)
	    {
		for (int col = 0; col < cols; ++col)
		{
		    str += 'X';
		}
		str += '\n';
	    }
	    if (ff)
	    {
		str += '\f';
	    }
	}

	return str;
    }
}

void TextTests::pageSizeLf()
{
    QVERIFY(Text::pageSize("TEST") == QSize(80, 66));
    QVERIFY(Text::pageSize("1\n22\n333") == QSize(80, 66));
    QVERIFY(Text::pageSize("1\n22\n333\n") == QSize(80, 66));
}

void TextTests::pageSizeCr()
{
    QVERIFY(Text::pageSize("1\r22\r333") == QSize(80, 66));
}

void TextTests::pageSizeCrLf()
{
    QVERIFY(Text::pageSize("1\r\n22\r\n333") == QSize(80, 66));
}

void TextTests::pageSizeNoFormFeeds()
{
    const QString str = generateData(66, 80, 1, false);
    QVERIFY(Text::pageSize(str) == QSize(80, 66));
}

void TextTests::pageSizeFormFeeds()
{
    const QString str = generateData(66, 132, 1, true);
    QVERIFY(Text::pageSize(str) == QSize(132, 66));
}

void TextTests::expandTabs()
{
    QCOMPARE(Text::expandTabs("\tHello"), QString("        Hello"));
}
