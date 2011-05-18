#ifndef FORMWIDGET_H
#define FORMWIDGET_H

#include <QWidget>

class Form;

namespace Ui
{
	class FormWidget;
}

class FormWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FormWidget(QWidget *parent = 0);
	~FormWidget();

	void connectForm(const Form* form);
	void update(const Form* form);

signals:
	void setRows(qint32);
	void setCols(qint32);

private slots:
	void rowsChanged(int rows);
	void colsChanged(int cols);

private:
	Ui::FormWidget *ui;
};

#endif // FORMWIDGET_H
