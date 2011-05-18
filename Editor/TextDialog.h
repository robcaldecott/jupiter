#ifndef TEXTDIALOG_H
#define TEXTDIALOG_H

#include <QDialog>

class QString;

namespace Ui
{
	class TextDialog;
}

class TextDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TextDialog(const QString& str, QWidget *parent = 0);
	QString text() const;

private:
	Ui::TextDialog* ui;
};

#endif // TEXTDIALOG_H
