#include "offer_info.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

offer_info::offer_info(qlonglong id) :
    m_id(id)
{
}

offer_info::offer_info(qlonglong id, const offer_info& offer) :
    m_id(id)
{
    this->from(offer);
}

offer_info::offer_info(QXmlStreamReader& xml)
{
    this->read_from_xml(xml);
}

void offer_info::from(const offer_info& offer)
{
    if (offer.m_target != type::sell)
    {
        m_target = type::buy;
    }
    else
    {
        m_target = offer.m_target;
    }

    m_item_id = offer.m_item_id;

    m_amount = offer.m_amount;
    m_price = offer.m_price;

    m_fee = offer.m_fee;

    m_expires = offer.m_expires;
}

qlonglong offer_info::id() const
{
    return m_id;
}

offer_info::type offer_info::target() const
{
    return m_target;
}

void offer_info::set_target(type target)
{
    if (target != type::sell)
    {
        m_target = type::buy;
    }
    else
    {
        m_target = target;
    }
}

offer_info::state offer_info::status() const
{
    return m_status;
}

void offer_info::set_staus(state status)
{
    m_status = status;
}

qlonglong offer_info::item_id() const
{
    return m_item_id;
}

void offer_info::set_item_id(qlonglong id)
{
    m_item_id = id;
}

int offer_info::amount() const
{
    return m_amount;
}

void offer_info::add_amount(int amount)
{
    m_amount += amount;
}

void offer_info::sub_amount(int amount)
{
    m_amount -= amount;
}

void offer_info::set_amount(int amount)
{
    m_amount = amount;
}

int offer_info::price() const
{
    return m_price;
}

void offer_info::set_price(int price)
{
    m_price = price;
}

int offer_info::total_price() const
{
    return (m_amount * m_price);
}

int offer_info::fee() const
{
    return m_fee;
}

void offer_info::set_fee(int fee)
{
    m_fee = fee;
}

int offer_info::total_fee() const
{
    return m_total_fee;
}

void offer_info::add_total_fee(int fee)
{
    m_total_fee += fee;
}

void offer_info::set_total_fee(int total)
{
    m_total_fee = total;
}

bool offer_info::flag() const
{
    return m_flag;
}

void offer_info::set_flag(bool flag)
{
    m_flag = flag;
}

void offer_info::toggle_flag()
{
    m_flag = !m_flag;
}

QDateTime offer_info::expires() const
{
    return m_expires;
}

void offer_info::refresh()
{
    m_expires = QDateTime::currentDateTime().addDays(30);
}

void offer_info::set_expires(const QDateTime& expires)
{
    m_expires = expires;
}

bool offer_info::operator==(const offer_info& r) const
{
    return (m_id == r.m_id);
}

void offer_info::read_from_xml(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attr = xml.attributes();

    bool ok = false;
    m_id = attr.value("id").toLongLong(&ok);
    if (!ok) m_id = -1;

    m_target = static_cast<type>(attr.value("target").toInt(&ok));
    if (!ok || (m_target != type::buy && m_target != type::sell)) m_target = type::buy;

    m_item_id = attr.value("item").toInt(&ok);
    if (!ok) m_item_id = -1;

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "info")
            {
                attr = xml.attributes();
                m_amount = attr.value("amount").toInt();
                m_price  = attr.value("price").toInt();
                continue;
            }

            if (xml.name() == "fee")
            {
                attr = xml.attributes();
                m_total_fee = attr.value("total").toInt();
                m_fee       = xml.readElementText().toInt();
                continue;
            }

            if (xml.name() == "expires")
            {
                m_expires = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                continue;
            }

            continue;
        }

        if (token == QXmlStreamReader::EndElement)
        {
            if (xml.name() == "offer")
            {
                break;
            }
        }
    }
}

void offer_info::append_to_xml(QXmlStreamWriter& xml)
{
    xml.writeStartElement("offer");
    xml.writeAttribute("id", QString::number(m_id));
    xml.writeAttribute("target", QString::number(static_cast<int>(m_target)));
    xml.writeAttribute("item", QString::number(m_item_id));

    xml.writeEmptyElement("info");
    xml.writeAttribute("amount", QString::number(m_amount));
    xml.writeAttribute("price", QString::number(m_price));

    xml.writeStartElement("fee");
    xml.writeAttribute("total", QString::number(m_total_fee));
    xml.writeCharacters(QString::number(m_fee));
    xml.writeEndElement();

    xml.writeTextElement("expires", m_expires.toString(Qt::ISODate));

    xml.writeEndElement();
}
