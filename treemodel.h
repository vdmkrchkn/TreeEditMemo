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
     // геттер для m_Items
     const QStringList &getItemNames() const;
     // возвращает элемент данных, соответствующий модельному индексу <<index>>
     virtual QVariant data(const QModelIndex &index, int role) const;
     // редактирование элемента модели
     virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
     // возвращает флаги элемента модели, используемые делегатом контроллера
     virtual Qt::ItemFlags flags(const QModelIndex &index) const;
     // возвращает данные заголовка
     virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
     // возвращает модельный индекс узла <<row>> поля <<column>>
     virtual QModelIndex index(int row, int column,
                       const QModelIndex &parent = QModelIndex()) const;
     virtual QModelIndex parent(const QModelIndex &index) const;
     // возвращает количество строк в модели
     virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
     virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

 private:
     void setupModelData_(const QStringList &lines, TreeItem *parent);
     void setupModelData(const QStringList &lines, TreeItem *parent);
     TreeItem *rootItem;     
     // список элементов перечислимого типа
     QStringList m_Items;
 };

#endif // TREEMODEL_H
