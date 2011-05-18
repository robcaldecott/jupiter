#ifndef REPORT_H
#define REPORT_H

#include "MarginsF.h"
#include "Picture.h"
#include <QObject>
#include <QString>
#include <QList>
#include <QStringList>
#include <QFont>
#include <QColor>
#include <QMargins>
#include <QSizeF>
#include <QPrinter>
#include <QUrl>
#include <QMultiMap>

class QPixmap;

/// Class used for report data
class Report : public QObject
{
    Q_OBJECT

public:
    typedef QList<QStringList> Data;

    explicit Report(QPrinter* printer, QObject *parent = 0);

    bool isEmpty() const;
    void clear();
    QString data() const;
    void setData(const QString& data);
    int rows() const;
    int cols() const;
    Data split() const;
    QFont font() const;
    bool stretchFont() const;
    int lpi() const;
    int cpi() const;
    QColor fontColor() const;
    bool bold() const;
    bool stripes() const;
    QColor stripeColor() const;
    QPrinter::Unit unit() const;
    QPrinter::Orientation orientation() const;
    MarginsF margins() const;
    QSizeF paperSize() const;
    QList<Picture> pictures() const;

public slots:
    void setRows(int rows);
    void setCols(int cols);
    void setFont(const QFont& font);
    void setStretchFont(bool stretch);
    void setLpi(int newLpi);
    void setCpi(int newCpi);
    void setFontColor(const QColor& color);
    void setBold(bool bold);
    void setStripes(bool stripes);
    void setStripeColor(const QColor& color);

private slots:
    void pictureDownloaded(const QUrl& url, const QPixmap& pixmap);

signals:
    void loaded();
    void hasContent(bool);
    void changed();
    void stretchFontChanged(bool);
    void orientationChanged(QPrinter::Orientation);
    void lpiChanged(int);
    void cpiChanged(int);
    void boldChanged(bool);
    void fontChanged(const QFont&);

private:
    Report& operator=(const Report& report);
    void processCommands();

    QPrinter* printer_;
    QString data_;
    int rows_;
    int cols_;
    QFont font_;
    bool stretchFont_;
    int lpi_;
    int cpi_;
    QColor fontColor_;
    bool bold_;
    bool stripes_;
    QColor stripeColor_;
    QPrinter::Unit unit_;
    QPrinter::Orientation orientation_;
    bool autoLandscape_;
    int autoLandscapeCol_;
    MarginsF margins_;
    QSizeF paperSize_;
    QMultiMap<QUrl, Picture> pictures_;
};

#endif // REPORT_H
