#include "SelectPictureDialog.h"
#include "ui_SelectPictureDialog.h"
#include <QFileDialog>
#include <QDesktopServices>

SelectPictureDialog::SelectPictureDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SelectPictureDialog)
{
	ui->setupUi(this);

	connect(ui->browse, SIGNAL(clicked()), this, SLOT(browse()));
}

SelectPictureDialog::~SelectPictureDialog()
{
	delete ui;
}

QUrl SelectPictureDialog::url() const
{
	return ui->url->text();
}

void SelectPictureDialog::setUrl(const QUrl& url)
{
	const QString str = url.toString();
	if (str != "picture")
		ui->url->setText(url.toString());
}

void SelectPictureDialog::browse()
{
	const QString folder = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), folder, tr("All Files (*.*)"));
	if (fileName != "")
		ui->url->setText(fileName);
}
