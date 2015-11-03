#include "treeitem.h"

Person::Person(const QString &rcName, int nValue, bool bEnabled, int nVariant)
    : name(rcName), age(nValue), isMarried(bEnabled), sex(nVariant){}

TreeItem::TreeItem(const Person &p, TreeItem *parent)
{
    this->person = p;    
    this->parentItem = parent;
}

TreeItem::~TreeItem()
{
    qDeleteAll(this->childItems);
}

void TreeItem::appendChild(TreeItem *child)
{
    this->childItems.append(child);
}

TreeItem *TreeItem::child(int row)
{
    return this->childItems.value(row);
}

int TreeItem::childCount() const
{
    return this->childItems.count();
}

int TreeItem::columnCount() const
{
    return 4;
}

QVariant TreeItem::data(int column) const
{
    switch (column) {
    case 0:
        return this->person.name;
    case 1:
        return this->person.age;
    case 2:
        return this->person.isMarried;
    case 3:
        return this->person.sex;
    default:
        return QVariant();
    }
}

int TreeItem::row() const
{
     if (this->parentItem)
        return this->parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
     else
        return 0;
}

TreeItem *TreeItem::parent()
{
    return this->parentItem;
}
