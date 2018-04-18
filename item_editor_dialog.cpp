#include "item_editor_dialog.h"
#include "ui_item_editor_dialog.h"
#include "item_info.h"

item_editor_dialog::item_editor_dialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::item_editor_dialog)
{
    ui->setupUi(this);

    ui->edName->setFocus();
}

item_editor_dialog::~item_editor_dialog()
{
    delete ui;
}

void item_editor_dialog::from(const item_info& i)
{
    ui->edName->setText(i.name());

    ui->sbBought->setValue(i.bought());
    ui->sbBuyMax->setValue(i.buy_max());
    ui->sbBuyAvg->setValue(i.average_buy_price());
    ui->sbBuyMin->setValue(i.buy_min());
    ui->sbBuyNPC->setValue(i.buy_npc());
    ui->sbTotalBought->setValue(i.total_bought());

    ui->sbSold->setValue(i.sold());
    ui->sbSellMax->setValue(i.sell_max());
    ui->sbSellAvg->setValue(i.average_sell_price());
    ui->sbSellMin->setValue(i.sell_min());
    ui->sbSellNPC->setValue(i.sell_npc());
    ui->sbTotalSold->setValue(i.total_sold());

    ui->cbProfit->setCurrentIndex(i.flag());

    ui->sbLastBought->setValue(i.last_bought_amount());
    ui->sbLastTotalBought->setValue(i.last_bought_price() * i.last_bought_amount());
    ui->dtLastBought->setDateTime(i.last_bought_time());

    ui->sbLastSold->setValue(i.last_sold_amount());
    ui->sbLastTotalSold->setValue(i.last_sold_price() * i.last_sold_amount());
    ui->dtLastSold->setDateTime(i.last_sold_time());
}

item_info item_editor_dialog::get_item() const
{
    item_info i;

    i.set_name(ui->edName->text());
    i.set_flag(static_cast<item_info::profit>(ui->cbProfit->currentIndex()));
    i.set_buy_npc(ui->sbBuyNPC->value());
    i.set_sell_npc(ui->sbSellNPC->value());

    return i;
}

void item_editor_dialog::set_name(const QString& name)
{
    ui->edName->setText(name);
}

void item_editor_dialog::done(int r)
{
    if (r == item_editor_dialog::Accepted)
    {
        if (ui->edName->text().isEmpty())
        {
            ui->edName->setFocus();
            return;
        }
    }

    QDialog::done(r);
}
