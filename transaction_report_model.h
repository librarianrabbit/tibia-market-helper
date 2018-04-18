#ifndef TRANSACTION_REPORT_MODEL_H
#define TRANSACTION_REPORT_MODEL_H

#include <QAbstractTableModel>
#include "item_info.h"

class transaction_report_model : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum transaction_report_columns
    {
        name_column = 0,
        profit_column,
        total_bought_column,
        units_bought_column,
        avg_buy_price_column,
        total_sold_column,
        units_sold_column,
        avg_sell_price_column,
        total_profit_column,
        avg_profit_column,
        npc_buy_price_column,
        npc_sell_price_column
    };

public:
    explicit transaction_report_model(QObject* parent = 0);

    void set_items(const QList<item_info*>& items);

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

protected:
    QList<item_info*> m_items;

    int m_sort_column = name_column;
    Qt::SortOrder m_sort_order = Qt::AscendingOrder;
};

#endif // TRANSACTION_REPORT_MODEL_H
