#ifndef MARKET_INFO_H
#define MARKET_INFO_H

#include <QList>
#include "item_info.h"
#include "offer_info.h"

class QXmlStreamReader;
class QXmlStreamWriter;

class market_info
{
public:
    struct details
    {
        int buy_items = 0;
        int sell_items = 0;
        int buy_total = 0;
        int sell_total = 0;
    };

public:
    static int calculate_fee(int total_price, int pfee = 1, int minimum_fee = 20, int maximum_fee = 1000);

public:
    item_info* item_by_id(qlonglong id);

    item_info* item_import(const item_info& i);
    bool item_remove(qlonglong id);

    offer_info* offer_by_id(qlonglong id);

    offer_info* offer_import(const offer_info& o);
    bool offer_accept(offer_info& o, int amount);
    bool offer_increase(offer_info& o, int amount);
    bool offer_reprice(offer_info& o, int price, int amount = 0);
    bool offer_decrease(offer_info& o, int amount);
    bool offer_update(offer_info& o, const offer_info& n);
    bool offer_cancel(qlonglong id);
    void offer_flag(offer_info& o) const;
    void offer_flag(offer_info& o, bool flag) const;

    details get_details() const;

    QList<item_info*> items();

    QList<offer_info> buy_history();
    QList<offer_info> sell_history();

    QList<offer_info*> buy_offers();
    QList<offer_info*> sell_offers();

    int count_offers() const;

    void clear_items();

    void clear_offers();

    void clear_buy_history();
    void clear_sell_history();

    void clear_buy_offers();
    void clear_sell_offers();

    QList<item_info>& item_cache();

    int apply_item_cache();

    QList<offer_info>& buy_cache();
    QList<offer_info>& sell_cache();

    int apply_buy_cache();
    int apply_sell_cache();

    int initial_balance() const;
    void set_initial_balance(int balance);

    int initial_capital() const;
    void set_initial_capital(int total);

    int balance() const;
    void set_balance(int balance);

    QDateTime last_transaction() const;
    void set_last_transaction(const QDateTime& last);

    int total_buy_offers() const;
    void set_total_buy_offers(int total);

    int total_sell_offers() const;
    void set_total_sell_offers(int total);

    int total_items_bought() const;
    void set_total_items_bought(int total);

    int total_items_sold() const;
    void set_total_items_sold(int total);

    int total_bought() const;
    void set_total_bought(int total);

    int total_sold() const;
    void set_total_sold(int total);

    QDateTime last_bought() const;
    void set_last_bought(const QDateTime& last);

    QDateTime last_sold() const;
    void set_last_sold(const QDateTime& last);

    void read_from_xml(QXmlStreamReader& xml);
    void append_to_xml(QXmlStreamWriter& xml);

    void reset();

protected:
    item_info* item_load(const item_info& i);

    offer_info* offer_load(const offer_info& o);
    bool offer_finish(offer_info& o);

    qlonglong gen_item_id();
    qlonglong gen_offer_id();

protected:
    QList<item_info> m_items;

    QList<item_info> m_item_cache;

    QList<offer_info> m_offers;

    QList<offer_info> m_buy_history;
    QList<offer_info> m_sell_history;

    QList<offer_info> m_buy_cache;
    QList<offer_info> m_sell_cache;

    int m_balance = 0;

    int m_initial_balance = 0;
    int m_initial_capital = 0;

    QDateTime m_last_transaction;

    int m_total_buy_offers = 0;
    int m_total_sell_offers = 0;

    int m_total_items_bought = 0;
    int m_total_items_sold = 0;

    int m_total_bought = 0;
    int m_total_sold = 0;

    QDateTime m_last_bought;
    QDateTime m_last_sold;

    qlonglong m_item_id_counter = 0;
    qlonglong m_offer_id_counter = 0;
};

#endif // MARKET_INFO_H
