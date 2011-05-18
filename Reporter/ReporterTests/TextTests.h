#ifndef TEXTTESTS_H
#define TEXTTESTS_H

#include "AutoTest.h"

class TextTests : public QObject
{
    Q_OBJECT

private slots:
    void pageSizeLf();
    void pageSizeCr();
    void pageSizeCrLf();
    void pageSizeNoFormFeeds();
    void pageSizeFormFeeds();
    void expandTabs();
};

DECLARE_TEST(TextTests)

#endif // TEXTTESTS_H
