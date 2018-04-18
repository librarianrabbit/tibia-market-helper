#ifndef MARKET_MODEL_H
#define MARKET_MODEL_H

#include <QAbstractTableModel>
#include "offer_info.h"

class market_info;

class market_model : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum market_columns
    {
        name_column = 0,
        total_price_column,
        price_piece_column,
        amount_column,
        total_fee_column,
        updates_column,
        ends_at_column
    };

    enum flag_update
    {
        toggle_flag = 0,
        set_flag,
        clear_flag
    };

public:
    explicit market_model(market_info& market, offer_info::type target, QObject* parent = 0);

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    offer_info* import_offer(const offer_info& o);
    bool accept_offer(offer_info& o, int amount);
    bool update_offer(offer_info& o, const offer_info& n);
    bool cancel_offer(qlonglong id);
    bool flag_offer(offer_info& o, int update);
    int clear_offer_flags();

    QModelIndex find_offer_by_id(qlonglong id) const;
    QModelIndex find_offer_by_item_id(qlonglong id) const;

    void load_offers();
    void clear();

protected:
    void update_offers();

protected:
    offer_info::type m_target = offer_info::type::buy;

    market_info& m_market;

    QList<offer_info*> m_offers;

    int m_sort_column = name_column;
    Qt::SortOrder m_sort_order = Qt::AscendingOrder;
};

#endif // MARKET_MODEL_H
