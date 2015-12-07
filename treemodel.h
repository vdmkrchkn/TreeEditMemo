#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <treeitem.h>

class TreeModel : public QAbstractItemModel
 {
     Q_OBJECT
 public:
     TreeModel(const QString &data, QObject *parent = 0);
     ~TreeModel();
     // возвращает список элементов перечислимого типа
     const QStringList &getItemNames() const;
     // возвращает элемент данных, соответствующий модельному индексу `index`
     virtual QVariant data(const QModelIndex &index, int role) const;
     // редактирование элемента модели
     virtual bool setData(const QModelIndex &index, const QVariant &value,
                          int role = Qt::EditRole);
     // возвращает флаги элемента модели, используемые делегатом контроллера
     virtual Qt::ItemFlags flags(const QModelIndex &index) const;
     //
     virtual Qt::DropActions supportedDropActions() const;
     // возвращает список типов MIME
     virtual QStringList mimeTypes() const;
     // возвращает сериализованные данные в форматe MIME
     virtual QMimeData *mimeData(const QModelIndexList & indexes) const;
     // drop данных в позицию, соответствующую (`row`, `column`,`parent`)
     virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                       int row, int column, const QModelIndex &parent);
     // возвращает данные заголовка
     virtual QVariant headerData(int section, Qt::Orientation orientation,
                                 int role = Qt::DisplayRole) const;
     // создание модельного индекса для дочернего элемента с заданными строкой,
     // столбцом и индексом
     virtual QModelIndex index(int row, int column,
                       const QModelIndex &parent) const;
     // возвращает индекс родительского элемента, соответствующего заданному индексу
     virtual QModelIndex parent(const QModelIndex &index) const;
     // определение количества строк для текущего элемента
     virtual int rowCount(const QModelIndex &parent) const;
     // определение количества столбцов для текущего элемента
     virtual int columnCount(const QModelIndex &parent) const;
     // удаление элементов
     virtual bool removeRows(int row, int count,
                             const QModelIndex &parent = QModelIndex());
 private:     
     // инициализация данных
     void setupModelData(const QStringList &lines, TreeItem *parent);
     // возвращает узел для текущего индекса
     TreeItem *itemFromIndex(const QModelIndex &index) const;
     // корень модели
     TreeItem *rootItem;     
     // список элементов перечислимого типа
     QStringList m_Items;
     //
     QString m_MimeType;
 };

#endif // TREEMODEL_H
