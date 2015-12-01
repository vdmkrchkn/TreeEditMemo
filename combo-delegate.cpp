// combo-delegate.cpp

#include "combo-delegate.h"
#include "treemodel.h"
#include <QtWidgets>

ComboDelegate::ComboDelegate(QObject *pParent): QStyledItemDelegate(pParent){}

QWidget *ComboDelegate::createEditor(
  QWidget *pParent,
  const QStyleOptionViewItem &rcOption,
  const QModelIndex &rcIndex) const
{
  QComboBox *pComboBox = new QComboBox(pParent);
  const TreeModel *pcModel = qobject_cast <const TreeModel *> (rcIndex.model());
  if (pcModel || !rcOption.showDecorationSelected)
    pComboBox->addItems(pcModel->getItemNames());
  //
  return pComboBox;
}

void ComboDelegate::setEditorData(QWidget *pEditor, const QModelIndex &rcIndex) const
{
  const int cnValue = rcIndex.model()->data(rcIndex, Qt::EditRole).toInt();
  QComboBox *pComboBox = qobject_cast <QComboBox *> (pEditor);
  if (pComboBox)
    pComboBox->setCurrentIndex(cnValue);
}

void ComboDelegate::setModelData(QWidget *pEditor,QAbstractItemModel *pModel,const QModelIndex &rcIndex) const
{
  QComboBox *pComboBox = qobject_cast <QComboBox *> (pEditor);
  if (pComboBox)
  {
    const int cnValue = pComboBox->currentIndex();
    pModel->setData(rcIndex, cnValue, Qt::EditRole);
  }
}

void ComboDelegate::updateEditorGeometry(
  QWidget *pEditor,
  const QStyleOptionViewItem &rcOption,
  const QModelIndex &) const
{
  pEditor->setGeometry(rcOption.rect);
}

// End of File
