#ifndef PAGE_H
#define PAGE_H

#include <QObject>

class Page : public QObject
{
	Q_OBJECT
public:
	explicit Page(QObject *parent = 0);
	Page(const Page& page);

	Page& operator=(const Page& rhs);

signals:

public slots:

};

#endif // PAGE_H
