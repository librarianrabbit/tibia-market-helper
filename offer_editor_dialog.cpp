#include "offer_editor_dialog.h"
#include "ui_offer_editor_dialog.h"
#include "item_info.h"
#include "item_editor_dialog.h"
#include "market_info.h"
#include <QMessageBox>
#include <QTimer>

offer_editor_dialog::offer_editor_dialog(market_info& market, offer_info::type target, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::offer_editor_dialog),
    m_market(market)
{
    ui->setupUi(this);

    this->connect(ui->sbTotalPrice, SIGNAL(valueChanged(int)), this, SLOT(total_price_changed(int)));
    this->connect(ui->sbPricePiece, SIGNAL(valueChanged(int)), this, SLOT(price_piece_changed(int)));
    this->connect(ui->sbAmount, SIGNAL(valueChanged(int)), this, SLOT(amount_changed(int)));

    this->connect(ui->cbItem, SIGNAL(currentIndexChanged(int)), this, SLOT(update_price()));

    this->connect(ui->cbType, SIGNAL(currentIndexChanged(int)), this, SLOT(update_fee()));
    this->connect(ui->cbType, SIGNAL(currentIndexChanged(int)), this, SLOT(update_price()));

    this->connect(ui->btItemNew, &QAbstractButton::clicked, this, &offer_editor_dialog::item_new);
    this->connect(ui->btItemEdit, &QAbstractButton::clicked, this, &offer_editor_dialog::item_edit);
    this->connect(ui->btItemDel, &QAbstractButton::clicked, this, &offer_editor_dialog::item_del);

    ui->dtExpires->setDateTime(QDateTime::currentDateTime().addDays(30));

    this->set_balance(market.balance());
    this->set_type(target);
    this->update_items();
}

offer_editor_dialog::~offer_editor_dialog()
{
    delete ui;
}

void offer_editor_dialog::from(const offer_info& o)
{
    item_info* item = m_market.item_by_id(o.item_id());

    if (item != nullptr)
    {
        for (int i = 0; i < ui->cbItem->count(); ++i)
        {
            if (ui->cbItem->itemData(i) == item->id())
            {
                ui->cbItem->setCurrentIndex(i);
                break;
            }
        }
    }

    int total_price = o.total_price();
    int fee = market_info::calculate_fee(total_price);

    ui->cbType->setCurrentIndex(o.target() == offer_info::type::buy ? 0 : 1);
    ui->sbTotalPrice->setValue(total_price);
    ui->sbPricePiece->setValue(o.price());
    ui->sbAmount->setValue(o.amount());
    ui->sbFee->setValue(fee);
    ui->sbTotal->setValue(o.target() == offer_info::type::buy ? (total_price + fee) : fee);

    ui->sbPricePiece->setFocus();
}

offer_info offer_editor_dialog::get_offer() const
{
    offer_info o;

    o.set_target(ui->cbType->currentIndex() ? offer_info::type::buy : offer_info::type::sell);
    o.set_staus(offer_info::state::open);
    o.set_item_id(ui->cbItem->currentData().toInt());
    o.set_target(this->get_type());
    o.set_price(ui->sbPricePiece->value());
    o.set_amount(ui->sbAmount->value());
    o.set_fee(m_fee_enabled ? market_info::calculate_fee(o.total_price()) : 0);
    o.set_total_fee(o.fee());
    o.refresh();

    return o;
}

offer_info::type offer_editor_dialog::get_type() const
{
    return (ui->cbType->currentIndex() == 0 ? offer_info::type::buy : offer_info::type::sell);
}

void offer_editor_dialog::set_type(offer_info::type target)
{
    ui->cbType->setCurrentIndex(target == offer_info::type::buy ? 0 : 1);
    this->update_fee();
}

bool offer_editor_dialog::fee_enabled() const
{
    return m_fee_enabled;
}

void offer_editor_dialog::set_fee_enabled(bool enabled)
{
    m_fee_enabled = enabled;
    this->update_fee();
}

void offer_editor_dialog::set_balance(int gp)
{
    ui->sbBalance->setValue(gp);
    this->update_fee();
}

void offer_editor_dialog::update_items()
{
    ui->cbItem->clear();

    QList<item_info*> items = m_market.items();
    qSort(items.begin(), items.end(), [](item_info* l, item_info* r) { return (l->name() < r->name()); });

    for (item_info* i : items)
    {
        ui->cbItem->addItem(i->name(), i->id());
    }

    ui->cbItem->setCurrentIndex(-1);

    this->update_price();
}

void offer_editor_dialog::total_price_changed(int value)
{
    int amount = ui->sbAmount->value();

    if (amount > 0)
    {
        bool block = ui->sbPricePiece->blockSignals(true);
        ui->sbPricePiece->setValue(value / amount);
        ui->sbPricePiece->blockSignals(block);
    }

    this->update_fee();
}

void offer_editor_dialog::price_piece_changed(int value)
{
    bool block = ui->sbTotalPrice->blockSignals(true);
    ui->sbTotalPrice->setValue(value * ui->sbAmount->value());
    ui->sbTotalPrice->blockSignals(block);

    this->update_fee();
}

void offer_editor_dialog::amount_changed(int value)
{
    bool block = ui->sbTotalPrice->blockSignals(true);
    ui->sbTotalPrice->setValue(ui->sbPricePiece->value() * value);
    ui->sbTotalPrice->blockSignals(block);

    this->update_fee();
}

bool offer_editor_dialog::update_price()
{
    bool ok = false;
    qlonglong id = ui->cbItem->currentData().toInt(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    item_info* item = m_market.item_by_id(id);

    if (item == nullptr)
    {
        return false;
    }

    if (this->get_type() == offer_info::type::buy)
    {
        ui->sbPricePiece->setValue(item->last_bought_price());
        ui->sbAmount->setValue(item->last_bought_amount());
    }
    else
    {
        ui->sbPricePiece->setValue(item->last_sold_price());
        ui->sbAmount->setValue(item->last_sold_amount());
    }

    return true;
}

void offer_editor_dialog::update_fee()
{
    int total = ui->sbTotalPrice->value();
    int fee = (m_fee_enabled ? market_info::calculate_fee(total) : 0);

    ui->sbFee->setValue(fee);
    ui->sbTotal->setValue((this->get_type() == offer_info::type::buy ? total : 0) + fee);

    if (ui->sbBalance->value() < ui->sbTotal->value())
    {
        ui->sbTotal->setStyleSheet("color: red");
    }
    else
    {
        ui->sbTotal->setStyleSheet(QString());
    }
}

bool offer_editor_dialog::item_new()
{
    item_editor_dialog editor(this);

    if (ui->cbItem->currentText() != ui->cbItem->itemText(ui->cbItem->currentIndex()))
    {
        editor.set_name(ui->cbItem->currentText());
    }

    if (editor.exec() == item_editor_dialog::Accepted)
    {
        item_info* item = m_market.item_import(editor.get_item());

        if (item != nullptr)
        {
            ui->cbItem->addItem(item->name(), item->id());
            ui->cbItem->setCurrentIndex(ui->cbItem->count() - 1);
            return true;
        }
        else
        {
            QMessageBox::warning(this, "Invalid Item", "Cannot add this item. Please check if it does not exists already.");
            return false;
        }
    }

    return false;
}

bool offer_editor_dialog::item_edit()
{
    bool ok = false;
    qlonglong id = ui->cbItem->currentData().toInt(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    item_info* item = m_market.item_by_id(id);

    if (item == nullptr)
    {
        QMessageBox::warning(this, "Invalid Item", "Cannot edit this item.");
        return false;
    }

    item_editor_dialog editor(this);

    editor.from(*item);

    if (editor.exec() == item_editor_dialog::Accepted)
    {
        item->from(editor.get_item());
        ui->cbItem->setItemText(ui->cbItem->currentIndex(), item->name());
    }

    return true;
}

bool offer_editor_dialog::item_del()
{
    if (QMessageBox::question(this, "Item Removal", "Do you really want to remove this item?") != QMessageBox::Yes)
    {
        return false;
    }

    bool ok = false;
    qlonglong id = ui->cbItem->currentData().toInt(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    if (!m_market.item_remove(id))
    {
        QMessageBox::warning(this, "Invalid Item", "Cannot remove this item.");
        return false;
    }

    ui->cbItem->removeItem(ui->cbItem->currentIndex());

    return true;
}

void offer_editor_dialog::clear_item_alert()
{
    ui->cbItem->setStyleSheet(QString());
}

void offer_editor_dialog::done(int r)
{
    if (r == offer_editor_dialog::Accepted)
    {
        if (ui->cbItem->currentIndex() == -1 || (ui->cbItem->currentText() != ui->cbItem->itemText(ui->cbItem->currentIndex())))
        {
            ui->cbItem->setStyleSheet("background-color: red");
            QTimer::singleShot(500, this, SLOT(clear_item_alert()));
            return;
        }

        if (ui->sbTotal->value() > ui->sbBalance->value())
        {
            return;
        }
    }

    QDialog::done(r);
}
