#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Printer.h"
#include "Font.h"
#include "Report.h"
#include "ReportView.h"
#include "DownloadFile.h"
#include "SimpleMapi.h"
#include <QMessageBox>
#include <QDebug>
#include <QPrintPreviewWidget>
#include <QPrinter>
#include <QPainter>
#include <QFileDialog>
#include <QPageSetupDialog>
#include <QFontComboBox>
#include <QLineEdit>
#include <QComboBox>
#include <QColorDialog>
#include <QPrintDialog>
#include <QSettings>
#include <QCloseEvent>
#include <QActionGroup>
#include <QClipboard>
#include <QTimer>
#include <QInputDialog>
#include <QUrl>
#include <QTemporaryFile>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    createStatusBar();

    // Recent files
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
	recentFileActions[i] = new QAction(this);
	recentFileActions[i]->setVisible(false);
	connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
	ui->menuFile->insertAction(ui->actionExit, recentFileActions[i]);
    }
    recentFilesSeparator = ui->menuFile->insertSeparator(ui->actionExit);

    printer = new QPrinter;
    printer->setFullPage(true);
    printer->setPaperSize(QPrinter::Letter);
    printer->setPageMargins(.5, .5, .5, .5, QPrinter::Inch);

    report = new Report(printer, this);
    view = new ReportView(report, this);

    preview = new QPrintPreviewWidget(printer, this);
    setCentralWidget(preview);
    preview->fitToWidth();

    QFontComboBox* fontComboBox = new QFontComboBox;
    fontComboBox->setFontFilters(QFontComboBox::MonospacedFonts | QFontComboBox::ScalableFonts);
    fontComboBox->setCurrentFont(report->font().family());
    ui->fontToolBar->insertWidget(ui->actionBold, fontComboBox);
    connect(fontComboBox, SIGNAL(currentFontChanged(QFont)), report, SLOT(setFont(QFont)));

    pageNumberComboBox = new QComboBox;
    ui->viewToolBar->insertWidget(ui->actionNextPage, pageNumberComboBox);
    connect(pageNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPageChanged(int)));

    const qreal zoomFactors[] = { 12.5, 25, 50, 100, 125, 150, 200, 400, 800 };
    zoomComboBox = new QComboBox;
    for (unsigned int i = 0; i < sizeof(zoomFactors) / sizeof(*zoomFactors); ++i)
    {
	zoomComboBox->addItem(QString("%1%").arg(zoomFactors[i]));
    }
    zoomComboBox->setCurrentIndex(-1);
    ui->viewToolBar->insertWidget(ui->actionZoomOut, zoomComboBox);
    connect(zoomComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(zoomChanged(QString)));

    QActionGroup* orientationGroup = new QActionGroup(this);
    orientationGroup->addAction(ui->actionPortrait);
    orientationGroup->addAction(ui->actionLandscape);
    orientationChanged(report->orientation());

    QActionGroup* heightGroup = new QActionGroup(this);
    heightGroup->addAction(ui->action6LPI);
    heightGroup->addAction(ui->action8LPI);
    heightGroup->addAction(ui->action9LPI);
    heightGroup->setDisabled(ui->actionStretchFont->isEnabled());
    connect(ui->actionStretchFont, SIGNAL(toggled(bool)), heightGroup, SLOT(setDisabled(bool)));

    QActionGroup* widthGroup = new QActionGroup(this);
    widthGroup->addAction(ui->actionDefaultWidth);
    widthGroup->addAction(ui->action10CPI);
    widthGroup->addAction(ui->action12CPI);
    widthGroup->addAction(ui->action17CPI);
    widthGroup->setDisabled(ui->actionStretchFont->isEnabled());
    connect(ui->actionStretchFont, SIGNAL(toggled(bool)), widthGroup, SLOT(setDisabled(bool)));

    QActionGroup* pageGroup = new QActionGroup(this);
    pageGroup->addAction(ui->actionSinglePage);
    pageGroup->addAction(ui->actionFacingPages);
    pageGroup->addAction(ui->actionAllPages);
    ui->actionSinglePage->setChecked(preview->viewMode() == QPrintPreviewWidget::SinglePageView);
    ui->actionFacingPages->setChecked(preview->viewMode() == QPrintPreviewWidget::FacingPagesView);
    ui->actionAllPages->setChecked(preview->viewMode() == QPrintPreviewWidget::AllPagesView);

    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionOpenURL, SIGNAL(triggered()), this, SLOT(openUrl()));
    connect(ui->actionReload, SIGNAL(triggered()), this, SLOT(reload()));
    connect(ui->actionSaveAsPDF, SIGNAL(triggered()), this, SLOT(saveAsPdf()));
    connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(print()));
    connect(ui->actionPageSetup, SIGNAL(triggered()), this, SLOT(pageSetup()));
    connect(ui->actionEmail, SIGNAL(triggered()), this, SLOT(email()));
    connect(ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui->actionStretchFont, SIGNAL(toggled(bool)), report, SLOT(setStretchFont(bool)));
    connect(report, SIGNAL(stretchFontChanged(bool)), ui->actionStretchFont, SLOT(setChecked(bool)));
    connect(ui->actionBold, SIGNAL(toggled(bool)), report, SLOT(setBold(bool)));
    connect(ui->actionStripes, SIGNAL(toggled(bool)), report, SLOT(setStripes(bool)));
    connect(ui->actionStripeColor, SIGNAL(triggered()), this, SLOT(stripeColor()));
    connect(ui->actionColor, SIGNAL(triggered()), this, SLOT(fontColor()));
    connect(ui->action6LPI, SIGNAL(triggered()), this, SLOT(height6Lpi()));
    connect(ui->action8LPI, SIGNAL(triggered()), this, SLOT(height8Lpi()));
    connect(ui->action9LPI, SIGNAL(triggered()), this, SLOT(height9Lpi()));
    connect(ui->actionDefaultWidth, SIGNAL(toggled(bool)), this, SLOT(widthDefault()));
    connect(ui->action10CPI, SIGNAL(triggered()), this, SLOT(width10Cpi()));
    connect(ui->action12CPI, SIGNAL(triggered()), this, SLOT(width12Cpi()));
    connect(ui->action17CPI, SIGNAL(triggered()), this, SLOT(width17Cpi()));
    connect(ui->actionFirstPage, SIGNAL(triggered()), this, SLOT(firstPage()));
    connect(ui->actionPreviousPage, SIGNAL(triggered()), this, SLOT(previousPage()));
    connect(ui->actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
    connect(ui->actionLastPage, SIGNAL(triggered()), this, SLOT(lastPage()));
    connect(ui->actionFitWidth, SIGNAL(triggered()), preview, SLOT(fitToWidth()));
    connect(ui->actionFitHeight, SIGNAL(triggered()), preview, SLOT(fitInView()));
    connect(ui->actionPortrait, SIGNAL(triggered()), preview, SLOT(setPortraitOrientation()));
    connect(ui->actionLandscape, SIGNAL(triggered()), preview, SLOT(setLandscapeOrientation()));
    connect(ui->actionActualSize, SIGNAL(triggered()), this, SLOT(actualSize()));
    connect(ui->actionZoomIn, SIGNAL(triggered()), preview, SLOT(zoomIn()));
    connect(ui->actionZoomOut, SIGNAL(triggered()), preview, SLOT(zoomOut()));
    connect(ui->actionSinglePage, SIGNAL(triggered()), preview, SLOT(setSinglePageViewMode()));
    connect(ui->actionFacingPages, SIGNAL(triggered()), preview, SLOT(setFacingPagesViewMode()));
    connect(ui->actionAllPages, SIGNAL(triggered()), preview, SLOT(setAllPagesViewMode()));
    connect(ui->actionMainToolbar, SIGNAL(triggered(bool)), this, SLOT(toggleMainToolbar(bool)));
    connect(ui->actionViewToolbar, SIGNAL(triggered(bool)), this, SLOT(toggleViewToolbar(bool)));
    connect(ui->actionFontToolbar, SIGNAL(triggered(bool)), this, SLOT(toggleFontToolbar(bool)));
    connect(ui->actionStatusBar, SIGNAL(triggered(bool)), this, SLOT(toggleStatusBar(bool)));
    connect(ui->menuToolbars, SIGNAL(aboutToShow()), this, SLOT(updateToolbarMenu()));
    connect(ui->menuView, SIGNAL(aboutToShow()), this, SLOT(updateToolbarMenu()));
    connect(preview, SIGNAL(previewChanged()), this, SLOT(previewChanged()));

    connect(preview, SIGNAL(paintRequested(QPrinter*)), this, SLOT(paint(QPrinter*)));
    connect(report, SIGNAL(loaded()), preview, SLOT(updatePreview()));
    connect(report, SIGNAL(changed()), preview, SLOT(updatePreview()));
    connect(report, SIGNAL(loaded()), this, SLOT(documentLoaded()));
    connect(report, SIGNAL(orientationChanged(QPrinter::Orientation)), this, SLOT(orientationChanged(QPrinter::Orientation)));
    connect(report, SIGNAL(lpiChanged(int)), this, SLOT(lpiChanged(int)));
    connect(report, SIGNAL(cpiChanged(int)), this, SLOT(cpiChanged(int)));
    connect(report, SIGNAL(boldChanged(bool)), ui->actionBold, SLOT(setChecked(bool)));
    connect(report, SIGNAL(fontChanged(QFont)), fontComboBox, SLOT(setCurrentFont(QFont)));

    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("state").toByteArray());
    recentFiles = settings.value("recentFiles").toStringList();
    currentFolder = settings.value("currentFolder").toString();
    ui->statusBar->setVisible(settings.value("statusBar", true).toBool());

    updateRecentFileActions();

    zoomTimer = new QTimer(this);
    connect(zoomTimer, SIGNAL(timeout()), this, SLOT(updateZoom()));
    connect(preview, SIGNAL(previewChanged()), this, SLOT(updateZoom()));
    zoomTimer->start(1000);
}

MainWindow::~MainWindow()
{
    delete printer;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("currentFolder", currentFolder);
    settings.setValue("statusBar", ui->statusBar->isVisible());
    QWidget::closeEvent(event);
}

void MainWindow::createStatusBar()
{
    fileNamePane = new QLabel("");
    fileNamePane->setAlignment(Qt::AlignLeft);
    statusBar()->addWidget(fileNamePane, 1);

    pagesPane = new QLabel(" 100/100 ");
    pagesPane->setAlignment(Qt::AlignHCenter);
    pagesPane->setMinimumSize(pagesPane->sizeHint());
    statusBar()->addWidget(pagesPane);
    pagesPane->setText("");

    sizePane = new QLabel(" 100x100 ");
    sizePane->setAlignment(Qt::AlignHCenter);
    sizePane->setMinimumSize(sizePane->sizeHint());
    statusBar()->addWidget(sizePane);
    sizePane->setText("");

    zoomPane = new QLabel(" 800% ");
    zoomPane->setAlignment(Qt::AlignHCenter);
    zoomPane->setMinimumSize(sizePane->sizeHint());
    statusBar()->addWidget(zoomPane);
    zoomPane->setText("");
}

void MainWindow::updateRecentFileActions()
{
    QStringList existingFiles;
    foreach (const QString& fileName, recentFiles)
    {
	const QString scheme = QUrl(fileName).scheme().toLower();
	if (scheme == "ftp" || scheme == "http" || QFile::exists(fileName))
	{
	    existingFiles << fileName;
	}
    }
    recentFiles = existingFiles;

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
	if (i < recentFiles.count())
	{
	    QString str = tr("&%1 %2").arg(i + 1).arg(recentFiles[i]);
	    recentFileActions[i]->setText(str);
	    recentFileActions[i]->setVisible(true);
	    recentFileActions[i]->setData(recentFiles[i]);
	}
	else
	{
	    recentFileActions[i]->setVisible(false);
	}
    }
    recentFilesSeparator->setVisible(!recentFiles.isEmpty());
}

void MainWindow::enableActions()
{
    bool isEmpty = report->isEmpty();

    ui->actionReload->setEnabled(!isEmpty);
    ui->actionSaveAsPDF->setEnabled(!isEmpty);
    ui->actionPrint->setEnabled(!isEmpty);
    ui->actionEmail->setEnabled(Mapi::isAvailable() && !isEmpty);
    ui->actionCopy->setEnabled(!isEmpty);
    ui->actionStripes->setChecked(report->stripes());

    sizePane->setText(isEmpty ? "" :  QString("%1 x %2").arg(report->rows()).arg(report->cols()));
}

void MainWindow::documentLoaded()
{
    enableActions();
}

void MainWindow::open()
{
    const QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), currentFolder, tr("All Files (*.*)"));
    if (fileName != "")
    {
	currentFolder = QFileInfo(fileName).path();
	load(fileName);
    }
}

void MainWindow::openUrl()
{
    const QString url = QInputDialog::getText(this, tr("Open URL"), tr("URL:"));
    if (url != "")
    {
	load(url);
    }
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
    {
	load(action->data().toString());
    }
}

void MainWindow::reload()
{
    load(currentFileName);
}

void MainWindow::load(const QString& fileName)
{
    if (fileName != "")
    {
	statusBar()->showMessage(tr("Loading %1...").arg(fileName));

	DownloadFile* download = new DownloadFile(this);
	connect(download, SIGNAL(completed(QString)), this, SLOT(downloadFinished(QString)));
	connect(download, SIGNAL(error(QUrl,QString)), this, SLOT(downloadError(QUrl,QString)));
	connect(download, SIGNAL(finished()), download, SLOT(deleteLater()));
	download->download(fileName);
	currentFileName = fileName;

	recentFiles.removeAll(currentFileName);
	recentFiles.prepend(currentFileName);
	updateRecentFileActions();
    }
}

void MainWindow::saveAsPdf()
{
    saveAsPdf(QFileDialog::getSaveFileName(this, tr("Save As PDF"), QString(), tr("PDF Files (*.pdf)")));
}

void MainWindow::saveAsPdf(const QString& fileName) const
{
    if (fileName != "")
    {
	QPrinter pdfPrinter;
	pdfPrinter.setOutputFormat(QPrinter::PdfFormat);
	pdfPrinter.setOutputFileName(fileName);
	pdfPrinter.setColorMode(QPrinter::Color);

	Printer::applySettings(&pdfPrinter, printer);

	QPainter painter(&pdfPrinter);
	view->paint(painter, pdfPrinter);
    }
}

void MainWindow::print()
{
    QPrintDialog printDialog(printer, this);
    if (printDialog.exec() == QDialog::Accepted)
    {
	preview->print();
    }
}

void MainWindow::pageSetup()
{
    QPageSetupDialog pageSetupDialog(printer, this);
    if (pageSetupDialog.exec() == QDialog::Accepted)
    {
	preview->updatePreview();
    }
}

void MainWindow::email()
{
    QString fileName;

    // Create a temp PDF file
    {
	QTemporaryFile tempFile;
	tempFile.setAutoRemove(false);
	tempFile.open();
	fileName = tempFile.fileName();
	saveAsPdf(fileName);
    }

    Q_ASSERT(fileName != "");
    Mapi::send(fileName);

    // Add to list of temp files to be
    // deleted on exit.
    tempFiles.add(fileName);
}

void MainWindow::about()
{
    QSettings settings;
    QString str;
    str += tr("<h2>Jupiter Report Viewer %1.%2</h2>").arg(VER_MAJOR).arg(VER_MINOR);
    str += tr("<p>Built on %1 at %2</p>").arg(__DATE__).arg(__TIME__);
    str += tr("<p>Copyright &copy; 2010 XE-QT Solutions. All rights reserved.</p>"
	      "<p>This program is protected by copyright law and international "
	      "treaties. Unauthorized reproduction or distribution of this "
	      "program, or any portion of it, may result in severe civil and "
	      "criminal penalties, and will be prosecuted to the maximum "
	      "extent possible under the law.</p>");
    QMessageBox::about(this, tr("About"), str);
}

void MainWindow::paint(QPrinter* printer)
{
    Q_ASSERT(printer != NULL);

    QPainter painter(printer);
    int pageCount = view->paint(painter, *printer);

    pageNumberComboBox->setEnabled(pageCount > 1);
    pageNumberComboBox->clear();
    for (int i = 0; i < pageCount; ++i)
    {
	QString str = QString::number(i + 1);
	pageNumberComboBox->addItem(str);
    }

    ui->actionFirstPage->setEnabled(pageCount > 1);
    ui->actionPreviousPage->setEnabled(pageCount > 1);
    ui->actionNextPage->setEnabled(pageCount > 1);
    ui->actionLastPage->setEnabled(pageCount > 1);
}

void MainWindow::previewChanged()
{
    disconnect(pageNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPageChanged(int)));
    disconnect(zoomComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(zoomChanged(QString)));

    QString str = QString::number(preview->currentPage());
    pageNumberComboBox->setCurrentIndex(pageNumberComboBox->findText(str));

    const int zoomFactor = qRound(preview->zoomFactor() * 100);
    zoomComboBox->setCurrentIndex(zoomComboBox->findText(QString("%1%").arg(zoomFactor)));

    connect(pageNumberComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentPageChanged(int)));
    connect(zoomComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(zoomChanged(QString)));

    pagesPane->setText(QString("%1/%2").arg(preview->currentPage()).arg(preview->pageCount()));
}

void MainWindow::firstPage()
{
    preview->setCurrentPage(1);
    previewChanged();
}

void MainWindow::previousPage()
{
    const int currentPage = preview->currentPage();
    if (currentPage > 1)
    {
	preview->setCurrentPage(currentPage - 1);
	previewChanged();
    }
}

void MainWindow::nextPage()
{
    const int currentPage = preview->currentPage();
    if (currentPage < preview->pageCount())
    {
	preview->setCurrentPage(currentPage + 1);
	previewChanged();
    }
}

void MainWindow::lastPage()
{
    preview->setCurrentPage(preview->pageCount());
    previewChanged();
}

void MainWindow::height6Lpi()
{
    report->setLpi(6);
}

void MainWindow::height8Lpi()
{
    report->setLpi(8);
}

void MainWindow::height9Lpi()
{
    report->setLpi(9);
}

void MainWindow::widthDefault()
{
    report->setCpi(0);
}

void MainWindow::width10Cpi()
{
    report->setCpi(10);
}

void MainWindow::width12Cpi()
{
    report->setCpi(12);
}

void MainWindow::width17Cpi()
{
    report->setCpi(17);
}

void MainWindow::fontColor()
{
    const QColor color = QColorDialog::getColor(report->fontColor(), this);
    if (color.isValid())
    {
	report->setFontColor(color);
    }
}

void MainWindow::currentPageChanged(int pageNumber)
{
    preview->setCurrentPage(pageNumber + 1);
    pagesPane->setText(QString("%1/%2").arg(preview->currentPage()).arg(preview->pageCount()));
}

void MainWindow::stripeColor()
{
    const QColor color = QColorDialog::getColor(report->stripeColor(), this);
    if (color.isValid())
    {
	report->setStripeColor(color);
    }
}

void MainWindow::actualSize()
{
    preview->setZoomFactor(1.0);
    previewChanged();
}

void MainWindow::zoomChanged(QString str)
{
    const qreal zoomFactor = str.remove('%').toDouble();
    preview->setZoomFactor(zoomFactor / 100);
}

void MainWindow::copy()
{
    QApplication::clipboard()->setText(report->data());
}

void MainWindow::orientationChanged(QPrinter::Orientation orientation)
{
    switch (orientation)
    {
    case QPrinter::Portrait:
	ui->actionPortrait->setChecked(true);
	break;
    case QPrinter::Landscape:
	ui->actionLandscape->setChecked(true);
	break;
    }
}

void MainWindow::lpiChanged(int lpi)
{
    switch (lpi)
    {
    case 6:
	ui->action6LPI->setChecked(true);
	break;
    case 8:
	ui->action8LPI->setChecked(true);
	break;
    case 9:
	ui->action9LPI->setChecked(true);
	break;
    }
}

void MainWindow::cpiChanged(int cpi)
{
    switch (cpi)
    {
    case 0:
	ui->actionDefaultWidth->setChecked(true);
	break;
    case 10:
	ui->action10CPI->setChecked(true);
	break;
    case 12:
	ui->action12CPI->setChecked(true);
	break;
    case 17:
	ui->action17CPI->setChecked(true);
	break;
    }
}

void MainWindow::downloadFinished(const QString& data)
{
    statusBar()->showMessage("");
    report->setData(data);
    fileNamePane->setText(currentFileName);
}

void MainWindow::downloadError(const QUrl& url, const QString& error)
{
    statusBar()->showMessage("");

    const QString scheme = url.scheme().toLower();
    if (scheme == "http" || scheme == "ftp")
    {
	QMessageBox::warning(this, tr("Download Error"), tr("Failed to download %1: %2").arg(url.toString()).arg(error));
    }
    else
    {
	QMessageBox::warning(this, tr("Open Error"), tr("Failed to open %1").arg(url.toString()));
    }

    recentFiles.removeAll(url.toString());
    updateRecentFileActions();
    report->setData("");
    fileNamePane->setText("");
    enableActions();
}

void MainWindow::updateZoom()
{
    const int zoomFactor = qRound(preview->zoomFactor() * 100);
    zoomPane->setText(QString("%1%").arg(zoomFactor));
}

void MainWindow::toggleMainToolbar(bool toggled)
{
    ui->mainToolBar->setVisible(toggled);
}

void MainWindow::toggleViewToolbar(bool toggle)
{
    ui->viewToolBar->setVisible(toggle);
}

void MainWindow::toggleFontToolbar(bool toggle)
{
    ui->fontToolBar->setVisible(toggle);
}

void MainWindow::toggleStatusBar(bool toggle)
{
    ui->statusBar->setVisible(toggle);
}

void MainWindow::updateToolbarMenu()
{
    ui->actionMainToolbar->setChecked(ui->mainToolBar->isVisible());
    ui->actionViewToolbar->setChecked(ui->viewToolBar->isVisible());
    ui->actionFontToolbar->setChecked(ui->fontToolBar->isVisible());
    ui->actionStatusBar->setChecked(ui->statusBar->isVisible());
}
