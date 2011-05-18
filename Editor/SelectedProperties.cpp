#include "SelectedProperties.h"
#include "GraphicsItem.h"

SelectedProperties::SelectedProperties(const QList<GraphicsItem*>& items) :
	bold(false),
	italic(false),
	fontHeight(-1),
	horzAlign(-1),
	vertAlign(-1),
	borderWidth(-1),
	box(false),
	pictureMode(-1),
	padding(0),
	autoLines(false),
	firstLineBold(false),
	stripes(false)
{
	if (items.count() > 0)
	{
		const GraphicsItem* item = items.first();

		text = item->text();

		QFont font = item->font();
		fontName = font.family();
		bold = font.bold();
		italic = font.italic();
		fontHeight = item->fontHeight();

		textColor = item->textColor();
		backColor = item->backColor();
		horzAlign = item->horzAlign();
		vertAlign = item->vertAlign();
		borderWidth = item->borderWidth();
		borderColor = item->borderColor();
		box = item->box();
		pictureUrl = item->pictureUrl();
		pictureMode = item->pictureMode();
		padding = item->padding();
		autoLines = item->autoLines();
		firstLineBold = item->firstLineBold();
		stripes = item->stripes();
		stripeColor = item->stripeColor();

		for (int i = 1; i < items.count(); ++i)
		{
			item = items.at(i);

			if (item->text() != text)
				text = "";

			font = item->font();

			if (font.family() != fontName)
				fontName = "";

			if (font.bold() != bold)
				bold = true;

			if (font.italic() != italic)
				italic = true;

			if (item->fontHeight() != fontHeight)
				fontHeight = -1;

			if (item->textColor() != textColor)
				textColor = QColor();

			if (item->backColor() != backColor)
				backColor = QColor();

			if (item->horzAlign() != horzAlign)
				horzAlign = -1;

			if (item->vertAlign() != vertAlign)
				vertAlign = -1;

			if (item->borderWidth() != borderWidth)
				borderWidth = 0;

			if (item->borderColor() != borderColor)
				borderColor = QColor();

			if (item->box() != box)
				box = true;

			if (item->pictureUrl() != pictureUrl)
				pictureUrl.clear();

			if (item->pictureMode() != pictureMode)
				pictureMode = -1;

			if (!qFuzzyCompare(item->padding(), padding))
				padding = 0;

			if (item->autoLines() != autoLines)
				autoLines = true;

			if (item->firstLineBold() != firstLineBold)
				firstLineBold = true;

			if (item->stripes() != stripes)
				stripes = true;

			if (item->stripeColor() != stripeColor)
				stripeColor = QColor();
		}
	}
}
