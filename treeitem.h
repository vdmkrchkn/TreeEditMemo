#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

struct Person
{
    QString name;
    int age;
    bool isMarried;
    int sex;
    //
    Person(const QString &rcName = QString(),int nValue = 0,bool bEnabled = false,int nVariant = 0);
};

class TreeItem
{
public:
    TreeItem(const Person &p = Person(), TreeItem *parent = 0);
    ~TreeItem();
    // добавление дочернего узла
    void appendChild(TreeItem *child);
    // поиск дочернего узла с индексом row
    TreeItem *child(int row);
    // возвращает число дочерних узлов
    int childCount() const;
    // возвращает число полей узла
    int columnCount() const;
    // возвращает поле узла с индексом column
    QVariant data(int column) const;
    // определение позиции узла относительно родительского узла
    int row() const;
    // возвращает указатель на родительский узел
    TreeItem *parent();
    //
    Person &getPerson()
    {
        return this->person;
    }
    // реализовать setData    
private:            
    QList<TreeItem*> childItems; // список указателей на дочерние узлы
    TreeItem *parentItem;        // указатель на родительский узел    
    Person person;               // данные узла
};

#endif // TREEITEM_H
