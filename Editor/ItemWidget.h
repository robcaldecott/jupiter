#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include <QWidget>
#include <QList>
#include <QUrl>
#include <QString>

class GraphicsItem;
class QColor;

namespace Ui
{
	class ItemWidget;
}

class ItemWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ItemWidget(QWidget *parent = 0);
	~ItemWidget();

	void connectItem(GraphicsItem* item);
	void update(const QList<GraphicsItem*>& items);

signals:
	void fontChanged(QFont);
	void fontHeightChanged(quint32);
	void backColorChanged(QColor);
	void textColorChanged(QColor);
	void borderColorChanged(QColor);
	void borderWidthChanged(qint32);
	void horzAlignChanged(quint32);
	void vertAlignChanged(quint32);
	void pictureChanged(QUrl);
	void pictureModeChanged(quint32);
	void textChanged(QString);
	void paddingChanged(qreal);
	void stripeColorChanged(QColor);

public slots:
	void selectPicture();

private slots:
	void fontHeightIndexChanged(int index);
	void backColorIndexChanged(int index);
	void textColorIndexChanged(int index);
	void borderColorIndexChanged(int index);
	void borderWidthValueChanged(int value);
	void horzAlignIndexChanged(int index);
	void vertAlignIndexChanged(int index);
	void clearPicture();
	void pictureModeIndexChanged(int index);
	void clearText();
	void editText();
	void paddingValueChanged(double value);
	void stripeColorIndexChanged(int index);

private:
	void blockUiSignals(bool block);

	Ui::ItemWidget *ui;
	QUrl url;
	QString text;
};

#endif // ITEMWIDGET_H
