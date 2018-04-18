#include "transaction_report_model.h"
#include <QStringList>
#include <QLocale>

static QStringList g_columns = {
    "Name",
    "Profit",
    "Bought",
    "Units",
    "Price",
    "Sold",
    "Units",
    "Price",
    "Profit",
    "Average",
    "NPC Buy",
    "NPC Sell"
};

transaction_report_model::transaction_report_model(QObject* parent) :
    QAbstractTableModel(parent)
{
}

void transaction_report_model::set_items(const QList<item_info*>& items)
{
    this->beginRemoveRows(QModelIndex(), 0, this->rowCount());
    m_items.clear();
    this->endRemoveRows();

    this->beginInsertRows(QModelIndex(), 0, items.size() - 1);
    m_items = items;
    this->endInsertRows();

    this->sort(m_sort_column, m_sort_order);
}

int transaction_report_model::columnCount(const QModelIndex&) const
{
    return g_columns.size();
}

int transaction_report_model::rowCount(const QModelIndex&) const
{
    return m_items.size();
}

QVariant transaction_report_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            return g_columns.value(section);
        }
    }

    return QVariant();
}

QVariant transaction_report_model::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.row() < 0 || index.row() > m_items.size())
        {
            return QVariant();
        }

        item_info* item = m_items.at(index.row());

        switch (index.column())
        {
            case name_column:
                return item->name();

            case profit_column:
                return item->flag();

            case total_bought_column:
                return QLocale::system().toString(item->total_bought());

            case units_bought_column:
                return QLocale::system().toString(item->bought());

            case avg_buy_price_column:
                return QLocale::system().toString(item->average_buy_price());

            case total_sold_column:
                return QLocale::system().toString(item->total_sold());

            case units_sold_column:
                return QLocale::system().toString(item->sold());

            case avg_sell_price_column:
                return QLocale::system().toString(item->average_sell_price());

            case total_profit_column:
                return QLocale::system().toString(item->total_profit());

            case avg_profit_column:
                return QLocale::system().toString(item->average_profit_per_unit());

            case npc_buy_price_column:
                return QLocale::system().toString(item->buy_npc());

            case npc_sell_price_column:
                return QLocale::system().toString(item->sell_npc());
        }
    }
    else if (role == Qt::UserRole)
    {
        if (index.row() < 0 || index.row() > m_items.size())
        {
            return QVariant();
        }

        return m_items.at(index.row())->id();
    }

    return QVariant();
}

Qt::ItemFlags transaction_report_model::flags(const QModelIndex&) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void transaction_report_model::sort(int column, Qt::SortOrder order)
{
    m_sort_column = column;
    m_sort_order = order;

    auto by_name = [&](item_info* l, item_info* r)
    {
        if (l->name() == r->name()) return (order == Qt::AscendingOrder ? (l->id() >= r->id()) : (l->id() < r->id()));
        return (order == Qt::AscendingOrder ? (l->name() >= r->name()) : (l->name() < r->name()));
    };

    if (column == name_column)
    {
        qSort(m_items.begin(), m_items.end(), by_name);
    }
    else if (column == profit_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->flag() == r->flag()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->flag() >= r->flag()) : (l->flag() < r->flag()));
        });
    }
    else if (column == total_bought_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->total_bought() == r->total_bought()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->total_bought() >= r->total_bought()) : (l->total_bought() < r->total_bought()));
        });
    }
    else if (column == units_bought_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->bought() == r->bought()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->bought() >= r->bought()) : (l->bought() < r->bought()));
        });
    }
    else if (column == avg_buy_price_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->average_buy_price() == r->average_buy_price()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->average_buy_price() >= r->average_buy_price()) : (l->average_buy_price() < r->average_buy_price()));
        });
    }
    else if (column == total_sold_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->total_sold() == r->total_sold()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->total_sold() >= r->total_sold()) : (l->total_sold() < r->total_sold()));
        });
    }
    else if (column == units_sold_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->sold() == r->sold()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->sold() >= r->sold()) : (l->sold() < r->sold()));
        });
    }
    else if (column == avg_sell_price_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->average_sell_price() == r->average_sell_price()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->average_sell_price() >= r->average_sell_price()) : (l->average_sell_price() < r->average_sell_price()));
        });
    }
    else if (column == total_profit_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->total_profit() == r->total_profit()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->total_profit() >= r->total_profit()) : (l->total_profit() < r->total_profit()));
        });
    }
    else if (column == avg_profit_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->average_profit_per_unit() == r->average_profit_per_unit()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->average_profit_per_unit() >= r->average_profit_per_unit()) : (l->average_profit_per_unit() < r->average_profit_per_unit()));
        });
    }
    else if (column == npc_buy_price_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->buy_npc() == r->buy_npc()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->buy_npc() >= r->buy_npc()) : (l->buy_npc() < r->buy_npc()));
        });
    }
    else if (column == npc_sell_price_column)
    {
        qSort(m_items.begin(), m_items.end(), [&](item_info* l, item_info* r)
        {
            if (l->sell_npc() == r->sell_npc()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->sell_npc() >= r->sell_npc()) : (l->sell_npc() < r->sell_npc()));
        });
    }

    this->dataChanged(this->createIndex(0, name_column), this->createIndex(this->rowCount(), npc_sell_price_column));
}
