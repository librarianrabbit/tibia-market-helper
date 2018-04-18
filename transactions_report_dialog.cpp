#include "transactions_report_dialog.h"
#include "ui_transactions_report_dialog.h"
#include "market_info.h"
#include "transaction_report_model.h"
#include "item_profit_delegate.h"
#include "item_editor_dialog.h"
#include <QMessageBox>

transactions_report_dialog::transactions_report_dialog(market_info& market, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::transactions_report_dialog),
    m_market(market)
{
    ui->setupUi(this);

    this->connect(ui->tbReport, &QTableView::doubleClicked, this, &transactions_report_dialog::item_edit);

    ui->tbReport->setModel(m_transaction_model = new transaction_report_model(this));

    ui->tbReport->resizeColumnsToContents();
    ui->tbReport->resizeRowsToContents();
    ui->tbReport->horizontalHeader()->adjustSize();
    this->adjustSize();

    ui->tbReport->setItemDelegateForColumn(transaction_report_model::profit_column, new item_profit_delegate(ui->tbReport));

    ui->tbReport->setFocus();

    this->load_market_info();

    ui->tbReport->resizeColumnsToContents();

    ui->tbReport->sortByColumn(transaction_report_model::name_column);
}

transactions_report_dialog::~transactions_report_dialog()
{
    delete ui;
}

void transactions_report_dialog::load_market_info()
{
    ui->dtLastBought->setDateTime(m_market.last_bought());
    ui->dtLastSold->setDateTime(m_market.last_sold());

    ui->sbUnitsBought->setValue(m_market.total_items_bought());
    ui->sbTotalBought->setValue(m_market.total_bought());

    ui->sbUnitsSold->setValue(m_market.total_items_sold());
    ui->sbTotalSold->setValue(m_market.total_sold());

    ui->sbDiff->setValue(m_market.total_sold() - m_market.total_bought());

    m_transaction_model->set_items(m_market.items());
}

bool transactions_report_dialog::item_edit(const QModelIndex& index)
{
    bool ok = false;
    qlonglong id = index.data(Qt::UserRole).toLongLong(&ok);

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
    }

    return true;
}
