#include "treemodel.h"
#include <QMimeData>

 TreeModel::TreeModel(const QString &data, QObject *parent)
     : QAbstractItemModel(parent), rootItem(new TreeItem()),
       m_MimeType("application/vnd.text.list")
 {
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

 QVariant TreeModel::data(const QModelIndex &index, int role) const
 {
    if (index.isValid()
            && index.row() < rowCount(parent(index))
            && index.column() < columnCount(parent(index)))
    {
        TreeItem *item = itemFromIndex(index);
        if(!item)
            return QVariant();
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
              if (index.column() == 2)
                  return rcData.isMarried ? Qt::Checked : Qt::Unchecked;
        }
    }
    //
    return QVariant();
 }

 bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid()
            && index.row() < rowCount(parent(index))
            && index.column() < columnCount(parent(index)))
    {
        TreeItem *item = itemFromIndex(index);
        if(!item)
            return false;
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
        default:
            return false;
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

 Qt::DropActions TreeModel::supportedDropActions() const
 {
     return Qt::CopyAction | Qt::MoveAction;
 }

 QStringList TreeModel::mimeTypes() const
 {         
    return QStringList() << m_MimeType;
 }

 QMimeData *TreeModel::mimeData(const QModelIndexList &indexes) const
 {
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            QString text = data(index, Qt::DisplayRole).toString();
            stream << text;
        }
    }
    mimeData->setData(m_MimeType, encodedData);
    return mimeData;
 }

 bool TreeModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent)
 {
    if (action == Qt::IgnoreAction)
        return true;
    //
    if (!data || !data->hasFormat(m_MimeType) || column > 0)
        return false;
    //
    TreeItem *parentItem = itemFromIndex(parent);
    if (!parentItem)
        return false;
    // десериализация переносимых данных
    QByteArray encodedData = data->data(m_MimeType);
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<Person> personItems;
    while (!stream.atEnd()) {
        QString data[4];
        for(int i = 0; i < 4; ++i)
            stream >> data[i];
        personItems << Person(data[0],
                data[1].toInt(),data[2].toInt(),data[3].toInt());
    }
    //
    int beginRow = 0;
    if (row != -1)
        // drop перед элементом <<row>>
        beginRow = row;
    else if (parent.isValid())
        // drop в дочерние <<parent>>
        beginRow = parent.row();
    else
        beginRow = rootItem->childCount();
    // вставка в соответствующий индекс
    beginInsertRows(parent,beginRow,beginRow + personItems.count() - 1);
//    insertRows(beginRow, rows, parent);
    foreach (const Person &rcPerson, personItems) {
        parentItem->appendChild(new TreeItem(rcPerson, parentItem));
//        QModelIndex idx = index(beginRow, 0, parent);
//        setData(idx, text);
//        beginRow++;
    }
    endInsertRows();
    return true;
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
     // получение узла-предка
     TreeItem *parentItem = itemFromIndex(parent);
     // получение текущего узла
     TreeItem *childItem = parentItem->childAt(row);
     if (childItem)
         return createIndex(row, column, childItem);
     else
         return QModelIndex();
 }

 QModelIndex TreeModel::parent(const QModelIndex &index) const
 {
     if (!index.isValid())
         return QModelIndex();
     //
     TreeItem *childItem = itemFromIndex(index);
     if(!childItem)
         return QModelIndex();
     //
     TreeItem *parentItem = childItem->parent();
     if(!parentItem || parentItem == rootItem)
         return QModelIndex();
     //
     return createIndex(parentItem->row(), 0, parentItem);
 }

 int TreeModel::rowCount(const QModelIndex &parent) const
 {
     if (parent.column() > 0)
         return 0;
     TreeItem *parentItem = itemFromIndex(parent);
     if(!parentItem)
         return 0;
     return parentItem->childCount();   // число дочерних элементов
 }

 int TreeModel::columnCount(const QModelIndex &parent) const
 {
     TreeItem* parentItem = itemFromIndex(parent);
     return parentItem->columnCount();
 }

 bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
 {
     TreeItem* parentItem = itemFromIndex(parent);
     beginRemoveRows(parent,row,row + count - 1);
     for(int i = 0; i < count; ++i)
        delete parentItem->removeChild(row);
     endRemoveRows();
     return true;
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
                     parents << parents.last()->childAt(parents.last()->childCount()-1);
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
             // Append a new item to the current parent's list of children.
             parents.last()->appendChild(new TreeItem(curItem, parents.last()));
         }
     }
 }

 TreeItem *TreeModel::itemFromIndex(const QModelIndex &index) const
 {
     // для корневого узла индексом является корень модели
     if (!index.isValid())
         return rootItem;
     else
         // получаем указатель с помощью встроенного метода
         return static_cast<TreeItem*>(index.internalPointer());
 }
