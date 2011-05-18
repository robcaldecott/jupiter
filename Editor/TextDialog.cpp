#include "TextDialog.h"
#include "ui_TextDialog.h"

TextDialog::TextDialog(const QString& str, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::TextDialog)
{
	ui->setupUi(this);
	ui->text->setPlainText(str);
}

QString TextDialog::text() const
{
	return ui->text->toPlainText();
}
