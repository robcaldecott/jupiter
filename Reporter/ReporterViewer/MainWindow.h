#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TempFiles.h"
#include <QMainWindow>
#include <QPrinter>
#include <QStringList>

class Report;
class ReportView;
class QPrintPreviewWidget;
class QComboBox;
class QCloseEvent;
class QUrl;
class QLabel;
class QTimer;
class QAction;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void load(const QString& fileName);

public slots:
    void print();

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void documentLoaded();
    void about();
    void open();
    void openUrl();
    void openRecentFile();
    void reload();
    void saveAsPdf();
    void pageSetup();
    void email();
    void previewChanged();
    void firstPage();
    void previousPage();
    void nextPage();
    void lastPage();
    void height6Lpi();
    void height8Lpi();
    void height9Lpi();
    void widthDefault();
    void width10Cpi();
    void width12Cpi();
    void width17Cpi();
    void fontColor();
    void currentPageChanged(int pageNumber);
    void stripeColor();
    void actualSize();
    void zoomChanged(QString str);
    void copy();
    void orientationChanged(QPrinter::Orientation orientation);
    void lpiChanged(int lpi);
    void cpiChanged(int cpi);
    void paint(QPrinter* printer);
    void downloadFinished(const QString& data);
    void downloadError(const QUrl& url, const QString& error);
    void updateZoom();
    void toggleMainToolbar(bool toggle);
    void toggleViewToolbar(bool toggle);
    void toggleFontToolbar(bool toggle);
    void toggleStatusBar(bool toggle);
    void updateToolbarMenu();

private:
    enum { MaxRecentFiles = 5 };

    void createStatusBar();
    void updateRecentFileActions();
    void enableActions();
    void saveAsPdf(const QString& fileName) const;

    Ui::MainWindow* ui;
    QLabel* fileNamePane;
    QLabel* pagesPane;
    QLabel* sizePane;
    QLabel* zoomPane;
    QPrintPreviewWidget* preview;
    QPrinter* printer;
    Report* report;
    ReportView* view;
    QComboBox* pageNumberComboBox;
    QComboBox* zoomComboBox;
    QString currentFileName;
    QString currentFolder;
    TempFiles tempFiles;
    QTimer* zoomTimer;
    QStringList recentFiles;
    QAction* recentFileActions[MaxRecentFiles];
    QAction* recentFilesSeparator;
};

#endif // MAINWINDOW_H
