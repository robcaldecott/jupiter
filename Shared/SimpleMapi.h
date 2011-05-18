#ifndef SIMPLEMAPI_H
#define SIMPLEMAPI_H

#include <QScopedPointer>

class QString;

class Mapi
{
private:
	Mapi();

public:
	~Mapi();
	static Mapi& instance();
	bool isAvailable();
	bool send(const QString& fileName, const QString& subject, const QString& name);

private:
	struct Impl;
	QScopedPointer<Impl> impl;
};

#endif // SIMPLEMAPI_H
