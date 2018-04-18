#ifndef OFFER_HISTORY_DIALOG_H
#define OFFER_HISTORY_DIALOG_H

#include <QDialog>

namespace Ui
{
    class offer_history_dialog;
}

class market_info;

class offer_history_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit offer_history_dialog(market_info& market, QWidget* parent = 0);
    ~offer_history_dialog();

protected:
    void load_market_info();

private:
    Ui::offer_history_dialog* ui;
    market_info& m_market;
};

#endif // OFFER_HISTORY_DIALOG_H
