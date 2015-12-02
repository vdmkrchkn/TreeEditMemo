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
    void closeSubWindow(QMdiSubWindow *subWindow);
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
    // обновляет меню File
    void updateRecentFileActions();
    //
    bool eventFilter(QObject *target, QEvent *event);
};
/* рабочее окружение документа
class EditorSubWindow : public QWidget
{
    Q_OBJECT
public:
    EditorSubWindow(const QString &name, QWidget *parent = 0);
    QString m_Name;
signals:
    void closed(const QString&);
protected:
    void closeEvent(QCloseEvent *closeEvent);
private:
    QAction *eAction;
};
*/
#endif // MAINWINDOW_H
