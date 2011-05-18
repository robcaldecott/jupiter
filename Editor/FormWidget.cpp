#include "FormWidget.h"
#include "ui_FormWidget.h"
#include "Form.h"

FormWidget::FormWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::FormWidget)
{
	ui->setupUi(this);

	connect(ui->rows, SIGNAL(valueChanged(int)), this, SLOT(rowsChanged(int)));
	connect(ui->cols, SIGNAL(valueChanged(int)), this, SLOT(colsChanged(int)));
}

FormWidget::~FormWidget()
{
	delete ui;
}

void FormWidget::connectForm(const Form* form)
{
	Q_ASSERT(form);

	connect(ui->font, SIGNAL(currentFontChanged(QFont)), form, SLOT(setFont(QFont)));
	connect(ui->watermark, SIGNAL(textChanged(QString)), form, SLOT(setWatermark(QString)));
	connect(this, SIGNAL(setRows(qint32)), form, SLOT(setRows(qint32)));
	connect(this, SIGNAL(setCols(qint32)), form, SLOT(setCols(qint32)));
}

void FormWidget::update(const Form* form)
{
	Q_ASSERT(form);

	ui->rows->setValue(form->rows());
	ui->cols->setValue(form->cols());
	ui->font->setCurrentFont(form->font());
	ui->watermark->setText(form->watermark());
}

void FormWidget::rowsChanged(int rows)
{
	emit setRows(rows);
}

void FormWidget::colsChanged(int cols)
{
	emit setCols(cols);
}
