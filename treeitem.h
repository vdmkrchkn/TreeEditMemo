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
    Person(const QString &rcName = QString(),int nValue = 0,bool bEnabled = false
            ,int nVariant = 0);
};

class TreeItem
{
public:
    TreeItem(const Person &p = Person(), TreeItem *parent = 0);
    ~TreeItem();
    // добавление дочернего узла в позицию `row`. В случае -1 добавляется в конец
    void addChild(TreeItem *child, int row = -1);
    // удаление дочернего узла с индексом `row`
    TreeItem *removeChild(int row);
    // поиск дочернего узла с индексом `row`
    TreeItem *childAt(int row) const;
    // возвращает число дочерних узлов
    int childCount() const;
    // возвращает число полей узла
    int columnCount() const;    
    // определение позиции узла относительно родительского узла
    int row() const;
    // возвращает указатель на родительский узел
    TreeItem *parent() const;
    // возвращает данные узла
    Person &getPerson() { return this->person; }
private:            
    QList<TreeItem*> childItems; // список указателей на дочерние узлы
    Person person;               // данные узла
    TreeItem *parentItem;        // указатель на родительский узел        
};

#endif // TREEITEM_H
