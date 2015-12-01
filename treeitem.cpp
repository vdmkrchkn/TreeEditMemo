#include "treeitem.h"

Person::Person(const QString &rcName, int nValue, bool bEnabled, int nVariant)
    : name(rcName), age(nValue), isMarried(bEnabled), sex(nVariant){}

TreeItem::TreeItem(const Person &p, TreeItem *parent)
    : person(p),parentItem(parent){}

TreeItem::~TreeItem()
{
    qDeleteAll(this->childItems);
}

void TreeItem::appendChild(TreeItem *child)
{
    this->childItems.append(child);
}

TreeItem *TreeItem::child(int row) const
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

int TreeItem::row() const
{
     if (this->parentItem)
        return this->parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
     else
        return 0;
}

TreeItem *TreeItem::parent() const
{
    return this->parentItem;
}
