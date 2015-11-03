#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void onActionOpenTriggered();
    void openRecentFile();
    void onActionAboutActivated();
    void updateActions();
    void closeSubWindow();
    void closeCurSubWindow();
    void closeAllSubWindows();
private:
    Ui::MainWindow *ui;
    QMdiArea *mdiArea;
    QString wTitle;
    // динамический разделитель меню File
    QAction *separatorFileAction;
    // список последних открывавшихся файлов
    QStringList recentFiles;
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActions[MaxRecentFiles];
    // динамический разделитель меню Window
    QAction *separatorWindowAction;
    // список текущих открытых файлов
    QStringList curOpenFiles;
    QList<QAction*> curOpenFilesActions;
    // загрузка файла данных
    bool loadFile(const QString &filename);
    // сохранение файла данных
    bool saveFile(const QString &filename);
    // действия, совершаемые при открытии файла
    void setCurrentFile(const QString &filename);
    // возвращает имя файла без полного пути
    QString strippedFileName(const QString &filename);
    // обновляет меню File
    void updateRecentFileActions();
};

class TreeEditor : public QMdiSubWindow
{
    Q_OBJECT
public:
    TreeEditor(const QString &name, QMdiSubWindow *parent = 0);
    QString eName;
signals:
    void closed(const QString&);
protected:
    void closeEvent(QCloseEvent *closeEvent);
private:
    QAction *eAction;
};

#endif // MAINWINDOW_H
