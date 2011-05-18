#include "Page.h"

Page::Page(QObject *parent) :
	QObject(parent)
{
}

Page::Page(const Page& page)
{

}

Page& Page::operator=(const Page& rhs)
{
	Q_UNUSED(rhs);
	return *this;
}
