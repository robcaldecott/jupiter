#ifndef SELECTPICTUREDIALOG_H
#define SELECTPICTUREDIALOG_H

#include <QDialog>
#include <QUrl>

namespace Ui
{
	class SelectPictureDialog;
}

class SelectPictureDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SelectPictureDialog(QWidget *parent = 0);
	~SelectPictureDialog();

	QUrl url() const;
	void setUrl(const QUrl& url);

private slots:
	void browse();

private:
	Ui::SelectPictureDialog *ui;
};

#endif // SELECTPICTUREDIALOG_H
