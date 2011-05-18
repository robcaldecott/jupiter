#include <QtGui/QApplication>
#include <QPrintDialog>
#include <QDebug>
#include <QMessageBox>
#include "MainWindow.h"
#include "PrintDialog.h"
#include "PrintDefault.h"
#include "CommandLine.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName("Jupiter Reporter");
    a.setOrganizationName("XE-QT Solutions");
    a.setOrganizationDomain("xe-qt-solutions.com");

    CommandLine cmd(a.arguments());

    // Get the filename to load
    const QString fileName = cmd.fileName();

    if (fileName != "")
    {
	if (cmd.printDefault())
	{
	    PrintDefault printDefault;
	    QObject::connect(&printDefault, SIGNAL(finished()), &a, SLOT(quit()));
	    printDefault.print(fileName);
	    return a.exec();
	}

	if (cmd.print())
	{
	    a.setQuitOnLastWindowClosed(false);

	    PrintDialog printDialog;
	    QObject::connect(&printDialog, SIGNAL(finished()), &a, SLOT(quit()));
	    printDialog.print(fileName, cmd.printer());
	    return a.exec();
	}
    }

    MainWindow w;
    w.load(fileName);
    w.show();

    return a.exec();
}
