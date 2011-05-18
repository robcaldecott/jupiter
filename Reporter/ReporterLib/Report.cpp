#include "Report.h"
#include "Text.h"
#include "Printer.h"
#include "StringList.h"
#include "DownloadFile.h"
#include "DownloadPictures.h"
#include <QDebug>
#include <QList>
#include <QPair>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace
{
    namespace Command
    {
	enum Type
	{
	    Unknown,
	    Font,
	    StretchFont,
	    Rows,
	    Cols,
	    Lpi,
	    Cpi,
	    FontColor,
	    Bold,
	    Stripes,
	    StripeColor,
	    Units,
	    Margins,
	    PaperSize,
	    Orientation,
	    AutoLandscape,
	    AutoLandscapeCol,
	    Picture,
	};

	struct Info
	{
	    Type type;
	    QString name;
	} commands[] =
	{
	    { Font, "font" },
	    { StretchFont, "stretchfont" },
	    { Rows, "rows" },
	    { Cols, "cols" },
	    { Lpi, "lpi" },
	    { Cpi, "cpi" },
	    { FontColor, "fontcolor" },
	    { Bold, "bold" },
	    { Stripes, "stripes" },
	    { StripeColor, "stripecolor" },
	    { Units, "units" },
	    { Margins, "margins" },
	    { PaperSize, "papersize" },
	    { Orientation, "orientation" },
	    { AutoLandscape, "autolandscape" },
	    { AutoLandscapeCol, "autolandscapecol" },
	    { Picture, "picture" },
	};

	QMap<QString, Type> commandMap;

	Type stringToType(const QString& str)
	{
	    if (commandMap.isEmpty())
	    {
		for (unsigned int i = 0; i < sizeof(commands) / sizeof(*commands); ++i)
		{
		    commandMap[commands[i].name] = commands[i].type;
		}
	    }

	    QMap<QString, Type>::const_iterator it = commandMap.find(str.toLower());
	    return it != commandMap.end() ? it.value() : Unknown;
	}
    }
}

Report::Report(QPrinter* printer, QObject *parent) :
    QObject(parent),
    printer_(printer),
    rows_(0),
    cols_(0),
    font_("Lucida Console"),
    stretchFont_(true),
    lpi_(6),
    cpi_(12),
    fontColor_(Qt::black),
    bold_(false),
    stripes_(false),
    stripeColor_(QColor(230, 230, 230)),
    unit_(QPrinter::Inch),
    orientation_(printer->orientation()),
    autoLandscape_(false),
    autoLandscapeCol_(80),
    margins_(Printer::margins(printer, QPrinter::Inch)),
    paperSize_(printer->paperSize(QPrinter::Inch))
{
}

Report& Report::operator=(const Report& report)
{
    rows_ = report.rows_;
    cols_ = report.cols_;
    font_ = report.font_;
    stretchFont_ = report.stretchFont_;
    lpi_ = report.lpi_;
    cpi_ = report.cpi_;
    fontColor_ = report.fontColor_;
    bold_ = report.bold_;
    stripes_ = report.stripes_;
    stripeColor_ = report.stripeColor_;
    unit_ = report.unit_;
    orientation_ = report.orientation_;
    autoLandscape_ = report.autoLandscape_;
    autoLandscapeCol_ = report.autoLandscapeCol_;
    margins_ = report.margins_;
    paperSize_ = report.paperSize_;
    pictures_ = report.pictures_;
    return *this;
}

bool Report::isEmpty() const
{
    return data_.isEmpty();
}

void Report::clear()
{
    *this = Report(printer_, parent());
}

QString Report::data() const
{
    return data_;
}

void Report::setData(const QString& data)
{
    if (data == data_)
    {
	return;
    }

    clear();
    data_ = data;
    processCommands();

    // Count the number of rows/cols
    if (rows_ == 0 || cols_ == 0)
    {
	const QSize size = Text::pageSize(data_);
	rows_ = size.height();
	cols_ = size.width();
    }

    // Load pictures
    if (!pictures_.isEmpty())
    {
	DownloadPictures* download = new DownloadPictures(this);
	connect(download, SIGNAL(downloaded(QUrl,QPixmap)), this, SLOT(pictureDownloaded(QUrl,QPixmap)));
	connect(download, SIGNAL(finished()), this, SIGNAL(loaded()));
	connect(download, SIGNAL(finished()), download, SLOT(deleteLater()));

	foreach (const QUrl& url, pictures_.uniqueKeys())
	{
	    download->download(url);
	}
    }
    else
    {
	emit loaded();
    }
}

void Report::pictureDownloaded(const QUrl& url, const QPixmap& pixmap)
{
    Q_ASSERT(!url.isEmpty());

    QMap<QUrl, Picture>::iterator it = pictures_.find(url);
    while (it != pictures_.end())
    {
	if (it.key() == url)
	{
	    it.value().setPixmap(pixmap);
	}
	++it;
    }
}

int Report::rows() const
{
    return rows_;
}

int Report::cols() const
{
    return cols_;
}

Report::Data Report::split() const
{
    Data list;

    if (!data_.isEmpty())
    {
	const QStringList pages = StringList::trimRight(data_.split('\f'));
	foreach (const QString& page, pages)
	{
	    const QStringList lines = StringList::trimRight(page.split(QRegExp("(\\r)|(\\n)|(\\r\\n)")));
	    list.append(lines);
	}
    }

    return list;
}

void Report::setRows(int rows)
{
    if (rows != rows_)
    {
	rows_ = rows;
	emit changed();
    }
}

void Report::setCols(int cols)
{
    if (cols_ != cols)
    {
	cols_ = cols;
	emit changed();
    }
}

QFont Report::font() const
{
    return font_;
}

void Report::setFont(const QFont& font)
{
    if (font.isCopyOf(font_))
    {
	return;
    }
    font_ = font;
    emit changed();
}

bool Report::stretchFont() const
{
    return stretchFont_;
}

void Report::setStretchFont(bool stretch)
{
    if (stretchFont_ != stretch)
    {
	stretchFont_ = stretch;
	emit changed();
    }
}

int Report::lpi() const
{
    return lpi_;
}

void Report::setLpi(int newLpi)
{
    if (newLpi != lpi_)
    {
	lpi_ = newLpi;
	emit changed();
    }
}

int Report::cpi() const
{
    return cpi_;
}

void Report::setCpi(int newCpi)
{
    if (newCpi != cpi_)
    {
	cpi_ = newCpi;
	emit changed();
    }
}

QColor Report::fontColor() const
{
    return fontColor_;
}

void Report::setFontColor(const QColor& color)
{
    if (color != fontColor_)
    {
	fontColor_ = color;
	emit changed();
    }
}

bool Report::stripes() const
{
    return stripes_;
}

void Report::setStripes(bool stripes)
{
    if (stripes != stripes_)
    {
	stripes_ = stripes;
	emit changed();
    }
}

QColor Report::stripeColor() const
{
    return stripeColor_;
}

void Report::setStripeColor(const QColor& color)
{
    if (stripeColor_ != color)
    {
	stripeColor_ = color;
	emit changed();
    }
}

bool Report::bold() const
{
    return bold_;
}

void Report::setBold(bool bold)
{
    if (bold != bold_)
    {
	bold_ = bold;
	emit changed();
    }
}

QPrinter::Unit Report::unit() const
{
    return unit_;
}

QPrinter::Orientation Report::orientation() const
{
    return orientation_;
}

MarginsF Report::margins() const
{
    return margins_;
}

QSizeF Report::paperSize() const
{
    return paperSize_;
}

QList<Picture> Report::pictures() const
{
    return pictures_.values();
}

void Report::processCommands()
{
    // Use regex to extract the command block
    QRegExp re("\\[REPORT\\].*\\[REPORT\\](\\r|\\n|\\r\\n)?");
    if (data_.indexOf(re) != -1)
    {
	// Get the commands and remove from the data
	const QString commands = re.cap();
	data_.remove(re);

	typedef QPair<QString, QStringList> CommandPair;
	QList<CommandPair> list;

	// Process the commands using another regex
	re.setPattern("\\[[A-Za-z]+=[^\\]]+\\]");
	for (int index = commands.indexOf(re); index != -1; index = commands.indexOf(re, index + re.matchedLength()))
	{
	    const QString cap = re.cap();
	    const int index = cap.indexOf('=');
	    Q_ASSERT(index != -1);
	    const QString command = cap.mid(1, index - 1);
	    const QString value = cap.mid(index + 1, cap.length() - index - 2);
	    // Ignore comments
	    if (commands.at(0) != '#')
	    {
		// Split into commands
		const QStringList tokens = value.split(',');
		// Add to the list
		list << CommandPair(command, tokens);
	    }
	}

	// Process the list
	foreach (const CommandPair& command, list)
	{
	    const Command::Type type = Command::stringToType(command.first);
	    switch (type)
	    {
	    case Command::Font:
		font_.setFamily(command.second.front());
		emit fontChanged(font_);
		break;
	    case Command::StretchFont:
		stretchFont_ = command.second.front() == "1";
		emit stretchFontChanged(stretchFont_);
		break;
	    case Command::Rows:
		rows_ = command.second.front().toInt();
		break;
	    case Command::Cols:
		cols_ = command.second.front().toInt();
		break;
	    case Command::Lpi:
		lpi_ = command.second.front().toInt();
		emit lpiChanged(lpi_);
		break;
	    case Command::Cpi:
		cpi_ = command.second.front().toInt();
		emit cpiChanged(cpi_);
		break;
	    case Command::FontColor:
		fontColor_ = QColor(command.second.front());
		break;
	    case Command::Bold:
		bold_ = command.second.front() == "1";
		emit boldChanged(bold_);
		break;
	    case Command::Stripes:
		stripes_ = command.second.front() == "1";
		break;
	    case Command::StripeColor:
		stripeColor_ = QColor(command.second.front());
		break;
	    case Command::Units:
		unit_ = static_cast<QPrinter::Unit>(command.second.front().toInt());
		break;
	    case Command::Margins:
		margins_ = MarginsF(command.second[0].toDouble(),
				    command.second[1].toDouble(),
				    command.second[2].toDouble(),
				    command.second[3].toDouble());
		break;
	    case Command::PaperSize:
		paperSize_ = QSizeF(command.second[0].toDouble(), command.second[1].toDouble());
		break;
	    case Command::Orientation:
		orientation_ = static_cast<QPrinter::Orientation>(command.second.front().toInt());
		printer_->setOrientation(orientation_);
		emit orientationChanged(orientation_);
		break;
	    case Command::AutoLandscape:
		autoLandscape_ = command.second.front() == "1";
		break;
	    case Command::AutoLandscapeCol:
		autoLandscapeCol_ = command.second.front().toInt();
		break;
	    case Command::Picture:
		pictures_.insert(command.second[0], Picture(command.second[0],
							    QRectF(command.second[1].toDouble(),
								   command.second[2].toDouble(),
								   command.second[3].toDouble(),
								   command.second[4].toDouble())));
		break;
	    case Command::Unknown:
		break;
	    }
	}
	// Apply printer settings that depend on the measurement unit
	printer_->setPageMargins(margins_.left(), margins_.top(), margins_.right(), margins_.bottom(), unit_);
	printer_->setPaperSize(paperSize_, unit_);
    }
}
