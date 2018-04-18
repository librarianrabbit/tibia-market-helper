#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include "market_info.h"
#include "market_model.h"

namespace Ui
{
    class main_window;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_window(QWidget* parent = 0);
    ~main_window();

public slots:
    void clear();

    bool open();
    bool revert();
    bool save();
    bool save_as();
    bool close_file();

    bool buy_new();
    bool buy_edit();
    bool buy_del();
    bool buy_offer();
    bool buy_custom();
    offer_info* buy_flag();
    int clear_buy_flags();

    bool sell_new();
    bool sell_edit();
    bool sell_del();
    bool sell_offer();
    bool sell_custom();
    offer_info* sell_flag();
    int clear_sell_flags();

    void update_balance();

    void market_updated();

    void view_report();

    void reset();

protected:
    bool load_xml(const QString& file);

    offer_info* new_offer(const offer_info& o);
    bool accept_offer(offer_info& o, int amount);

    bool accept_buy_offer_by_id(qlonglong id);
    bool edit_buy_offer_by_id(qlonglong id);
    bool cancel_buy_offer_by_id(qlonglong id);
    offer_info* flag_buy_offer_by_id(qlonglong id, int update = market_model::toggle_flag);
    bool select_buy_offer_by_id(qlonglong id);

    bool accept_sell_offer_by_id(qlonglong id);
    bool edit_sell_offer_by_id(qlonglong id);
    bool cancel_sell_offer_by_id(qlonglong id);
    offer_info* flag_sell_offer_by_id(qlonglong id, int update = market_model::toggle_flag);
    bool select_sell_offer_by_id(qlonglong id);

    bool eventFilter(QObject* o, QEvent* e);

    void closeEvent(QCloseEvent* e) override;

protected slots:
    bool open_buy_editor(const QModelIndex& index);
    bool open_sell_editor(const QModelIndex& index);

    bool buy_index_changed(const QModelIndex& index, const QModelIndex& previous);
    bool sell_index_changed(const QModelIndex& index, const QModelIndex& previous);

protected:
    Ui::main_window* ui;

    market_info m_market;

    market_model* m_buy_model = nullptr;
    market_model* m_sell_model = nullptr;

    QString m_file;
    bool m_has_changes = false;
};

#endif // MAIN_WINDOW_H
