#ifndef OFFER_INFO_H
#define OFFER_INFO_H

#include <QDateTime>

class QXmlStreamReader;
class QXmlStreamWriter;

class offer_info
{
public:
    enum class type
    {
        buy,
        sell
    };

    enum class state
    {
        open,
        cancelled,
        finished,
        expired
    };

public:
    offer_info() = default;
    offer_info(qlonglong id);
    offer_info(qlonglong id, const offer_info& offer);
    offer_info(QXmlStreamReader& xml);

    void from(const offer_info& offer);

    qlonglong id() const;

    type target() const;
    void set_target(type target);

    state status() const;
    void set_staus(state status);

    qlonglong item_id() const;
    void set_item_id(qlonglong id);

    int amount() const;
    void add_amount(int amount);
    void sub_amount(int amount);
    void set_amount(int amount);

    int price() const;
    void set_price(int price);

    int total_price() const;

    int fee() const;
    void set_fee(int fee);

    int total_fee() const;
    void add_total_fee(int fee);
    void set_total_fee(int total);

    bool flag() const;
    void set_flag(bool flag);
    void toggle_flag();

    QDateTime expires() const;
    void set_expires(const QDateTime& expires);
    void refresh();

    bool operator==(const offer_info& r) const;

    void read_from_xml(QXmlStreamReader& xml);
    void append_to_xml(QXmlStreamWriter& xml);

protected:
    qlonglong m_id = -1;

    type m_target = type::buy;
    state m_status = state::open;

    qlonglong m_item_id = 0;

    int m_amount = 0;
    int m_price = 0;

    int m_fee = 0;
    int m_total_fee = 0;

    bool m_flag = false;

    QDateTime m_expires;
};

#endif // OFFER_INFO_H
