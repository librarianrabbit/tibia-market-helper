#include "item_profit_delegate.h"
#include "item_info.h"
#include <QPainter>

item_profit_delegate::item_profit_delegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void item_profit_delegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    bool ok = false;
    item_info::profit flag = static_cast<item_info::profit>(index.data(Qt::DisplayRole).toInt(&ok));

    if (!ok)
    {
        flag = item_info::medium;
    }

    painter->fillRect(option.rect, index.data(Qt::BackgroundRole).value<QBrush>());

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    QPixmap img;

    switch (flag)
    {
        case item_info::do_not_worth:
            img.load(":/img/do_not_worth.png");
            break;

        case item_info::bad:
            img.load(":/img/bad.png");
            break;

        case item_info::medium:
            img.load(":/img/medium.png");
            break;

        case item_info::good:
            img.load(":/img/good.png");
            break;

        case item_info::excelent:
            img.load(":/img/excelent.png");
            break;
    }

    img = img.scaledToHeight(option.rect.height() + 4);

    painter->save();
    painter->translate(option.rect.center().x() - (img.width() / 2), option.rect.top() - 1);
    painter->drawPixmap(img.rect().center() - QPoint(img.width() / 2, img.height() / 2), img);
    painter->restore();
}
