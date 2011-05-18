#ifndef MARGINSF_H
#define MARGINSF_H

#include <QtGlobal>

class QDataStream;

class MarginsF
{
public:
	friend QDataStream& operator<<(QDataStream&, const MarginsF&);
	friend QDataStream& operator>>(QDataStream&, MarginsF&);

	MarginsF();
	MarginsF(qreal left, qreal top, qreal right, qreal bottom);

	qreal left() const;
	qreal top() const;
	qreal right() const;
	qreal bottom() const;

private:
	qreal left_;
	qreal top_;
	qreal right_;
	qreal bottom_;
};

#endif // MARGINSF_H
