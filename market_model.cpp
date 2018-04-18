#include "market_model.h"
#include "market_info.h"
#include <QStringList>
#include <QLocale>
#include <QBrush>

static QStringList g_columns = {
    "Name",
    "Total Price",
    "Price Piece",
    "Amount",
    "Total Fee",
    "Updates",
    "Ends at"
};

market_model::market_model(market_info& market, offer_info::type target, QObject* parent) :
    QAbstractTableModel(parent),
    m_target(target),
    m_market(market)
{
}

int market_model::rowCount(const QModelIndex&) const
{
    return m_offers.size();
}

int market_model::columnCount(const QModelIndex&) const
{
    return g_columns.size();
}

QVariant market_model::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant market_model::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        offer_info* o = m_offers.value(index.row(), nullptr);

        if (o != nullptr)
        {
            item_info* item = m_market.item_by_id(o->item_id());

            switch (index.column())
            {
                case name_column:
                    if (item != nullptr) return item->name();
                    break;

                case total_price_column:
                    return QLocale::system().toString(o->total_price());

                case price_piece_column:
                    return QLocale::system().toString(o->price());

                case amount_column:
                    return QLocale::system().toString(o->amount());

                case total_fee_column:
                    return QLocale::system().toString(o->total_fee());

                case updates_column:
                    if (item != nullptr) return (m_target == offer_info::type::buy ? item->buy_updates() : item->sell_updates());
                    break;

                case ends_at_column:
                    return o->expires().toString();
            }
        }
    }
    else if (role == Qt::ForegroundRole)
    {
        offer_info* o = m_offers.value(index.row(), nullptr);

        if (o != nullptr && o->flag())
        {
            return QBrush(Qt::white);
        }

        return QVariant();
    }
    else if (role == Qt::BackgroundRole)
    {
        offer_info* o = m_offers.value(index.row(), nullptr);

        if (o != nullptr && o->flag())
        {
            return QBrush(Qt::darkBlue);
        }

        return QVariant();
    }
    else if (role == Qt::UserRole)
    {
        offer_info* o = m_offers.value(index.row(), nullptr);

        if (o != nullptr)
        {
            return o->id();
        }
    }
    else if (role == Qt::UserRole + 1)
    {
        offer_info* o = m_offers.value(index.row(), nullptr);

        if (o != nullptr)
        {
            return o->item_id();
        }
    }

    return QVariant();
}

Qt::ItemFlags market_model::flags(const QModelIndex&) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void market_model::sort(int column, Qt::SortOrder order)
{
    m_sort_column = column;
    m_sort_order = order;

    auto by_name = [&](offer_info* l, offer_info* r)
    {
        item_info* li = m_market.item_by_id(l->item_id());
        item_info* ri = m_market.item_by_id(r->item_id());

        if (li == nullptr) return (order == Qt::AscendingOrder ? true : false);
        if (ri == nullptr) return (order == Qt::AscendingOrder ? false : true);

        if (li->name() == ri->name()) return (order == Qt::AscendingOrder ? (li->id() >= ri->id()) : (li->id() < ri->id()));

        return (order == Qt::AscendingOrder ? (li->name() >= ri->name()) : (li->name() < ri->name()));
    };

    if (column == name_column)
    {
        qSort(m_offers.begin(), m_offers.end(), by_name);
    }
    else if (column == total_price_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            if (l->total_price() == r->total_price()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->total_price() >= r->total_price()) : (l->total_price() < r->total_price()));
        });
    }
    else if (column == price_piece_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            if (l->price() == r->price()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->price() >= r->price()) : (l->price() < r->price()));
        });
    }
    else if (column == amount_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            if (l->amount() == r->amount()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->amount() >= r->amount()) : (l->amount() < r->amount()));
        });
    }
    else if (column == total_fee_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            if (l->total_fee() == r->total_fee()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->total_fee() >= r->total_fee()) : (l->total_fee() < r->total_fee()));
        });
    }
    else if (column == updates_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            item_info* li = m_market.item_by_id(l->item_id());
            item_info* ri = m_market.item_by_id(r->item_id());

            if (li == nullptr) return (order == Qt::AscendingOrder ? true : false);
            if (ri == nullptr) return (order == Qt::AscendingOrder ? false : true);

            int lu = (m_target == offer_info::type::buy ? li->buy_updates() : li->sell_updates());
            int ru = (m_target == offer_info::type::buy ? ri->buy_updates() : ri->sell_updates());

            if (lu == ru) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (lu >= ru) : (lu < ru));
        });
    }
    else if (column == ends_at_column)
    {
        qSort(m_offers.begin(), m_offers.end(), [&](offer_info* l, offer_info* r)
        {
            if (l->expires() == r->expires()) return by_name(l, r);
            return (order == Qt::AscendingOrder ? (l->expires() >= r->expires()) : (l->expires() < r->expires()));
        });
    }

    emit dataChanged(this->createIndex(0, name_column), this->createIndex(this->rowCount(), ends_at_column));
}

offer_info* market_model::import_offer(const offer_info& o)
{
    offer_info* offer = m_market.offer_import(o);

    if (offer != nullptr)
    {
        int row = this->rowCount();

        this->beginInsertRows(QModelIndex(), row, row);
        this->update_offers();
        this->endInsertRows();

        this->sort(m_sort_column, m_sort_order);
    }

    return offer;
}

bool market_model::accept_offer(offer_info& o, int amount)
{
    int row = -1;

    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->id() == o.id())
        {
            row = i;
            break;
        }
    }

    if (row == -1)
    {
        return false;
    }

    bool remove = (o.amount() == amount);

    if (remove)
    {
        this->beginRemoveRows(QModelIndex(), row, row);
    }

    bool ret = m_market.offer_accept(o, amount);

    if (remove)
    {
        if (ret)
        {
            this->update_offers();
        }

        this->endRemoveRows();
    }

    if (!ret)
    {
        return false;
    }

    if (!remove)
    {
        emit dataChanged(this->createIndex(row, name_column), this->createIndex(row, ends_at_column));
    }

    this->sort(m_sort_column, m_sort_order);

    return true;
}

bool market_model::update_offer(offer_info& o, const offer_info& n)
{
    int row = -1;

    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->id() == o.id())
        {
            row = i;
            break;
        }
    }

    if (row == -1)
    {
        return false;
    }

    bool remove = (n.amount() == 0);

    if (remove)
    {
        this->beginRemoveRows(QModelIndex(), row, row);
    }

    bool ret = m_market.offer_update(o, n);

    if (remove)
    {
        if (ret)
        {
            this->update_offers();
        }

        this->endRemoveRows();
    }

    if (!ret)
    {
        return false;
    }

    if (!remove)
    {
        emit dataChanged(this->createIndex(row, name_column), this->createIndex(row, ends_at_column));
    }

    this->sort(m_sort_column, m_sort_order);

    return true;
}

bool market_model::cancel_offer(qlonglong id)
{
    int row = -1;

    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->id() == id)
        {
            row = i;
            break;
        }
    }

    if (row == -1)
    {
        return false;
    }

    bool ret = m_market.offer_cancel(id);

    if (ret)
    {
        this->beginRemoveRows(QModelIndex(), row, row);
        this->update_offers();
        this->endRemoveRows();

        this->sort(m_sort_column, m_sort_order);
    }

    return ret;
}

bool market_model::flag_offer(offer_info& o, int update)
{
    int row = -1;

    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->id() == o.id())
        {
            row = i;
            break;
        }
    }

    if (row == -1)
    {
        return false;
    }

    if (update == toggle_flag)
    {
        m_market.offer_flag(o);
    }
    else
    {
        m_market.offer_flag(o, (update == set_flag));
    }

    emit dataChanged(this->createIndex(row, name_column), this->createIndex(row, ends_at_column));

    return true;
}

int market_model::clear_offer_flags()
{
    int counter = 0;

    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->flag())
        {
            m_market.offer_flag(*m_offers[i]);

            ++counter;

            emit dataChanged(this->createIndex(i, name_column), this->createIndex(i, ends_at_column));
        }
    }

    return counter;
}

QModelIndex market_model::find_offer_by_id(qlonglong id) const
{
    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->id() == id)
        {
            return this->createIndex(i, name_column);
        }
    }

    return QModelIndex();
}

QModelIndex market_model::find_offer_by_item_id(qlonglong id) const
{
    for (int i = 0; i < m_offers.size(); ++i)
    {
        if (m_offers.at(i)->item_id() == id)
        {
            return this->createIndex(i, name_column);
        }
    }

    return QModelIndex();
}

void market_model::load_offers()
{
    if (m_target == offer_info::type::buy)
    {
        QList<offer_info*> buy = m_market.buy_offers();

        if (buy.isEmpty())
        {
            return;
        }

        this->beginInsertRows(QModelIndex(), 0, (buy.size() - 1));
        m_offers = buy;
        this->endInsertRows();
    }
    else
    {
        QList<offer_info*> sell = m_market.sell_offers();

        if (sell.isEmpty())
        {
            return;
        }

        this->beginInsertRows(QModelIndex(), 0, (sell.size() - 1));
        m_offers = sell;
        this->endInsertRows();
    }

    this->sort(m_sort_column, m_sort_order);
}

void market_model::clear()
{
    int count = this->rowCount();

    if (count == 0)
    {
        return;
    }

    if (m_target == offer_info::type::buy)
    {
        m_market.clear_buy_offers();
    }
    else
    {
        m_market.clear_sell_offers();
    }

    this->beginRemoveRows(QModelIndex(), 0, (count - 1));
    this->update_offers();
    this->endRemoveRows();

    this->sort(m_sort_column, m_sort_order);
}

void market_model::update_offers()
{
    m_offers = (m_target == offer_info::type::buy ? m_market.buy_offers() : m_market.sell_offers());
}
