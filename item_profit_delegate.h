#ifndef ITEM_PROFIT_DELEGATE_H
#define ITEM_PROFIT_DELEGATE_H

#include <QStyledItemDelegate>

class item_profit_delegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit item_profit_delegate(QObject* parent = 0);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

#endif // ITEM_PROFIT_DELEGATE_H
