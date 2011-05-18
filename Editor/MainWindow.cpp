#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Form.h"
#include "File.h"
#include "GraphicsView.h"
#include "GraphicsItem.h"
#include "ItemWidget.h"
#include "FormWidget.h"
#include "SelectedProperties.h"
#include "Printer.h"
#include "SimpleMapi.h"
#include <QString>
#include <QMessageBox>
#include <QSettings>
#include <QPageSetupDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <QFontComboBox>
#include <QFile>
#include <QDataStream>
#include <QTimer>
#include <QCloseEvent>
#include <QClipboard>
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QFileInfo>
#include <QPrintPreviewDialog>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	for (int i = 0; i < MaxRecentFiles; ++i)
	{
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
		ui->menuFile->insertAction(ui->actionExit, recentFileActions[i]);
	}
	recentFilesSeparator = ui->menuFile->insertSeparator(ui->actionExit);

	form = new Form(this);
	view = new GraphicsView(form, this);
	setCentralWidget(view);

	itemWidget = new ItemWidget(ui->propertyWidget);
	itemWidget->setHidden(true);
	connect(view, SIGNAL(mouseDoubleClick()), itemWidget, SLOT(selectPicture()));

	formWidget = new FormWidget(ui->propertyWidget);
	formWidget->connectForm(form);
	formWidget->update(form);
	ui->propertyWidget->setWidget(formWidget);

	fontCombo = new QFontComboBox(this);
	ui->formatToolBar->insertWidget(ui->actionBold, fontCombo);

	insertGroup = new QActionGroup(this);
	insertGroup->addAction(ui->actionSelect);
	insertGroup->addAction(ui->actionPaint);
	connect(insertGroup, SIGNAL(triggered(QAction*)), this, SLOT(insertObject(QAction*)));

	ui->actionLeft->setData(Qt::AlignLeft);
	ui->actionCenter->setData(Qt::AlignHCenter);
	ui->actionRight->setData(Qt::AlignRight);

	zoomGroup = new QActionGroup(this);
	zoomGroup->addAction(ui->actionActualSize);
	zoomGroup->addAction(ui->actionFitWidth);
	zoomGroup->addAction(ui->actionFitHeight);
	connect(zoomGroup, SIGNAL(triggered(QAction*)), this, SLOT(zoom(QAction*)));

	ui->actionActualSize->setData(GraphicsView::ActualSize);
	ui->actionFitWidth->setData(GraphicsView::FitWidth);
	ui->actionFitHeight->setData(GraphicsView::FitHeight);

	horzAlignGroup = new QActionGroup(this);
	horzAlignGroup->addAction(ui->actionLeft);
	horzAlignGroup->addAction(ui->actionCenter);
	horzAlignGroup->addAction(ui->actionRight);
	connect(horzAlignGroup, SIGNAL(triggered(QAction*)), this, SLOT(horzAlign(QAction*)));

	sizeGroup = new QActionGroup(this);
	sizeGroup->addAction(ui->actionShrinkWidth);
	sizeGroup->addAction(ui->actionGrowWidth);
	sizeGroup->addAction(ui->actionPageWidth);
	sizeGroup->addAction(ui->actionShrinkHeight);
	sizeGroup->addAction(ui->actionGrowHeight);
	sizeGroup->addAction(ui->actionPageHeight);
	sizeGroup->addAction(ui->actionShrinkBoth);
	sizeGroup->addAction(ui->actionGrowBoth);
	sizeGroup->addAction(ui->actionPageBoth);

	ui->actionShrinkWidth->setData(Form::ShrinkWidth);
	ui->actionGrowWidth->setData(Form::GrowWidth);
	ui->actionPageWidth->setData(Form::PageWidth);
	ui->actionShrinkHeight->setData(Form::ShrinkHeight);
	ui->actionGrowHeight->setData(Form::GrowHeight);
	ui->actionPageHeight->setData(Form::PageHeight);
	ui->actionShrinkBoth->setData(Form::ShrinkBoth);
	ui->actionGrowBoth->setData(Form::GrowBoth);
	ui->actionPageBoth->setData(Form::PageBoth);

	connect(sizeGroup, SIGNAL(triggered(QAction*)), this, SLOT(size(QAction*)));

	connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newForm()));
	connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openForm()));
	connect(ui->actionReload, SIGNAL(triggered()), this, SLOT(reload()));
	connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveForm()));
	connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFormAs()));
	connect(ui->actionSaveAsPDF, SIGNAL(triggered()), this, SLOT(saveFormAsPdf()));
	connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
	connect(ui->actionOpenPrintData, SIGNAL(triggered()), this, SLOT(openPrintData()));
	connect(ui->actionPageSetup, SIGNAL(triggered()), form, SLOT(pageSetup()));
	connect(ui->actionPagePreview, SIGNAL(triggered()), this, SLOT(preview()));
	connect(ui->actionSend, SIGNAL(triggered()), this, SLOT(email()));
	connect(ui->actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(fullScreen()));
	connect(ui->actionMargins, SIGNAL(toggled(bool)), form, SLOT(showMargins(bool)));
	connect(ui->actionGrid, SIGNAL(toggled(bool)), form, SLOT(showGrid(bool)));
	connect(ui->actionPrintData, SIGNAL(toggled(bool)), form, SLOT(showData(bool)));
	connect(ui->actionSelectAll, SIGNAL(triggered()), form, SLOT(selectAll()));
	connect(ui->actionDelete, SIGNAL(triggered()), form, SLOT(deleteSelected()));
	connect(ui->actionProperties, SIGNAL(toggled(bool)), ui->propertyWidget, SLOT(setVisible(bool)));
	connect(ui->actionMoveForwards, SIGNAL(triggered()), form, SLOT(moveForwards()));
	connect(ui->actionMoveBackwards, SIGNAL(triggered()), form, SLOT(moveBackwards()));
	connect(ui->actionAlignLeft, SIGNAL(triggered()), form, SLOT(alignLeft()));
	connect(ui->actionAlignRight, SIGNAL(triggered()), form, SLOT(alignRight()));
	connect(ui->actionAlignTop, SIGNAL(triggered()), form, SLOT(alignTop()));
	connect(ui->actionAlignBottom, SIGNAL(triggered()), form, SLOT(alignBottom()));
	connect(ui->actionCut, SIGNAL(triggered()), form, SLOT(cut()));
	connect(ui->actionCopy, SIGNAL(triggered()), form, SLOT(copy()));
	connect(ui->actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
	connect(ui->actionFirstPage, SIGNAL(triggered()), this, SLOT(firstPage()));
	connect(ui->actionPreviousPage, SIGNAL(triggered()), this, SLOT(previousPage()));
	connect(ui->actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
	connect(ui->actionLastPage, SIGNAL(triggered()), this, SLOT(lastPage()));

	connect(ui->menuView, SIGNAL(aboutToShow()), this, SLOT(updateViewMenu()));
	connect(form, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
	connect(form, SIGNAL(changed()), this, SLOT(formChanged()));
	connect(view, SIGNAL(doneRubberBanding(QRectF)), this, SLOT(doneRubberBanding(QRectF)));

	connectForm();
	loadSettings();
	QTimer::singleShot(0, this, SLOT(init()));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (!saveChanges())
	{
		event->ignore();
		return;
	}

	saveSettings();
	QWidget::closeEvent(event);
}

void MainWindow::loadSettings()
{
	QSettings settings;
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	recentFiles = settings.value("recentFiles").toStringList();
	recentFolder = settings.value("recentFolder", QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)).toString();
	updateRecentFileActions();
}

void MainWindow::saveSettings() const
{
	QSettings settings;
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("recentFiles", recentFiles);
	settings.setValue("recentFolder", recentFolder);
}

void MainWindow::about()
{
	QString str;
	str += tr("<h2>Jupiter Form Editor %1.%2</h2>").arg(VER_MAJOR).arg(VER_MINOR);
	str += tr("<p>Built on %1 at %2 using Qt %3</p>").arg(__DATE__).arg(__TIME__).arg(QT_VERSION_STR);
	str += tr("<p>Copyright &copy; 2010 Genesistems, Inc. All rights reserved.</p>"
			  "<p>This program is protected by copyright law and international "
			  "treaties. Unauthorized reproduction or distribution of this "
			  "program, or any portion of it, may result in severe civil and "
			  "criminal penalties, and will be prosecuted to the maximum "
			  "extent possible under the law.</p>");
	QMessageBox::about(this, tr("About"), str);
}

void MainWindow::init()
{
	const QStringList args = QApplication::arguments();
	if (args.count() > 1)
	{
		const QString fileName = args.at(1);

		setWindowFilePath(fileName);
		loadFile(fileName);
		recentFolder = QFileInfo(fileName).path();
		updateRecentFile(fileName);
	}
	else
		newForm();
}

void MainWindow::newForm()
{
	if (saveChanges())
	{
		form->reset();
		setWindowFilePath(tr("Untitled"));
		setWindowModified(false);
		updateUi();
	}
}

void MainWindow::openForm()
{
	if (saveChanges())
	{
		const QString fileName = QFileDialog::getOpenFileName(this, tr("Open"),
									  recentFolder,
									  tr("Jupiter Forms (*.jform);;All Files (*.*)"));
		if (fileName != "")
		{
			setWindowFilePath(fileName);
			loadFile(fileName);
			recentFolder = QFileInfo(fileName).path();
			updateRecentFile(fileName);
		}
	}
}

void MainWindow::openRecentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action && saveChanges())
	{
		const QString fileName = QDir::fromNativeSeparators(action->data().toString());
		setWindowFilePath(fileName);
		loadFile(fileName);
		updateRecentFile(fileName);
	}
}

void MainWindow::reload()
{
	if (saveChanges())
	{
		loadFile(windowFilePath());
		setWindowModified(false);
	}
}

void MainWindow::saveForm()
{
	const QString fileName = windowFilePath();
	if (fileName.isEmpty() || fileName == tr("Untitled"))
		saveFormAs();
	else
	{
		form->saveFile(fileName);
		setWindowModified(false);
		updateRecentFile(fileName);
	}
}

void MainWindow::saveFormAs()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
													recentFolder,
													tr("Jupiter Forms (*.jform);;All Files (*.*)"));
	if (fileName != "")
	{
		if (!fileName.toLower().endsWith(".jform"))
			fileName += ".jform";
		setWindowFilePath(fileName);
		form->saveFile(fileName);
		setWindowModified(false);
		recentFolder = QFileInfo(fileName).path();
		updateRecentFile(fileName);
	}
}

void MainWindow::openPrintData()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open Print Data"),
														  recentFolder,
														  tr("Text Files (*.txt)"));
	if (fileName != "")
	{
		recentFolder = QFileInfo(fileName).path();
		form->setData(File::loadText(fileName));
	}
}

void MainWindow::fullScreen()
{
	bool fullScreen = !(windowState() & Qt::WindowFullScreen);
	fullScreen ? showFullScreen() : showNormal();
	ui->actionFullScreen->setChecked(fullScreen);
}

void MainWindow::insertObject(QAction* action)
{
	if (action == ui->actionSelect)
		view->stopRubberBanding();
	else
		view->startRubberBanding();
}

void MainWindow::doneRubberBanding(const QRectF& rect)
{
	GraphicsItem* const item = form->createItem(rect);
	connectItem(item);
	ui->actionSelect->setChecked(true);
}

void MainWindow::selectionChanged()
{
	if (form->selectedItems().count())
	{
		ui->propertyWidget->setWidget(itemWidget);
		itemWidget->update(form->selected());
	}
	else
	{
		ui->propertyWidget->setWidget(formWidget);
		formWidget->update(form);
	}
	updateUi();
}

void MainWindow::connectForm()
{
	connect(ui->actionBold, SIGNAL(toggled(bool)), form, SLOT(setBold(bool)));
	connect(ui->actionItalic, SIGNAL(triggered(bool)), form, SLOT(setItalic(bool)));
	connect(fontCombo, SIGNAL(currentFontChanged(QFont)), form, SLOT(setFont(QFont)));
}

void MainWindow::connectItems()
{
	foreach (GraphicsItem* item, form->items())
		connectItem(item);
}

void MainWindow::connectItem(GraphicsItem* item)
{
	Q_ASSERT(item);
	connect(item, SIGNAL(changed()), this, SLOT(formChanged()));
	connect(ui->actionBold, SIGNAL(triggered(bool)), item, SLOT(setBold(bool)));
	connect(ui->actionItalic, SIGNAL(triggered(bool)), item, SLOT(setItalic(bool)));
	connect(ui->actionBox, SIGNAL(triggered(bool)), item, SLOT(setBox(bool)));
	connect(fontCombo, SIGNAL(currentFontChanged(QFont)), item, SLOT(setFont(QFont)));
	itemWidget->connectItem(item);
}

bool MainWindow::saveChanges()
{
	if (isWindowModified())
	{
		int result = QMessageBox::question(this, QApplication::applicationName(),
										   tr("The form has been modified.\nDo you want to save your changes?"),
										   QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
		if (result == QMessageBox::Cancel)
			return false;
		if (result == QMessageBox::Yes)
			saveForm();
	}
	return true;
}

void MainWindow::loadFile(const QString& fileName)
{
	switch (form->openFile(fileName))
	{
	case Form::Ok:
		statusBar()->showMessage(tr("Loaded %1").arg(fileName));
		connectItems();
		setWindowModified(false);
		updateUi();
		break;
	case Form::OpenError:
		QMessageBox::warning(this, QApplication::applicationName(),
							 tr("Error opening %1").arg(fileName));
		break;
	case Form::MagicError:
		QMessageBox::warning(this, QApplication::applicationName(),
							 tr("%1 is not a %2 file")
							 .arg(fileName)
							 .arg(QApplication::applicationName()));
		break;
	case Form::VersionError:
		QMessageBox::warning(this, QApplication::applicationName(),
							 tr("%1 needs a more recent version of %2")
							 .arg(fileName)
							 .arg(QApplication::applicationName()));
		break;
	default:
		Q_ASSERT(false);
	}
}

void MainWindow::formChanged()
{
	if (!isWindowModified())
		setWindowModified(true);
	updateUi();
}

void MainWindow::updateUi()
{
	bool empty = form->isEmpty();

	const QString fileName = windowFilePath();
	ui->actionReload->setEnabled(isWindowModified() && fileName != "" && fileName != tr("Untitled"));
	ui->actionSave->setEnabled(isWindowModified());
	ui->actionSaveAs->setDisabled(empty);
	ui->actionPagePreview->setDisabled(empty);
	ui->actionPrint->setDisabled(empty);
	ui->actionSaveAsPDF->setDisabled(empty);
	ui->actionSend->setDisabled(empty || !Mapi::instance().isAvailable());

	int selected = form->selectedItems().count();
	int itemCount = form->items().count();

	ui->actionCut->setEnabled(selected > 0);
	ui->actionCopy->setEnabled(selected > 0);
	ui->actionDelete->setEnabled(selected > 0);
	ui->actionSelectAll->setEnabled(itemCount > 0);

	ui->actionMoveForwards->setEnabled(selected == 1);
	ui->actionMoveBackwards->setEnabled(selected == 1);

	ui->actionAlignLeft->setEnabled(selected > 1);
	ui->actionAlignRight->setEnabled(selected > 1);
	ui->actionAlignTop->setEnabled(selected > 1);
	ui->actionAlignBottom->setEnabled(selected > 1);
	ui->actionPaste->setEnabled(form->canPaste());

	ui->actionShrinkWidth->setEnabled(selected > 1);
	ui->actionGrowWidth->setEnabled(selected > 1);
	ui->actionPageWidth->setEnabled(selected >= 1);
	ui->actionShrinkHeight->setEnabled(selected > 1);
	ui->actionGrowHeight->setEnabled(selected > 1);
	ui->actionPageHeight->setEnabled(selected >= 1);
	ui->actionShrinkBoth->setEnabled(selected > 1);
	ui->actionGrowBoth->setEnabled(selected > 1);
	ui->actionPageBoth->setEnabled(selected >= 1);

	int pageCount = form->pageCount();
	int currentPage = form->currentPage();

	ui->actionFirstPage->setEnabled(pageCount > 1 && currentPage > 1);
	ui->actionPreviousPage->setEnabled(pageCount > 1 && currentPage > 1);
	ui->actionNextPage->setEnabled(pageCount > 1 && currentPage < pageCount);
	ui->actionLastPage->setEnabled(pageCount > 1 && currentPage < pageCount);

	fontCombo->blockSignals(true);

	if (selected == 0)
	{
		bool hasData = form->hasPrintData();
		const QFont font = form->font();
		fontCombo->setCurrentFont(font);
		ui->actionBold->setEnabled(hasData);
		ui->actionBold->setChecked(font.bold());
		ui->actionItalic->setEnabled(hasData);
		ui->actionItalic->setChecked(font.italic());

		horzAlignGroup->setDisabled(true);
		ui->actionBox->setDisabled(true);

		formWidget->update(form);
	}
	else
	{
		const QList<GraphicsItem*> items = form->selected();
		Q_ASSERT(items.count() > 0);

		const SelectedProperties sel(items);

		foreach (QAction* action, horzAlignGroup->actions())
			action->setChecked(false);

		if (!sel.fontName.isEmpty())
			fontCombo->setCurrentFont(QFont(sel.fontName));
		fontCombo->setEditText(sel.fontName);

		ui->actionBold->setEnabled(true);
		ui->actionItalic->setEnabled(true);
		ui->actionBold->setChecked(sel.bold);
		ui->actionItalic->setChecked(sel.italic);

		horzAlignGroup->setEnabled(true);
		foreach (QAction* action, horzAlignGroup->actions())
		{
			if (action->data().value<quint32>() == sel.horzAlign)
			{
				action->setChecked(true);
				break;
			}
		}

		ui->actionBox->setEnabled(true);
		ui->actionBox->setChecked(sel.box);

		itemWidget->update(items);
	}

	fontCombo->blockSignals(false);
}

void MainWindow::horzAlign(QAction* action)
{
	quint32 align = action->data().toInt();
	foreach (GraphicsItem* item, form->selected())
		item->setHorzAlign(align);
}

void MainWindow::saveFormAsPdf()
{
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save As PDF"),
													recentFolder,
													tr("PDF Files (*.pdf);;All Files (*.*)"));
	if (fileName != "")
	{
		recentFolder = QFileInfo(fileName).path();

		if (!fileName.toLower().endsWith(".pdf"))
			fileName += ".pdf";

		form->createPdf(fileName);
	}
}

void MainWindow::updateRecentFileActions()
{
	QStringList existingFiles;
	foreach (const QString& fileName, recentFiles)
	{
		const QString scheme = QUrl(fileName).scheme().toLower();
		if (scheme == "ftp" || scheme == "http" || QFileInfo(fileName).exists())
			existingFiles << fileName;
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
			recentFileActions[i]->setVisible(false);
	}
	recentFilesSeparator->setVisible(!recentFiles.isEmpty());
}

void MainWindow::updateRecentFile(const QString& fileName)
{
	const QString native = QDir::toNativeSeparators(fileName);
	recentFiles.removeAll(native);
	recentFiles.prepend(native);
	updateRecentFileActions();
}

void MainWindow::email()
{
	QString fileName;

	{
		QTemporaryFile tempFile;
		tempFile.setAutoRemove(false);
		tempFile.open();
		fileName = tempFile.fileName();
		form->createPdf(fileName);
	}

	Mapi::instance().send(fileName, "Jupiter Editor", "form.pdf");
}

void MainWindow::print()
{
	form->print(this);
}

void MainWindow::preview()
{
	QPrintPreviewDialog printPreviewDialog(form->printer(), this, Qt::Window);
	connect(&printPreviewDialog, SIGNAL(paintRequested(QPrinter*)), form, SLOT(preview(QPrinter*)));
	printPreviewDialog.exec();
}

void MainWindow::paste()
{
	foreach (GraphicsItem* item, form->paste())
		connectItem(item);
}

void MainWindow::zoom(QAction* action)
{
	view->setZoom((GraphicsView::Zoom)action->data().toInt());
}

void MainWindow::size(QAction* action)
{
	form->size((Form::Size)action->data().toInt());
}

void MainWindow::updateViewMenu()
{
	ui->actionProperties->setChecked(ui->propertyWidget->isVisible());
}

void MainWindow::firstPage()
{
	form->setCurrentPage(1);
	updateUi();
}

void MainWindow::previousPage()
{
	form->setCurrentPage(form->currentPage() - 1);
	updateUi();
}

void MainWindow::nextPage()
{
	form->setCurrentPage(form->currentPage() + 1);
	updateUi();
}

void MainWindow::lastPage()
{
	form->setCurrentPage(form->pageCount());
	updateUi();
}
