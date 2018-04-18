#ifndef TRANSACTIONS_REPORT_DIALOG_H
#define TRANSACTIONS_REPORT_DIALOG_H

#include <QDialog>

namespace Ui
{
    class transactions_report_dialog;
}

class market_info;
class transaction_report_model;

class transactions_report_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit transactions_report_dialog(market_info& market, QWidget* parent = 0);
    ~transactions_report_dialog();

protected:
    void load_market_info();

protected slots:
    bool item_edit(const QModelIndex& index);

protected:
    Ui::transactions_report_dialog* ui;
    transaction_report_model* m_transaction_model = nullptr;
    market_info& m_market;
};

#endif // TRANSACTIONS_REPORT_DIALOG_H
