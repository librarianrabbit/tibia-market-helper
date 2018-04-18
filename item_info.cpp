#include "item_info.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

item_info::item_info(qlonglong id) :
    m_id(id)
{
}

item_info::item_info(qlonglong id, const item_info& item) :
    m_id(id)
{
    this->from(item);
}

item_info::item_info(QXmlStreamReader& xml)
{
    this->read_from_xml(xml);
}

void item_info::from(const item_info& item)
{
    m_name = item.m_name;

    m_buy_npc  = item.m_buy_npc;
    m_sell_npc = item.m_sell_npc;

    if (item.m_flag < do_not_worth)
    {
        m_flag = do_not_worth;
    }
    else if (item.m_flag > excelent)
    {
        m_flag = excelent;
    }
    else
    {
        m_flag = item.m_flag;
    }
}

qlonglong item_info::id() const
{
    return m_id;
}

QString item_info::name() const
{
    return m_name;
}

void item_info::set_name(const QString& name)
{
    m_name = name;
}

int item_info::bought() const
{
    return m_bought;
}

void item_info::add_bought(int bought)
{
    m_bought += bought;
}

void item_info::set_bought(int bought)
{
    m_bought = bought;
}

int item_info::buy_max() const
{
    return m_buy_max;
}

void item_info::set_buy_max(int max)
{
    m_buy_max = max;
}

int item_info::buy_min() const
{
    return m_buy_min;
}

void item_info::set_buy_min(int min)
{
    m_buy_min = min;
}

int item_info::buy_npc() const
{
    return m_buy_npc;
}

void item_info::set_buy_npc(int price)
{
    m_buy_npc = price;
}

void item_info::update_buy_price(int price)
{
    if (price < m_buy_min || m_buy_min == 0)
    {
        m_buy_min = price;
    }

    if (price > m_buy_max || m_buy_max == 0)
    {
        m_buy_max = price;
    }
}

int item_info::buy_updates() const
{
    return m_buy_updates;
}

void item_info::inc_buy_updates()
{
    ++m_buy_updates;
}

void item_info::set_buy_updates(int updates)
{
    m_buy_updates = updates;
}

int item_info::sold() const
{
    return m_sold;
}

void item_info::add_sold(int sold)
{
    m_sold += sold;
}

void item_info::set_sold(int sold)
{
    m_sold = sold;
}

int item_info::sell_max() const
{
    return m_sell_max;
}

void item_info::set_sell_max(int max)
{
    m_sell_max = max;
}

int item_info::sell_min() const
{
    return m_sell_min;
}

void item_info::set_sell_min(int min)
{
    m_sell_min = min;
}

int item_info::sell_npc() const
{
    return m_sell_npc;
}

void item_info::set_sell_npc(int price)
{
    m_sell_npc = price;
}

void item_info::update_sell_price(int price)
{
    if (price < m_sell_min || m_sell_min == 0)
    {
        m_sell_min = price;
    }

    if (price > m_sell_max || m_sell_max == 0)
    {
        m_sell_max = price;
    }
}

int item_info::total_bought() const
{
    return m_total_bought;
}

void item_info::add_total_bought(int bought)
{
    m_total_bought += bought;
}

void item_info::set_total_bought(int bought)
{
    m_total_bought = bought;
}

int item_info::average_buy_price() const
{
    return (m_bought != 0 ? (m_total_bought / m_bought) : 0);
}

int item_info::total_sold() const
{
    return m_total_sold;
}

void item_info::add_total_sold(int sold)
{
    m_total_sold += sold;
}

void item_info::set_total_sold(int sold)
{
    m_total_sold = sold;
}

int item_info::sell_updates() const
{
    return m_sell_updates;
}

void item_info::inc_sell_updates()
{
    ++m_sell_updates;
}

void item_info::set_sell_updates(int updates)
{
    m_sell_updates = updates;
}

int item_info::average_sell_price() const
{
    return (m_sold != 0 ? (m_total_sold / m_sold) : 0);
}

int item_info::total_profit() const
{
    return (m_total_sold - m_total_bought);
}

int item_info::average_profit_per_unit() const
{
    return (this->average_sell_price() - this->average_buy_price());
}

item_info::profit item_info::flag() const
{
    return m_flag;
}

void item_info::set_flag(profit flag)
{
    if (flag < do_not_worth)
    {
        m_flag = do_not_worth;
    }
    else if (flag > excelent)
    {
        m_flag = excelent;
    }
    else
    {
        m_flag = flag;
    }
}

QDateTime item_info::last_bought_time() const
{
    return m_last_bought_time;
}

void item_info::set_last_bought_time(const QDateTime& time)
{
    m_last_bought_time = time;
}

int item_info::last_bought_amount() const
{
    return m_last_bougth_amount;
}

void item_info::set_last_bought_amount(int amount)
{
    m_last_bougth_amount = amount;
}

int item_info::last_bought_price() const
{
    return m_last_bought_price;
}

void item_info::set_last_bought_price(int price)
{
    m_last_bought_price = price;
}

QDateTime item_info::last_sold_time() const
{
    return m_last_sold_time;
}

void item_info::set_last_sold_time(const QDateTime& time)
{
    m_last_sold_time = time;
}

int item_info::last_sold_amount() const
{
    return m_last_sold_amount;
}

void item_info::set_last_sold_amount(int amount)
{
    m_last_sold_amount = amount;
}

int item_info::last_sold_price() const
{
    return m_last_sold_price;
}

void item_info::set_last_sold_price(int price)
{
    m_last_sold_price = price;
}

bool item_info::operator==(const item_info& r) const
{
    return (m_id == r.m_id || m_name == r.name());
}

void item_info::read_from_xml(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attr = xml.attributes();

    bool ok = false;
    m_id = attr.value("id").toLongLong(&ok);
    if (!ok) m_id = -1;

    m_flag = static_cast<profit>(attr.value("flag").toInt(&ok));
    if (!ok) m_flag = profit::medium;

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "name")
            {
                m_name = xml.readElementText();
                continue;
            }

            if (xml.name() == "buy")
            {
                attr = xml.attributes();
                m_buy_min      = attr.value("min").toInt();
                m_buy_max      = attr.value("max").toInt();
                m_bought       = attr.value("amount").toInt();
                m_total_bought = attr.value("total").toInt();
                m_buy_npc      = attr.value("npc").toInt();
                m_buy_updates  = attr.value("updates").toInt();
                continue;
            }

            if (xml.name() == "sell")
            {
                attr = xml.attributes();
                m_sell_min     = attr.value("min").toInt();
                m_sell_max     = attr.value("max").toInt();
                m_sold         = attr.value("amount").toInt();
                m_total_sold   = attr.value("total").toInt();
                m_sell_npc     = attr.value("npc").toInt();
                m_sell_updates = attr.value("updates").toInt();
                continue;
            }

            if (xml.name() == "last")
            {
                while (!xml.atEnd() && !xml.hasError())
                {
                    token = xml.readNext();

                    if (token == QXmlStreamReader::StartElement)
                    {
                        if (xml.name() == "buy")
                        {
                            attr = xml.attributes();
                            m_last_bougth_amount = attr.value("amount").toInt();
                            m_last_bought_price  = attr.value("price").toInt();
                            m_last_bought_time = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                            continue;
                        }

                        if (xml.name() == "sell")
                        {
                            attr = xml.attributes();
                            m_last_sold_amount = attr.value("amount").toInt();
                            m_last_sold_price  = attr.value("price").toInt();
                            m_last_sold_time = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                            continue;
                        }

                        continue;
                    }

                    if (token == QXmlStreamReader::EndElement)
                    {
                        if (xml.name() == "last")
                        {
                            break;
                        }
                    }
                }

                continue;
            }

            continue;
        }

        if (token == QXmlStreamReader::EndElement)
        {
            if (xml.name() == "item")
            {
                break;
            }
        }
    }
}

void item_info::append_to_xml(QXmlStreamWriter& xml)
{
    xml.writeStartElement("item");
    xml.writeAttribute("id"  , QString::number(m_id));
    xml.writeAttribute("flag", QString::number(m_flag));

    xml.writeTextElement("name", m_name);

    xml.writeEmptyElement("buy");
    xml.writeAttribute("amount" , QString::number(m_bought));
    xml.writeAttribute("min"    , QString::number(m_buy_min));
    xml.writeAttribute("max"    , QString::number(m_buy_max));
    xml.writeAttribute("total"  , QString::number(m_total_bought));
    xml.writeAttribute("npc"    , QString::number(m_buy_npc));
    xml.writeAttribute("updates", QString::number(m_buy_updates));

    xml.writeEmptyElement("sell");
    xml.writeAttribute("amount" , QString::number(m_sold));
    xml.writeAttribute("min"    , QString::number(m_sell_min));
    xml.writeAttribute("max"    , QString::number(m_sell_max));
    xml.writeAttribute("total"  , QString::number(m_total_sold));
    xml.writeAttribute("npc"    , QString::number(m_sell_npc));
    xml.writeAttribute("updates", QString::number(m_sell_updates));

    xml.writeStartElement("last");

    xml.writeStartElement("buy");
    xml.writeAttribute("amount", QString::number(m_last_bougth_amount));
    xml.writeAttribute("price" , QString::number(m_last_bought_price));
    xml.writeCharacters(m_last_bought_time.toString(Qt::ISODate));
    xml.writeEndElement();

    xml.writeStartElement("sell");
    xml.writeAttribute("amount", QString::number(m_last_sold_amount));
    xml.writeAttribute("price" , QString::number(m_last_sold_price));
    xml.writeCharacters(m_last_sold_time.toString(Qt::ISODate));
    xml.writeEndElement();

    xml.writeEndElement();

    xml.writeEndElement();
}
