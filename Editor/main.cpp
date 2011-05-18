#include <QtGui/QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	a.setApplicationName("Jupiter Editor");
	a.setOrganizationName("Genesistems");
	a.setOrganizationDomain("genesistems.com");

	MainWindow w;
	w.show();

	return a.exec();
}
