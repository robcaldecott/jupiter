#ifndef GRAPHICSITEMTYPES_H
#define GRAPHICSITEMTYPES_H

#include <QGraphicsItem>

namespace GraphicsItemTypes
{
	enum Type
	{
		Paper = QGraphicsItem::UserType + 1,
		Page,
		Watermark,
		Grid,
		Data,
		Item,
	};
}

#endif // GRAPHICSITEMTYPES_H
