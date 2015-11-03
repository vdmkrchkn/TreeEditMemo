#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QScopedPointer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //        
    QTranslator translator;
    //
    QStringList translationDirs;
    translationDirs
            << QString(".")
            << QString("../translations")
            << QString("../../TreeEditMemo/translations");
    QStringListIterator tsDirsIterator(translationDirs);
    //
    bool bTranslationLoaded = false;
    while(!bTranslationLoaded && tsDirsIterator.hasNext())
    {
        bTranslationLoaded = translator.load(QLocale::system(), QString("TreeEditMemo"),
                                             QString("_"), tsDirsIterator.next());
    }
    //
    if (bTranslationLoaded)
        a.installTranslator(&translator);
    else
        QMessageBox::warning(0, QObject::tr("Translation"), QObject::tr("Not loaded"));
    //    
    QScopedPointer <MainWindow> mainWindow(new MainWindow);    
    mainWindow->show();
    //
    return a.exec();
}
