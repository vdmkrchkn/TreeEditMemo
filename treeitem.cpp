#include "treeitem.h"
#include <QXmlStreamWriter>
#include <QStack>

Person::Person(const QString &rcName, int nValue, bool bEnabled, int nVariant)
    : name(rcName), age(nValue), isMarried(bEnabled), sex(nVariant){}

TreeItem::TreeItem(const Person &p, TreeItem *parent)
    : person(p),parentItem(parent){}

TreeItem::~TreeItem()
{
    qDeleteAll(this->childItems);
}

void TreeItem::addChild(TreeItem *child, int row)
{
    child->parentItem = this;
    this->childItems.insert(row == -1 ? this->childCount() : row, child);
}

TreeItem *TreeItem::removeChild(int row)
{
    TreeItem *item = childAt(row);
    item->parentItem = 0;
    this->childItems.removeAt(row);
    return item;
}

TreeItem *TreeItem::childAt(int row) const
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

void TreeItem::serialize(QXmlStreamWriter *writer, TreeItem *item)
{
    if(item->parent()){ // пропустить корневой элемент
        const Person & rcPerson = item->getPerson();
        // запись данных элемента
        writer->writeStartElement("person");
        writer->writeAttribute("Name",rcPerson.name);
        writer->writeAttribute("Age",QString::number(rcPerson.age));
        writer->writeAttribute("Married",rcPerson.isMarried ? "1" : "0");
        writer->writeAttribute("Sex",QString::number(rcPerson.sex));
    }
    // запись данных дочерних элементов
    foreach (TreeItem *child, item->childItems) {
        serialize(writer,child);
    }
    if(item->parent())
        writer->writeEndElement();  // </person>
}

void TreeItem::deserialize(QXmlStreamReader *reader, TreeItem *item,
                           const int &row)
{
    QStack<TreeItem *> itemStack;
    itemStack.push(item);
    while (!reader->atEnd()) {
        reader->readNext();
        if (reader->isStartElement()) {
            if (reader->name() == "person") {
                const QString name = reader->attributes().value("Name").toString();
                int age = reader->attributes().value("Age").toInt();
                bool isMarried = reader->attributes().value("Married") == "1";
                int sex = reader->attributes().value("Sex").toInt();
                TreeItem *child = new TreeItem(Person(name,age,isMarried,sex),
                                                 item);
                if(itemStack.top() == item)
                    itemStack.top()->addChild(child,row);
                else
                    itemStack.top()->addChild(child);
                itemStack.push(child);
            }
         }
        else if (reader->isEndElement()) {
            if (reader->name() == "person")
                itemStack.pop();            
        }
        Q_ASSERT(!itemStack.isEmpty());
    }
    itemStack.pop();
    Q_ASSERT(itemStack.isEmpty());
}
