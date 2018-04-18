#ifndef ITEM_INFO_H
#define ITEM_INFO_H

#include <QDateTime>
#include <QString>

class QXmlStreamReader;
class QXmlStreamWriter;

class item_info
{
public:
    enum profit : int
    {
        do_not_worth = 0,
        bad,
        medium,
        good,
        excelent
    };

public:
    item_info() = default;
    item_info(qlonglong id);
    item_info(qlonglong id, const item_info& item);
    item_info(QXmlStreamReader& xml);

    void from(const item_info& item);

    qlonglong id() const;

    QString name() const;
    void set_name(const QString& name);

    int bought() const;
    void add_bought(int bought);
    void set_bought(int bought);

    int buy_max() const;
    void set_buy_max(int max);

    int buy_min() const;
    void set_buy_min(int min);

    int buy_npc() const;
    void set_buy_npc(int price);

    void update_buy_price(int price);

    int buy_updates() const;
    void inc_buy_updates();
    void set_buy_updates(int updates);

    int sold() const;
    void add_sold(int sold);
    void set_sold(int sold);

    int sell_max() const;
    void set_sell_max(int max);

    int sell_min() const;
    void set_sell_min(int min);

    int sell_npc() const;
    void set_sell_npc(int price);

    void update_sell_price(int price);

    int sell_updates() const;
    void inc_sell_updates();
    void set_sell_updates(int updates);

    int total_bought() const;
    void add_total_bought(int bought);
    void set_total_bought(int bought);

    int average_buy_price() const;

    int total_sold() const;
    void add_total_sold(int sold);
    void set_total_sold(int sold);

    int average_sell_price() const;

    int total_profit() const;
    int average_profit_per_unit() const;

    profit flag() const;
    void set_flag(profit flag);

    QDateTime last_bought_time() const;
    void set_last_bought_time(const QDateTime& time);

    int last_bought_amount() const;
    void set_last_bought_amount(int amount);

    int last_bought_price() const;
    void set_last_bought_price(int price);

    QDateTime last_sold_time() const;
    void set_last_sold_time(const QDateTime& time);

    int last_sold_amount() const;
    void set_last_sold_amount(int amount);

    int last_sold_price() const;
    void set_last_sold_price(int price);

    void read_from_xml(QXmlStreamReader& xml);
    void append_to_xml(QXmlStreamWriter& xml);

    bool operator==(const item_info& r) const;

protected:
    qlonglong m_id = -1;

    QString m_name;

    int m_bought  = 0;
    int m_buy_max = 0;
    int m_buy_min = 0;
    int m_buy_npc = 0;
    int m_buy_updates = 0;

    int m_sold     = 0;
    int m_sell_max = 0;
    int m_sell_min = 0;
    int m_sell_npc = 0;
    int m_sell_updates = 0;

    profit m_flag = profit::medium;

    int m_total_bought = 0;
    int m_total_sold = 0;

    QDateTime m_last_bought_time;
    int m_last_bougth_amount = 0;
    int m_last_bought_price = 0;

    QDateTime m_last_sold_time;
    int m_last_sold_amount = 0;
    int m_last_sold_price = 0;
};

#endif // ITEM_INFO_H
