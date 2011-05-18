#include "ItemWidget.h"
#include "ui_ItemWidget.h"
#include "GraphicsItem.h"
#include "SelectedProperties.h"
#include "SelectPictureDialog.h"
#include "TextDialog.h"
#include <QDebug>
#include <QFileInfo>

ItemWidget::ItemWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ItemWidget)
{
	ui->setupUi(this);

	ui->horzAlignment->addItem(tr("None"), 0);
	ui->horzAlignment->addItem(tr("Left"), Qt::AlignLeft);
	ui->horzAlignment->addItem(tr("Center"), Qt::AlignHCenter);
	ui->horzAlignment->addItem(tr("Right"), Qt::AlignRight);

	ui->vertAlignment->addItem(tr("None"), 0);
	ui->vertAlignment->addItem(tr("Top"), Qt::AlignTop);
	ui->vertAlignment->addItem(tr("Center"), Qt::AlignVCenter);
	ui->vertAlignment->addItem(tr("Bottom"), Qt::AlignBottom);

	connect(ui->fontHeight, SIGNAL(currentIndexChanged(int)), this, SLOT(fontHeightIndexChanged(int)));
	connect(ui->backColor, SIGNAL(currentIndexChanged(int)), this, SLOT(backColorIndexChanged(int)));
	connect(ui->textColor, SIGNAL(currentIndexChanged(int)), this, SLOT(textColorIndexChanged(int)));
	connect(ui->borderWidth, SIGNAL(valueChanged(int)), this, SLOT(borderWidthValueChanged(int)));
	connect(ui->borderColor, SIGNAL(currentIndexChanged(int)), this, SLOT(borderColorIndexChanged(int)));
	connect(ui->horzAlignment, SIGNAL(currentIndexChanged(int)), this, SLOT(horzAlignIndexChanged(int)));
	connect(ui->vertAlignment, SIGNAL(currentIndexChanged(int)), this, SLOT(vertAlignIndexChanged(int)));
	connect(ui->picture, SIGNAL(clicked()), this, SLOT(selectPicture()));
	connect(ui->removePicture, SIGNAL(clicked()), this, SLOT(clearPicture()));
	connect(ui->pictureMode, SIGNAL(currentIndexChanged(int)), this, SLOT(pictureModeIndexChanged(int)));
	connect(ui->removeText, SIGNAL(clicked()), this, SLOT(clearText()));
	connect(ui->textButton, SIGNAL(clicked()), this, SLOT(editText()));
	connect(ui->padding, SIGNAL(valueChanged(double)), this, SLOT(paddingValueChanged(double)));
	connect(ui->stripeColor, SIGNAL(currentIndexChanged(int)), this, SLOT(stripeColorIndexChanged(int)));
}

ItemWidget::~ItemWidget()
{
	delete ui;
}

void ItemWidget::connectItem(GraphicsItem* item)
{
	connect(ui->text, SIGNAL(textEdited(QString)), item, SLOT(setText(QString)));
	connect(this, SIGNAL(textChanged(QString)), item, SLOT(setText(QString)));
	connect(ui->font, SIGNAL(currentFontChanged(QFont)), item, SLOT(setFont(QFont)));
	connect(this, SIGNAL(fontHeightChanged(quint32)), item, SLOT(setFontHeight(quint32)));
	connect(this, SIGNAL(backColorChanged(QColor)), item, SLOT(setBackColor(QColor)));
	connect(this, SIGNAL(textColorChanged(QColor)), item, SLOT(setTextColor(QColor)));
	connect(this, SIGNAL(borderWidthChanged(qint32)), item, SLOT(setBorderWidth(qint32)));
	connect(this, SIGNAL(borderColorChanged(QColor)), item, SLOT(setBorderColor(QColor)));
	connect(this, SIGNAL(horzAlignChanged(quint32)), item, SLOT(setHorzAlign(quint32)));
	connect(this, SIGNAL(vertAlignChanged(quint32)), item, SLOT(setVertAlign(quint32)));
	connect(this, SIGNAL(pictureChanged(QUrl)), item, SLOT(setPictureUrl(QUrl)));
	connect(this, SIGNAL(pictureModeChanged(quint32)), item, SLOT(setPictureMode(quint32)));
	connect(this, SIGNAL(paddingChanged(qreal)), item, SLOT(setPadding(qreal)));
	connect(ui->autoLines, SIGNAL(toggled(bool)), item, SLOT(setAutoLines(bool)));
	connect(ui->firstLineBold, SIGNAL(toggled(bool)), item, SLOT(setFirstLineBold(bool)));
	connect(ui->stripes, SIGNAL(toggled(bool)), item, SLOT(setStripes(bool)));
	connect(this, SIGNAL(stripeColorChanged(QColor)), item, SLOT(setStripeColor(QColor)));
}

void ItemWidget::update(const QList<GraphicsItem*>& items)
{
	Q_ASSERT(items.count() > 0);

	const SelectedProperties sel(items);

	blockUiSignals(true);

	text = sel.text;
	int pos = ui->text->cursorPosition();
	ui->text->setText(text);
	ui->text->setCursorPosition(pos);
	ui->removeText->setEnabled(text != "");

	if (!sel.fontName.isEmpty())
	ui->font->setCurrentFont(QFont(sel.fontName));
	ui->font->setEditText(sel.fontName);
	ui->fontHeight->setCurrentIndex(sel.fontHeight);
	ui->textColor->setColor(sel.textColor);
	ui->backColor->setColor(sel.backColor);

	ui->horzAlignment->setCurrentIndex(ui->horzAlignment->findData(sel.horzAlign));
	ui->vertAlignment->setCurrentIndex(ui->vertAlignment->findData(sel.vertAlign));
	ui->borderWidth->setValue(sel.borderWidth);
	ui->borderColor->setColor(sel.borderColor);

	if (sel.pictureUrl.isEmpty())
	{
		ui->picture->setText(tr("None"));
		ui->removePicture->setEnabled(false);
		ui->pictureModeLabel->setEnabled(false);
		ui->pictureMode->setEnabled(false);
	}
	else
	{
		ui->picture->setText(QFileInfo(sel.pictureUrl.path()).fileName());
		ui->removePicture->setEnabled(true);
		ui->pictureModeLabel->setEnabled(true);
		ui->pictureMode->setEnabled(true);
	}
	url = sel.pictureUrl;
	ui->pictureMode->setCurrentIndex(sel.pictureMode);
	ui->padding->setValue(sel.padding);
	ui->autoLines->setChecked(sel.autoLines);
	ui->firstLineBold->setChecked(sel.firstLineBold);
	ui->stripes->setChecked(sel.stripes);
	ui->stripeColor->setColor(sel.stripeColor);

	blockUiSignals(false);
}

void ItemWidget::fontHeightIndexChanged(int index)
{
	emit fontHeightChanged(index);
}

void ItemWidget::backColorIndexChanged(int index)
{
	emit backColorChanged(ui->backColor->itemData(index).value<QColor>());
}

void ItemWidget::textColorIndexChanged(int index)
{
	emit textColorChanged(ui->textColor->itemData(index).value<QColor>());
}

void ItemWidget::borderColorIndexChanged(int index)
{
	emit borderColorChanged(ui->borderColor->itemData(index).value<QColor>());
}

void ItemWidget::borderWidthValueChanged(int value)
{
	emit borderWidthChanged(value);
}

void ItemWidget::horzAlignIndexChanged(int index)
{
	emit horzAlignChanged(ui->horzAlignment->itemData(index).value<quint32>());
}

void ItemWidget::vertAlignIndexChanged(int index)
{
	emit vertAlignChanged(ui->vertAlignment->itemData(index).value<quint32>());
}

void ItemWidget::selectPicture()
{
	SelectPictureDialog dlg(this);
	dlg.setUrl(url);
	if (dlg.exec() == QDialog::Accepted)
		emit pictureChanged(dlg.url());
}

void ItemWidget::clearPicture()
{
	emit pictureChanged(QUrl());
}

void ItemWidget::pictureModeIndexChanged(int index)
{
	emit pictureModeChanged(index);
}

void ItemWidget::blockUiSignals(bool block)
{
	ui->text->blockSignals(block);
	ui->font->blockSignals(block);
	ui->fontHeight->blockSignals(block);
	ui->textColor->blockSignals(block);
	ui->backColor->blockSignals(block);
	ui->borderWidth->blockSignals(block);
	ui->borderColor->blockSignals(block);
	ui->horzAlignment->blockSignals(block);
	ui->vertAlignment->blockSignals(block);
	ui->pictureMode->blockSignals(block);
	ui->padding->blockSignals(block);
	ui->autoLines->blockSignals(block);
	ui->firstLineBold->blockSignals(block);
	ui->stripes->blockSignals(block);
	ui->stripeColor->blockSignals(block);
}

void ItemWidget::clearText()
{
	emit textChanged("");
}

void ItemWidget::editText()
{
	TextDialog dlg(text, this);
	if (dlg.exec() == QDialog::Accepted)
		emit textChanged(dlg.text());
}

void ItemWidget::paddingValueChanged(double value)
{
	emit paddingChanged(value);
}

void ItemWidget::stripeColorIndexChanged(int index)
{
	emit stripeColorChanged(ui->stripeColor->itemData(index).value<QColor>());
}
