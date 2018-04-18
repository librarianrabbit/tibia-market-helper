#include "market_view.h"

market_view::market_view(QWidget* parent) :
    QTableView(parent)
{
}

void market_view::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    emit current_index_changed(current, previous);
    return QTableView::currentChanged(current, previous);
}
