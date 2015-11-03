#include "treemodel.h"
#include <QtDebug>

TreeModel::TreeModel(const QString &data, QObject *parent)
     : QAbstractItemModel(parent)
 {
     this->rootItem = new TreeItem();
     //
     m_Items << QString("Unknown") << QString("Male") << QString("Female");     
     setupModelData(data.split(QString("\n")), this->rootItem);
 }

 TreeModel::~TreeModel()
 {
     delete rootItem;
 }

 const QStringList &TreeModel::getItemNames() const
 {
     return m_Items;
 }

 int TreeModel::columnCount(const QModelIndex &parent) const
 {
     if (parent.isValid())
         return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
     else
         return rootItem->columnCount();
 }

 QVariant TreeModel::data(const QModelIndex &index, int role) const
 {
    if (index.isValid() && index.row() < rowCount() && index.column() < columnCount())
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        const Person &rcData = item->getPerson();
        switch (role)
        {
            case Qt::DisplayRole:
            case Qt::EditRole:
              switch (index.column())
              {
                case 0:
                  return rcData.name;
                case 1:
                  return rcData.age;
                case 2:
                  return rcData.isMarried;
                case 3:
                  if (role == Qt::DisplayRole)
                    return m_Items[rcData.sex];
                  else
                    return rcData.sex;
              }
            case Qt::CheckStateRole:
              switch (index.column())
              {
                case 2:
                  return rcData.isMarried ? Qt::Checked : Qt::Unchecked;
              }
        }
    }
    //
    return QVariant();
 }

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()
            && index.row() < rowCount() && index.column() < columnCount()
            && (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::CheckStateRole))
    {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        Person &rData = item->getPerson();
        switch (role)
        {
          case Qt::DisplayRole:
          case Qt::EditRole:
            switch (index.column())
            {
              case 0:
                rData.name = value.toString();
                break;
              case 1:
                rData.age = value.toInt();
                break;
              case 3:
                rData.sex = value.toInt();
                break;
              default:
                return false;
            }
            break;
          case Qt::CheckStateRole:
            switch (index.column())
            {
              case 2:
                rData.isMarried = (value.toInt() == Qt::Checked);
                break;
              default:
                return false;
            }
        }
        //
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

 Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
 {
     Qt::ItemFlags nReturn = QAbstractItemModel::flags(index);
     //
     nReturn |= Qt::ItemIsDropEnabled;
     if (index.isValid())
     {
       nReturn |= Qt::ItemIsDragEnabled;
       if (index.column() == 2)
         nReturn |= Qt::ItemIsUserCheckable;
       else
         nReturn |= Qt::ItemIsEditable;
     }
     //
     return nReturn;
 }

 QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
 {                 
     if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
         switch (section)
         {
           case 0:
             return QString("Name");
           case 1:
             return QString("Age");
           case 2:
             return QString("Married");
           case 3:
             return QString("Sex");
         }         
     //
     return QAbstractItemModel::headerData(section, orientation, role);
 }

 QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
             const
 {    
     if (!hasIndex(row, column, parent))
         return QModelIndex();
     //
     TreeItem *parentItem;
     // для корневого узла предок не определен
     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());
     //
     TreeItem *childItem = parentItem->child(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex TreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();

     TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
     TreeItem *parentItem = childItem->parent();

     if (parentItem == rootItem)
         return QModelIndex();

     return createIndex(parentItem->row(), 0, parentItem);
 }

 int TreeModel::rowCount(const QModelIndex &parent) const
 {
     TreeItem *parentItem;
     if (parent.column() > 0)
         return 0;
     //
     if (!parent.isValid())
         parentItem = rootItem;
     else
         parentItem = static_cast<TreeItem*>(parent.internalPointer());
     //
     return parentItem->childCount();
 }

 void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
 {     
     QList<TreeItem*> parents;
     QList<int> indentations;
     parents << parent;
     indentations << 0;
     // цикл по строкам
     foreach (QString line, lines)
     {         
         int position = 0;  // позиция 1-го непробельного символа в line
         while (position < line.length())
         {
             if (line.mid(position, 1) != " ")
                 break;
             position++;
         }

         QString lineData = line.mid(position).trimmed();

         if (!lineData.isEmpty())
         {
             QStringList columnStrings = lineData.split(",");
             QChar male = columnStrings[3].toUpper()[0];
             int iMale = 0;
             if(male == 'M')
                iMale = 1;
             else if(male == 'F')
                iMale = 2;
             Person curItem(columnStrings[0],columnStrings[1].toInt(),columnStrings[2].toInt(),iMale);
             //
             if (position > indentations.last())
             {
                 // The last child of the current parent is now the new parent
                 // unless the current parent has no children.
                 if (parents.last()->childCount() > 0)
                 {
                     parents << parents.last()->child(parents.last()->childCount()-1);
                     indentations << position;
                 }
             }
             else
             {
                 while (position < indentations.last() && parents.count() > 0)
                 {
                     parents.pop_back();
                     indentations.pop_back();
                 }
             }
             parents.last()->appendChild(new TreeItem(curItem, parents.last()));
         }
     }
 }
