#include "market_info.h"
#include <cmath>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

int market_info::calculate_fee(int total_price, int pfee, int minimum_fee, int maximum_fee)
{
    if (total_price == 0)
    {
        return 0;
    }

    total_price = std::ceil(static_cast<double>(total_price) / static_cast<double>(100.0 * pfee));

    if (total_price <= minimum_fee)
    {
        return minimum_fee;
    }
    else if (total_price >= maximum_fee)
    {
        return maximum_fee;
    }

    return total_price;
}

item_info* market_info::item_by_id(qlonglong id)
{
    for (item_info& i : m_items)
    {
        if (i.id() == id)
        {
            return &i;
        }
    }

    return nullptr;
}

item_info* market_info::item_import(const item_info& i)
{
    if (m_items.contains(i))
    {
        return nullptr;
    }

    qlonglong id = this->gen_item_id();

    if (id == -1)
    {
        return nullptr;
    }

    m_items.append(item_info(id, i));

    return &m_items.last();
}

item_info* market_info::item_load(const item_info& i)
{
    if (i.id() == -1)
    {
        return this->item_import(i);
    }

    if (m_items.contains(i))
    {
        return nullptr;
    }

    m_items.append(i);

    return &m_items.last();
}

bool market_info::item_remove(qlonglong id)
{
    for (int i = 0; i < m_items.size(); ++i)
    {
        if (m_items.at(i).id() == id)
        {
            m_items.removeAt(i);
            return true;
        }
    }

    return false;
}

offer_info* market_info::offer_by_id(qlonglong id)
{
    for (offer_info& o : m_offers)
    {
        if (o.id() == id)
        {
            return &o;
        }
    }

    return nullptr;
}

offer_info* market_info::offer_import(const offer_info& o)
{
    if (m_offers.contains(o))
    {
        return nullptr;
    }

    qlonglong id = this->gen_offer_id();

    if (id == -1)
    {
        return nullptr;
    }

    m_offers.append(offer_info(id, o));

    offer_info* offer = &m_offers.last();

    if (offer->target() == offer_info::type::buy)
    {
        m_balance -= offer->total_price();
    }

    offer->set_total_fee(offer->fee());

    m_balance -= offer->fee();

    m_last_transaction = QDateTime::currentDateTime();

    return offer;
}

offer_info* market_info::offer_load(const offer_info& o)
{
    if (o.id() == -1)
    {
        return this->offer_import(o);
    }

    if (m_offers.contains(o))
    {
        return nullptr;
    }

    m_offers.append(o);

    return &m_offers.last();
}

bool market_info::offer_accept(offer_info& o, int amount)
{
    if (!m_offers.contains(o))
    {
        return false;
    }

    o.sub_amount(amount);

    if (o.target() == offer_info::type::sell)
    {
        int revenue = (amount * o.price());

        m_total_items_sold += amount;
        m_total_sold += revenue;

        m_balance += revenue;

        m_last_sold = QDateTime::currentDateTime();

        item_info* item = this->item_by_id(o.item_id());

        if (item != nullptr)
        {
            item->add_sold(amount);
            item->add_total_sold(revenue);

            item->update_sell_price(o.price());

            item->set_last_sold_amount(amount);
            item->set_last_sold_price(o.price());
            item->set_last_sold_time(m_last_sold);
        }
    }
    else
    {
        int payment = (amount * o.price());

        m_total_items_bought += amount;
        m_total_bought += payment;

        m_last_bought = QDateTime::currentDateTime();

        item_info* item = this->item_by_id(o.item_id());

        if (item != nullptr)
        {
            item->add_bought(amount);
            item->add_total_bought(payment);

            item->update_buy_price(o.price());

            item->set_last_bought_amount(amount);
            item->set_last_bought_price(o.price());
            item->set_last_bought_time(m_last_bought);
        }
    }

    m_last_transaction = QDateTime::currentDateTime();

    if (o.amount() == 0)
    {
        return this->offer_finish(o);
    }

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    return true;
}

bool market_info::offer_increase(offer_info& o, int amount)
{
    if (!m_offers.contains(o))
    {
        return false;
    }

    o.add_amount(amount);

    if (o.target() == offer_info::type::buy)
    {
        m_balance -= (amount * o.price());
    }

    o.set_fee(o.fee() > 0 ? market_info::calculate_fee(o.total_price()) : 0);
    o.add_total_fee(o.fee());

    m_balance -= o.fee();

    o.refresh();

    m_last_transaction = QDateTime::currentDateTime();

    if (o.amount() == 0)
    {
        return this->offer_cancel(o.id());
    }

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    return true;
}

bool market_info::offer_reprice(offer_info& o, int price, int amount)
{
    if (!m_offers.contains(o))
    {
        return false;
    }

    int old_price = o.price();

    if (o.target() == offer_info::type::buy)
    {
        m_balance += o.total_price();
    }

    o.set_price(price);

    if (amount != 0)
    {
        o.set_amount(amount);
    }

    if (o.target() == offer_info::type::buy)
    {
        m_balance -= o.total_price();
    }

    o.set_fee(o.fee() > 0 ? market_info::calculate_fee(o.total_price()) : 0);
    o.add_total_fee(o.fee());

    m_balance -= o.fee();

    o.refresh();

    if (o.target() == offer_info::type::buy)
    {
        if (price > old_price)
        {
            item_info* item = this->item_by_id(o.item_id());

            if (item != nullptr)
            {
                item->inc_buy_updates();
            }
        }
    }
    else
    {
        if (price < old_price)
        {
            item_info* item = this->item_by_id(o.item_id());

            if (item != nullptr)
            {
                item->inc_sell_updates();
            }
        }
    }

    m_last_transaction = QDateTime::currentDateTime();

    if (o.amount() == 0)
    {
        return this->offer_cancel(o.id());
    }

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    return true;
}

bool market_info::offer_decrease(offer_info& o, int amount)
{
    if (!m_offers.contains(o))
    {
        return false;
    }

    o.sub_amount(amount);

    if (o.target() == offer_info::type::buy)
    {
        m_balance += (amount * o.price());
    }

    o.set_fee(o.fee() > 0 ? market_info::calculate_fee(o.total_price()) : 0);
    o.add_total_fee(o.fee());

    m_balance -= o.fee();

    o.refresh();

    m_last_transaction = QDateTime::currentDateTime();

    if (o.amount() == 0)
    {
        return this->offer_cancel(o.amount());
    }

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    return true;
}

bool market_info::offer_update(offer_info& o, const offer_info& n)
{
    if (!m_offers.contains(o))
    {
        return false;
    }

    if (o.target() != n.target())
    {
        return false;
    }

    o.set_item_id(n.item_id());

    if (o.price() == n.price())
    {
        if (o.amount() > n.amount())
        {
            this->offer_decrease(o, (o.amount() - n.amount()));
        }
        else if (o.amount() < n.amount())
        {
            this->offer_increase(o, (n.amount() - o.amount()));
        }
    }
    else
    {
        this->offer_reprice(o, n.price(), n.amount());
    }

    o.set_expires(n.expires());

    m_last_transaction = QDateTime::currentDateTime();

    if (o.amount() == 0)
    {
        return this->offer_cancel(o.id());
    }

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    return true;
}

bool market_info::offer_finish(offer_info& o)
{
    if (o.amount() != 0)
    {
        return false;
    }

    if (o.target() == offer_info::type::buy)
    {
        ++m_total_buy_offers;
    }
    else
    {
        ++m_total_sell_offers;
    }

    o.set_staus(offer_info::state::finished);

    if (o.target() == offer_info::type::buy)
    {
        m_buy_history.append(o);
    }
    else
    {
        m_sell_history.append(o);
    }

    m_offers.removeOne(o);

    m_last_transaction = QDateTime::currentDateTime();

    return true;
}

bool market_info::offer_cancel(qlonglong id)
{
    offer_info* o = this->offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    if (o->target() == offer_info::type::buy)
    {
        m_balance += o->total_price();
    }

    o->set_staus(offer_info::state::cancelled);

    if (o->target() == offer_info::type::buy)
    {
        m_buy_history.append(*o);
    }
    else
    {
        m_sell_history.append(*o);
    }

    m_offers.removeOne(*o);

    m_last_transaction = QDateTime::currentDateTime();

    return true;
}

void market_info::offer_flag(offer_info& o) const
{
    o.toggle_flag();
}

void market_info::offer_flag(offer_info& o, bool flag) const
{
    o.set_flag(flag);
}

market_info::details market_info::get_details() const
{
    details d;

    for (const offer_info& o : m_offers)
    {
        if (o.target() == offer_info::type::buy)
        {
            d.buy_items += o.amount();
            d.buy_total += o.total_price();
        }
        else
        {
            d.sell_items += o.amount();
            d.sell_total += o.total_price();
        }
    }

    return d;
}

qlonglong market_info::gen_item_id()
{
    qlonglong id = m_item_id_counter;

    if (id == -1)
    {
        id = 0;
    }

    int limit = m_items.size() + 1;

    while (this->item_by_id(id) != nullptr)
    {
        if (limit-- <= 0)
        {
            return -1;
        }

        ++id;
    }

    return (m_item_id_counter = id);
}

qlonglong market_info::gen_offer_id()
{
    qlonglong id = m_offer_id_counter;

    if (id == -1)
    {
        id = 0;
    }

    int limit = m_offers.size() + 1;

    while (this->offer_by_id(id) != nullptr)
    {
        if (limit-- <= 0)
        {
            return -1;
        }

        ++id;
    }

    return (m_offer_id_counter = id);
}

QList<item_info*> market_info::items()
{
    QList<item_info*> ni;

    for (item_info& i : m_items)
    {
        ni.append(&i);
    }

    return ni;
}

QList<offer_info> market_info::buy_history()
{
    return m_buy_history;
}

QList<offer_info> market_info::sell_history()
{
    return m_sell_history;
}

QList<offer_info*> market_info::buy_offers()
{
    QList<offer_info*> no;

    for (offer_info& o : m_offers)
    {
        if (o.target() == offer_info::type::buy)
        {
            no.append(&o);
        }
    }

    return no;
}

QList<offer_info*> market_info::sell_offers()
{
    QList<offer_info*> no;

    for (offer_info& o : m_offers)
    {
        if (o.target() == offer_info::type::sell)
        {
            no.append(&o);
        }
    }

    return no;
}

int market_info::count_offers() const
{
    return m_offers.size();
}

void market_info::clear_items()
{
    m_items.clear();
}

void market_info::clear_offers()
{
    m_offers.clear();
}

void market_info::clear_buy_history()
{
    m_buy_history.clear();
}

void market_info::clear_sell_history()
{
    m_sell_history.clear();
}

void market_info::clear_buy_offers()
{
    QMutableListIterator<offer_info> no(m_offers);

    while (no.hasNext())
    {
        if (no.next().target() == offer_info::type::buy)
        {
            no.remove();
        }
    }
}

void market_info::clear_sell_offers()
{
    QMutableListIterator<offer_info> no(m_offers);

    while (no.hasNext())
    {
        if (no.next().target() == offer_info::type::sell)
        {
            no.remove();
        }
    }
}

QList<item_info>& market_info::item_cache()
{
    return m_item_cache;
}

int market_info::apply_item_cache()
{
    int count = 0;

    for (item_info& i : m_item_cache)
    {
        if (this->item_load(i) != nullptr)
        {
            ++count;
        }
    }

    m_item_cache.clear();

    return count;
}

QList<offer_info>& market_info::buy_cache()
{
    return m_buy_cache;
}

QList<offer_info>& market_info::sell_cache()
{
    return m_sell_cache;
}

int market_info::apply_buy_cache()
{
    this->clear_buy_offers();

    int count = 0;

    for (offer_info& o : m_buy_cache)
    {
        if (this->offer_load(o) != nullptr)
        {
            ++count;
        }
    }

    m_buy_cache.clear();

    return count;
}

int market_info::apply_sell_cache()
{
    this->clear_sell_offers();

    int count = 0;

    for (offer_info& o : m_sell_cache)
    {
        if (this->offer_load(o) != nullptr)
        {
            ++count;
        }
    }

    m_sell_cache.clear();

    return count;
}

int market_info::initial_balance() const
{
    return m_initial_balance;
}

void market_info::set_initial_balance(int balance)
{
    m_initial_balance = balance;
}

int market_info::initial_capital() const
{
    return m_initial_capital;
}

void market_info::set_initial_capital(int total)
{
    m_initial_capital = total;
}

int market_info::balance() const
{
    return m_balance;
}

void market_info::set_balance(int balance)
{
    m_balance = balance;
}

QDateTime market_info::last_transaction() const
{
    return m_last_transaction;
}

void market_info::set_last_transaction(const QDateTime& last)
{
    m_last_transaction = last;
}

int market_info::total_buy_offers() const
{
    return m_total_buy_offers;
}

void market_info::set_total_buy_offers(int total)
{
    m_total_buy_offers = total;
}

int market_info::total_sell_offers() const
{
    return m_total_sell_offers;
}

void market_info::set_total_sell_offers(int total)
{
    m_total_sell_offers = total;
}

int market_info::total_items_bought() const
{
    return m_total_items_bought;
}

void market_info::set_total_items_bought(int total)
{
    m_total_items_bought = total;
}

int market_info::total_items_sold() const
{
    return m_total_items_sold;
}

void market_info::set_total_items_sold(int total)
{
    m_total_items_sold = total;
}

int market_info::total_bought() const
{
    return m_total_bought;
}

void market_info::set_total_bought(int total)
{
    m_total_bought = total;
}

int market_info::total_sold() const
{
    return m_total_sold;
}

void market_info::set_total_sold(int total)
{
    m_total_sold = total;
}

QDateTime market_info::last_bought() const
{
    return m_last_bought;
}

void market_info::set_last_bought(const QDateTime& last)
{
    m_last_bought = last;
}

QDateTime market_info::last_sold() const
{
    return m_last_sold;
}

void market_info::set_last_sold(const QDateTime& last)
{
    m_last_sold = last;
}

void market_info::read_from_xml(QXmlStreamReader& xml)
{
    m_item_cache.clear();

    m_buy_history.clear();
    m_sell_history.clear();

    m_buy_cache.clear();
    m_sell_cache.clear();

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "balance")
            {
                m_balance = xml.readElementText().toInt();
                continue;
            }

            if (xml.name() == "last")
            {
                m_last_transaction = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                continue;
            }

            if (xml.name() == "buy")
            {
                QXmlStreamAttributes attr = xml.attributes();
                m_total_buy_offers   = attr.value("offers").toInt();
                m_total_items_bought = attr.value("items").toInt();
                m_total_bought       = attr.value("total").toInt();
                m_last_bought        = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                continue;
            }

            if (xml.name() == "sell")
            {
                QXmlStreamAttributes attr = xml.attributes();
                m_total_sell_offers = attr.value("offers").toInt();
                m_total_items_sold  = attr.value("items").toInt();
                m_total_sold        = attr.value("total").toInt();
                m_last_sold         = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                continue;
            }

            if (xml.name() == "items")
            {
                m_item_id_counter = xml.attributes().value("lid").toLongLong();

                if (m_item_id_counter == -1)
                {
                    m_item_id_counter = 0;
                }

                while (!xml.atEnd() && !xml.hasError())
                {
                    token = xml.readNext();

                    if (token == QXmlStreamReader::StartElement)
                    {
                        if (xml.name() == "item")
                        {
                            m_item_cache.append(item_info(xml));
                            continue;
                        }

                        continue;
                    }

                    if (token == QXmlStreamReader::EndElement)
                    {
                        if (xml.name() == "items")
                        {
                            break;
                        }

                        continue;
                    }
                }

                continue;
            }

            if (xml.name() == "offers")
            {
                m_offer_id_counter = xml.attributes().value("lid").toLongLong();

                if (m_offer_id_counter == -1)
                {
                    m_offer_id_counter = 0;
                }

                while (!xml.atEnd() && !xml.hasError())
                {
                    token = xml.readNext();

                    if (token == QXmlStreamReader::StartElement)
                    {
                        if (xml.name() == "offer")
                        {
                            offer_info o(xml);

                            if (o.target() == offer_info::type::buy)
                            {
                                m_buy_cache.append(o);
                            }
                            else
                            {
                                m_sell_cache.append(o);
                            }

                            continue;
                        }

                        continue;
                    }

                    if (token == QXmlStreamReader::EndElement)
                    {
                        if (xml.name() == "offers")
                        {
                            break;
                        }

                        continue;
                    }
                }

                continue;
            }

            continue;
        }

        if (token == QXmlStreamReader::EndElement)
        {
            if (xml.name() == "market")
            {
                break;
            }

            continue;
        }
    }

    qSort(m_item_cache.begin(), m_item_cache.end(), [](const item_info& l , const item_info& r ) { return (l.id() <= r.id()); });

    qSort(m_buy_cache.begin() , m_buy_cache.end() , [](const offer_info& l, const offer_info& r) { return (l.id() <= r.id()); });
    qSort(m_sell_cache.begin(), m_sell_cache.end(), [](const offer_info& l, const offer_info& r) { return (l.id() <= r.id()); });

    m_initial_balance = m_balance;
}

void market_info::append_to_xml(QXmlStreamWriter& xml)
{
    xml.writeStartElement("market");

    xml.writeTextElement("balance", QString::number(m_balance));
    xml.writeTextElement("last", m_last_transaction.toString(Qt::ISODate));

    xml.writeStartElement("buy");
    xml.writeAttribute("offers", QString::number(m_total_buy_offers));
    xml.writeAttribute("items" , QString::number(m_total_items_bought));
    xml.writeAttribute("total" , QString::number(m_total_bought));
    xml.writeCharacters(m_last_bought.toString(Qt::ISODate));
    xml.writeEndElement();

    xml.writeStartElement("sell");
    xml.writeAttribute("offers", QString::number(m_total_sell_offers));
    xml.writeAttribute("items" , QString::number(m_total_items_sold));
    xml.writeAttribute("total" , QString::number(m_total_sold));
    xml.writeCharacters(m_last_sold.toString(Qt::ISODate));
    xml.writeEndElement();

    xml.writeStartElement("items");
    xml.writeAttribute("lid", QString::number(m_item_id_counter));

    for (item_info& i : m_items)
    {
        i.append_to_xml(xml);
    }

    xml.writeEndElement();

    xml.writeStartElement("offers");
    xml.writeAttribute("lid", QString::number(m_offer_id_counter));

    for (offer_info& o : m_offers)
    {
        if (o.target() == offer_info::type::buy)
        {
            o.append_to_xml(xml);
        }
    }

    for (offer_info& o : m_offers)
    {
        if (o.target() == offer_info::type::sell)
        {
            o.append_to_xml(xml);
        }
    }

    xml.writeEndElement();

    xml.writeEndElement();
}

void market_info::reset()
{
    this->clear_items();

    this->clear_offers();

    this->clear_buy_history();
    this->clear_sell_history();

    m_initial_balance = 0;
    m_balance = 0;

    m_last_transaction = QDateTime();

    m_total_buy_offers = 0;
    m_total_sell_offers = 0;

    m_total_items_bought = 0;
    m_total_items_sold = 0;

    m_total_bought = 0;
    m_total_sold = 0;

    m_item_id_counter = 0;
    m_offer_id_counter = 0;
}
