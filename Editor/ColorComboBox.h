#ifndef COLORCOMBOBOX_H
#define COLORCOMBOBOX_H

#include <QComboBox>

class QColor;
class QString;

class ColorComboBox : public QComboBox
{
	Q_OBJECT

public:
	explicit ColorComboBox(QWidget *parent = 0);

public slots:
	void setColor(const QColor& color);

private:
	void addColor(const QColor& color, const QString& name);
};

#endif // COLORCOMBOBOX_H
