#include "offer_history_dialog.h"
#include "ui_offer_history_dialog.h"

offer_history_dialog::offer_history_dialog(market_info& market, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::offer_history_dialog),
    m_market(market)
{
    ui->setupUi(this);
}

offer_history_dialog::~offer_history_dialog()
{
    delete ui;
}

void offer_history_dialog::load_market_info()
{
}
