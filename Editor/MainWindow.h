#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Form;
class GraphicsView;
class GraphicsItem;
class ItemWidget;
class FormWidget;

class QCloseEvent;
class QRectF;
class QAction;
class QActionGroup;
class QFontComboBox;

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

protected:
	void closeEvent(QCloseEvent* event);

private slots:
	void about();
	void init();
	void newForm();
	void openForm();
	void openRecentFile();
	void reload();
	void saveForm();
	void saveFormAs();
	void openPrintData();
	void fullScreen();
	void insertObject(QAction* action);
	void doneRubberBanding(const QRectF& rect);
	void selectionChanged();
	void formChanged();
	void horzAlign(QAction* action);
	void saveFormAsPdf();
	void email();
	void print();
	void preview();
	void paste();
	void zoom(QAction* action);
	void size(QAction* action);
	void updateViewMenu();
	void firstPage();
	void previousPage();
	void nextPage();
	void lastPage();

private:
	void loadSettings();
	void saveSettings() const;
	void connectForm();
	void connectItems();
	void connectItem(GraphicsItem* item);
	bool saveChanges();
	void loadFile(const QString& fileName);
	void updateUi();

	Ui::MainWindow *ui;
	QActionGroup* insertGroup;
	QActionGroup* horzAlignGroup;
	QActionGroup* zoomGroup;
	QActionGroup* sizeGroup;
	Form* form;
	GraphicsView* view;
	ItemWidget* itemWidget;
	FormWidget* formWidget;
	QFontComboBox* fontCombo;
	QString recentFolder;

	enum { MaxRecentFiles = 5 };
	QStringList recentFiles;
	QAction* recentFileActions[MaxRecentFiles];
	QAction* recentFilesSeparator;
	void updateRecentFileActions();
	void updateRecentFile(const QString& fileName);
};

#endif // MAINWINDOW_H
