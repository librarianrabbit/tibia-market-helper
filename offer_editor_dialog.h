#ifndef OFFER_EDITOR_DIALOG_H
#define OFFER_EDITOR_DIALOG_H

#include <QDialog>
#include "offer_info.h"

namespace Ui
{
    class offer_editor_dialog;
}

class item_info;
class market_info;

class offer_editor_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit offer_editor_dialog(market_info& market, offer_info::type target, QWidget* parent = 0);
    ~offer_editor_dialog();

    void from(const offer_info& o);
    offer_info get_offer() const;

    offer_info::type get_type() const;
    void set_type(offer_info::type target);

    bool fee_enabled() const;
    void set_fee_enabled(bool enabled = true);

public slots:
    void set_balance(int gp);

    void update_items();

    void total_price_changed(int value);
    void price_piece_changed(int value);
    void amount_changed(int value);

    bool update_price();
    void update_fee();

    bool item_new();
    bool item_edit();
    bool item_del();

    void clear_item_alert();    

protected slots:
    void done(int r) override;

protected:
    Ui::offer_editor_dialog* ui;
    market_info& m_market;
    bool m_fee_enabled = true;
};

#endif // OFFER_EDITOR_DIALOG_H
