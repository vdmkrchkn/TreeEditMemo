#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "combo-delegate.h"
#include "treemodel.h"
#include "about_dialog.h"

#ifdef MODEL_TEST
#include "modeltest.h"
#endif

namespace
{
    // возвращает имя файла без полного пути
    QString strippedFileName(const QString &rcFullFileName)
    {
        return QFileInfo(rcFullFileName).fileName();
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), wTitle(tr("Family Edit"))
{
    ui->setupUi(this);        
    mdiArea = new QMdiArea;
    setCentralWidget(mdiArea);    
    //
    connect(mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow*)),this,SLOT(updateActions()));    
    // обработка нажатия Файл->Открыть
    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(onActionOpenTriggered()));
    // создание динамического меню последних открывавшихся файлов
    separatorFileAction = ui->menu_File->addSeparator();
    separatorFileAction->setVisible(false);
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActions[i] = new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i], SIGNAL(triggered()), this, SLOT(openRecentFile()));
        ui->menu_File->addAction(recentFileActions[i]);
    }
    // обработка нажатия Окно->Закрыть все окна
    connect(ui->action_Close_All, SIGNAL(triggered()), this, SLOT(closeAllSubWindows()));
    // создание динамического меню открытых файлов
    separatorWindowAction = ui->menu_Window->addSeparator();
    separatorWindowAction->setVisible(false);
    // обработка нажатия Справка->О программе
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onActionAboutActivated()));    
}

MainWindow::~MainWindow()
{
    delete ui;
    delete separatorFileAction;
    delete separatorWindowAction;
    delete mdiArea;
    qDeleteAll(curOpenFilesActions);
    for (int i = 0; i < MaxRecentFiles; ++i)
        delete recentFileActions[i];
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    mdiArea->closeAllSubWindows();
    if (!mdiArea->subWindowList().isEmpty())
        event->ignore();
    else
        event->accept();
}

void MainWindow::onActionOpenTriggered()
{
    // диалоговое окно выбора файла данных
    QString filename = QFileDialog::getOpenFileName(this,tr("Open data file"),
                                                "../../TreeEditMemo/families",
                                                QString("Family files (*.fm)"));
    if(!filename.isEmpty())    
        loadFile(filename);
}

void MainWindow::onActionAboutActivated()
{
    About_Dialog infoD(this);
    infoD.exec();
}

void MainWindow::updateActions()
{
    QMdiSubWindow *subWindow = mdiArea->activeSubWindow();
    if(subWindow)
        setWindowTitle(QString("%1[*] - %2").arg(subWindow->accessibleName())
                                                .arg(wTitle));
    else
        setWindowTitle(wTitle);
    ui->action_Close_All->setVisible(!curOpenFiles.empty());
}

void MainWindow::closeSubWindow(QMdiSubWindow *subWindow)
{
//    EditorSubWindow *subWindow = qobject_cast<EditorSubWindow *>(sender());
//    if(subWindow){}
    int idx = mdiArea->subWindowList().indexOf(subWindow);
    if(idx >= 0)
    {
        curOpenFiles.removeAt(idx);
        if(idx < curOpenFilesActions.size()){
            curOpenFilesActions[idx]->setVisible(false);
            curOpenFilesActions.removeAt(idx);
        }
        //
        subWindow->close();
        //
        separatorWindowAction->setVisible(!curOpenFiles.empty());
    }
}

void MainWindow::closeCurSubWindow()
{
    QAction *pAction = qobject_cast<QAction *>(sender());
    if (pAction)
    {
        int idx = curOpenFilesActions.indexOf(pAction);
        if(idx >= 0)
        {
            curOpenFilesActions[idx]->setVisible(false);
            curOpenFilesActions.removeAt(idx);
            //
            if(idx < curOpenFiles.size())
                curOpenFiles.removeAt(idx);
            //
            mdiArea->subWindowList()[idx]->close();
        }
        //
        separatorWindowAction->setVisible(!curOpenFiles.empty());        
    }
}

void MainWindow::closeAllSubWindows()
{
    foreach (QAction *qAction, curOpenFilesActions) {
        qAction->setVisible(false);
    }
    curOpenFilesActions.clear();
    curOpenFiles.clear();
    //
    separatorWindowAction->setVisible(false);
    //
    mdiArea->closeAllSubWindows();
}

void MainWindow::openRecentFile()
{
    QAction *pAction = qobject_cast<QAction *>(sender());
    if (pAction)
        loadFile(pAction->data().toString());
}

bool MainWindow::loadFile(const QString &filename)
{
    if(curOpenFiles.contains(filename))
        return false;
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return false;
    // создание модели
    TreeModel *m_pTreeModel = new TreeModel(file.readAll());
    file.close();
    // тестирование модели
#ifdef MODEL_TEST
    (void) new ModelTest(m_pTreeModel,this);
#endif
    //
    setCurrentFile(filename);    
    // создание представления
    QTreeView *subTreeView = new QTreeView;
    subTreeView->setModel(m_pTreeModel);
    // создание делегата
    ComboDelegate *pDelegate = new ComboDelegate(m_pTreeModel);
    subTreeView->setItemDelegateForColumn(3,pDelegate);
    //
//    subTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    subTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    subTreeView->setAllColumnsShowFocus(true);
    // перемещение внутри дерева
    subTreeView->setDragDropMode(QAbstractItemView::InternalMove);
    subTreeView->setDropIndicatorShown(true);
    //
//    EditorSubWindow *editor = new EditorSubWindow(documentName,subTreeView);
//    connect(editor,SIGNAL(closed(QString)),this,SLOT(closeSubWindow()));
    QMdiSubWindow *subWindow = mdiArea->addSubWindow(subTreeView);
    subWindow->setAccessibleName(strippedFileName(filename));
    subWindow->installEventFilter(this);
    subWindow->show();
    //
    return true;
}

void MainWindow::setCurrentFile(const QString &filename)
{
    setWindowModified(false); // нет несохраненных изменений    
    if(!filename.isEmpty())
    {
        // обновление списка недавно открывавшихся файлов
        recentFiles.removeAll(filename);
        recentFiles.prepend(filename);
        //
        updateRecentFileActions();
        //
        curOpenFiles.append(filename);
        //
        QAction *curFileCloseAction = new QAction(this);
        curFileCloseAction->setText(strippedFileName(filename));
        curOpenFilesActions.append(curFileCloseAction);
        //
        connect(curFileCloseAction, SIGNAL(triggered()), this, SLOT(closeCurSubWindow()));
        //
        separatorWindowAction->setVisible(true);
        ui->menu_Window->addAction(curFileCloseAction);
    }    
}

void MainWindow::updateRecentFileActions()
{
    // удаление несуществующих файлов
    QMutableStringListIterator i(recentFiles);
    while(i.hasNext())
        if(!QFile::exists(i.next()))
            i.remove();
    // формирование названий пунктов меню File
    for(int j = 0; j < MaxRecentFiles; ++j)
        if (j < recentFiles.count())
        {
            QString text = QString("&%1 %2")
                .arg(j + 1)
                .arg(strippedFileName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else
            recentFileActions[j]->setVisible(false);
    //
    separatorFileAction->setVisible(!recentFiles.empty());
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    if(event->type() == QEvent::Close){
        QMdiSubWindow *subWindow = qobject_cast<QMdiSubWindow*>(target);
        if(subWindow)
            closeSubWindow(subWindow);
    }
    return QObject::eventFilter(target,event);
}
/*
EditorSubWindow::EditorSubWindow(const QString &name, QWidget *parent)
 : QWidget(parent), m_Name(name)
{
    this->eAction = new QAction(this);
    this->eAction->setCheckable(true);
    connect(eAction, SIGNAL(triggered()), this, SLOT(close()));
    setAttribute(Qt::WA_DeleteOnClose);
}

void EditorSubWindow::closeEvent(QCloseEvent *closeEvent)
{
    emit closed(m_Name);
    closeEvent->accept();
}
*/
