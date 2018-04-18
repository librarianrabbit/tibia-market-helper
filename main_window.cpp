#include "main_window.h"
#include "ui_main_window.h"
#include "market_model.h"
#include <QInputDialog>
#include "offer_editor_dialog.h"
#include <QMenu>
#include <QMessageBox>
#include "transactions_report_dialog.h"
#include <QFile>
#include <QFileDialog>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCloseEvent>

main_window::main_window(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::main_window)
{
    ui->setupUi(this);

    this->connect(ui->actionNew, &QAction::triggered, this, &main_window::clear);

    this->connect(ui->actionOpen, &QAction::triggered, this, &main_window::open);
    this->connect(ui->actionRevert, &QAction::triggered, this, &main_window::revert);
    this->connect(ui->actionSave, &QAction::triggered, this, &main_window::save);
    this->connect(ui->actionSaveAs, &QAction::triggered, this, &main_window::save_as);
    this->connect(ui->actionClose, &QAction::triggered, this, &main_window::close_file);

    this->connect(ui->btBuyNew, &QAbstractButton::clicked, this, &main_window::buy_new);
    this->connect(ui->btBuyEdit, &QAbstractButton::clicked, this, &main_window::buy_edit);
    this->connect(ui->btBuyDel, &QAbstractButton::clicked, this, &main_window::buy_del);
    this->connect(ui->btBuy, &QAbstractButton::clicked, this, &main_window::buy_offer);
    this->connect(ui->btCustomBuy, &QAbstractButton::clicked, this, &main_window::buy_custom);

    this->connect(ui->btSellNew, &QAbstractButton::clicked, this, &main_window::sell_new);
    this->connect(ui->btSellEdit, &QAbstractButton::clicked, this, &main_window::sell_edit);
    this->connect(ui->btSellDel, &QAbstractButton::clicked, this, &main_window::sell_del);
    this->connect(ui->btSell, &QAbstractButton::clicked, this, &main_window::sell_offer);
    this->connect(ui->btCustomSell, &QAbstractButton::clicked, this, &main_window::sell_custom);

    this->connect(ui->btBalance, &QAbstractButton::clicked, this, &main_window::update_balance);

    this->connect(ui->tbBuy, &QTableView::doubleClicked, this, &main_window::open_buy_editor);
    this->connect(ui->tbSell, &QTableView::doubleClicked, this, &main_window::open_sell_editor);

    this->connect(ui->tbBuy, &market_view::current_index_changed, this, &main_window::buy_index_changed);
    this->connect(ui->tbSell, &market_view::current_index_changed, this, &main_window::sell_index_changed);

    this->connect(ui->actionReport, &QAction::triggered, this, &main_window::view_report);

    m_buy_model = new market_model(m_market, offer_info::type::buy, ui->tbBuy);
    m_sell_model = new market_model(m_market, offer_info::type::sell, ui->tbSell);

    ui->tbBuy->setModel(m_buy_model);
    ui->tbSell->setModel(m_sell_model);

    ui->tbBuy->sortByColumn(market_model::name_column);
    ui->tbSell->sortByColumn(market_model::name_column);

    ui->tbBuy->installEventFilter(this);
    ui->tbSell->installEventFilter(this);

    this->market_updated();

    m_has_changes = false;
}

main_window::~main_window()
{
    delete ui;
}

void main_window::clear()
{
    m_buy_model->clear();
    m_sell_model->clear();

    m_market.reset();

    this->market_updated();
}

bool main_window::open()
{
    QString file = QFileDialog::getOpenFileName(this, "Open Tibia Market File", QString(), "Tibia Market File (*.xml);;All files (*.*)");

    if (file.isEmpty())
    {
        return false;
    }

    return this->load_xml(file);
}

bool main_window::load_xml(const QString& file)
{
    QFile f(file);

    if (!f.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, "Open File Error", "Failed to open file for reading.");
        return false;
    }

    QXmlStreamReader xml(&f);

    while (!xml.atEnd() && !xml.hasError())
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument)
        {
            continue;
        }

        if (token == QXmlStreamReader::StartElement)
        {
            if (xml.name() == "market")
            {
                this->clear();

                m_market.read_from_xml(xml);

                m_market.apply_item_cache();

                m_market.apply_buy_cache();
                m_market.apply_sell_cache();

                m_buy_model->load_offers();
                m_sell_model->load_offers();

                market_info::details d = m_market.get_details();
                m_market.set_initial_capital(d.buy_total);

                continue;
            }

            continue;
        }

        if (token == QXmlStreamReader::EndDocument)
        {
            break;
        }
    }

    f.close();
    m_file = file;

    ui->tbBuy->sortByColumn(0);

    this->market_updated();

    m_has_changes = false;

    return true;
}

bool main_window::revert()
{
    if (m_file.isEmpty())
    {
        return false;
    }

    bool ret = (!m_has_changes);

    if (m_has_changes)
    {
        int r = QMessageBox::question(this,
                                      "Data Changed",
                                      "The market data has been changed. Do you really want to revert it now?",
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (r == QMessageBox::Yes)
        {
            ret = true;
        }
        else if (r == QMessageBox::Cancel)
        {
            return false;
        }
    }

    if (ret)
    {
        ret = this->load_xml(m_file);
    }

    return ret;
}

bool main_window::save()
{
    if (m_file.isEmpty())
    {
        return this->save_as();
    }

    QFile f(m_file);

    if (!f.open(QFile::WriteOnly))
    {
        QMessageBox::warning(this, "Save File Error", "Failed to open file for writing.");
        return false;
    }

    QXmlStreamWriter xml(&f);
    xml.setAutoFormatting(true);
    xml.writeStartDocument("1.0");

    m_market.append_to_xml(xml);

    xml.writeEndDocument();

    f.close();

    m_has_changes = false;

    return true;
}

bool main_window::save_as()
{
    QString file = QFileDialog::getSaveFileName(this, "Save Tibia Market File", QString(), "Tibia Market File (*.xml);;All files (*.*)");

    if (file.isEmpty())
    {
        return false;
    }

    QFile f(file);

    if (!f.open(QFile::WriteOnly))
    {
        QMessageBox::warning(this, "Save File Error", "Failed to open file for writing.");
        return false;
    }

    f.close();
    m_file = file;

    return this->save();
}

bool main_window::close_file()
{
    bool ret = true;

    if (m_has_changes)
    {
        int r = QMessageBox::question(this,
                                      "Data Changed",
                                      "The market data has been changed. Do you want to save it now?",
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (r == QMessageBox::Yes)
        {
            ret = this->save();
        }
        else if (r == QMessageBox::Cancel)
        {
            return false;
        }
    }

    if (ret)
    {
        m_file.clear();
        this->reset();
    }

    return ret;
}

bool main_window::buy_new()
{
    if (m_market.count_offers() >= 100)
    {
        QMessageBox::information(this, "100 Offers Limit Reached", "You can't have more than 100 open offers at the same time.");
        return false;
    }

    offer_editor_dialog editor(m_market, offer_info::type::buy, this);

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info* offer = this->new_offer(editor.get_offer());

        if (offer != nullptr)
        {
            this->select_buy_offer_by_id(offer->id());
            this->market_updated();
            return true;
        }
        else
        {
            QMessageBox::warning(this, "Invalid Offer", "Cannot add this offer.");
            return false;
        }
    }

    return false;
}

bool main_window::buy_edit()
{
    bool ok = false;
    qlonglong id = ui->tbBuy->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->edit_buy_offer_by_id(id);
}

bool main_window::buy_del()
{
    bool ok = false;
    qlonglong id = ui->tbBuy->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->cancel_buy_offer_by_id(id);
}

bool main_window::buy_offer()
{
    bool ok = false;
    qlonglong id = ui->tbBuy->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->accept_buy_offer_by_id(id);
}

bool main_window::buy_custom()
{
    offer_editor_dialog editor(m_market, offer_info::type::buy, this);
    editor.set_fee_enabled(false);

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info* offer = m_market.offer_import(editor.get_offer());

        if (offer != nullptr)
        {
            if (m_market.offer_accept(*offer, offer->amount()))
            {
                this->market_updated();
                return true;
            }
            else
            {
                QMessageBox::warning(this, "Invalid Offer", "Cannot accept this offer.");
                return false;
            }
        }
        else
        {
            QMessageBox::warning(this, "Invalid Offer", "Cannot add this offer.");
            return false;
        }
    }

    return false;
}

offer_info* main_window::buy_flag()
{
    bool ok = false;
    qlonglong id = ui->tbBuy->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return nullptr;
    }

    return this->flag_buy_offer_by_id(id);
}

int main_window::clear_buy_flags()
{
    return m_buy_model->clear_offer_flags();
}

bool main_window::open_buy_editor(const QModelIndex& index)
{
    bool ok = false;
    qlonglong id = index.data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->edit_buy_offer_by_id(id);
}

bool main_window::sell_new()
{
    if (m_market.count_offers() >= 100)
    {
        QMessageBox::information(this, "100 Offers Limit Reached", "You can't have more than 100 open offers at the same time.");
        return false;
    }

    offer_editor_dialog editor(m_market, offer_info::type::sell, this);

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info* offer = this->new_offer(editor.get_offer());

        if (offer != nullptr)
        {
            this->select_sell_offer_by_id(offer->id());
            this->market_updated();
            return true;
        }
        else
        {
            QMessageBox::warning(this, "Invalid Offer", "Cannot add this offer.");
            return false;
        }
    }

    return false;
}

bool main_window::sell_edit()
{
    bool ok = false;
    qlonglong id = ui->tbSell->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->edit_sell_offer_by_id(id);
}

bool main_window::sell_del()
{
    bool ok = false;
    qlonglong id = ui->tbSell->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->cancel_sell_offer_by_id(id);
}

bool main_window::sell_offer()
{
    bool ok = false;
    qlonglong id = ui->tbSell->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->accept_sell_offer_by_id(id);
}

bool main_window::sell_custom()
{
    offer_editor_dialog editor(m_market, offer_info::type::sell, this);
    editor.set_fee_enabled(false);

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info* offer = m_market.offer_import(editor.get_offer());

        if (offer != nullptr)
        {
            if (m_market.offer_accept(*offer, offer->amount()))
            {
                this->market_updated();
                return true;
            }
            else
            {
                QMessageBox::warning(this, "Invalid Offer", "Cannot accept this offer.");
                return false;
            }
        }
        else
        {
            QMessageBox::warning(this, "Invalid Offer", "Cannot add this offer.");
            return false;
        }
    }

    return false;
}

offer_info* main_window::sell_flag()
{
    bool ok = false;
    qlonglong id = ui->tbSell->currentIndex().data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return nullptr;
    }

    return this->flag_sell_offer_by_id(id);
}

int main_window::clear_sell_flags()
{
    return m_sell_model->clear_offer_flags();
}

bool main_window::open_sell_editor(const QModelIndex& index)
{
    bool ok = false;
    qlonglong id = index.data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    return this->edit_sell_offer_by_id(id);
}

bool main_window::buy_index_changed(const QModelIndex& index, const QModelIndex&)
{
    if (!ui->cbSync->isChecked())
    {
        return false;
    }

    bool ok = false;
    qlonglong id = index.data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    QModelIndex sell = m_sell_model->find_offer_by_item_id(o->item_id());

    bool block = ui->tbSell->blockSignals(true);
    ui->tbSell->setCurrentIndex(sell);
    ui->tbSell->blockSignals(block);

    return true;
}

bool main_window::sell_index_changed(const QModelIndex& index, const QModelIndex&)
{
    if (!ui->cbSync->isChecked())
    {
        return false;
    }

    bool ok = false;
    qlonglong id = index.data(Qt::UserRole).toLongLong(&ok);

    if (!ok || id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    QModelIndex buy = m_buy_model->find_offer_by_item_id(o->item_id());

    bool block = ui->tbBuy->blockSignals(true);
    ui->tbBuy->setCurrentIndex(buy);
    ui->tbBuy->blockSignals(block);

    return true;
}

offer_info* main_window::new_offer(const offer_info& o)
{
    if (o.target() == offer_info::type::buy)
    {
        return m_buy_model->import_offer(o);
    }
    else
    {
        return m_sell_model->import_offer(o);
    }

    return nullptr;
}

bool main_window::accept_offer(offer_info& o, int amount)
{
    if (o.target() == offer_info::type::buy)
    {
        return m_buy_model->accept_offer(o, amount);
    }
    else
    {
        return m_sell_model->accept_offer(o, amount);
    }

    return nullptr;
}

bool main_window::accept_buy_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    bool ok = false;
    int amount = QInputDialog::getInt(this, "Amount", "Amount:", 1, 1, o->amount(), 1, &ok);

    if (!ok)
    {
        return false;
    }

    bool ret = m_buy_model->accept_offer(*o, amount);

    this->market_updated();

    return ret;
}

bool main_window::edit_buy_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    offer_editor_dialog editor(m_market, offer_info::type::buy, this);
    editor.from(*o);
    editor.set_balance(m_market.balance() + o->total_price());

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info n = editor.get_offer();

        if (o->target() != n.target())
        {
            m_buy_model->cancel_offer(o->id());
            m_sell_model->import_offer(n);
        }
        else
        {
            m_buy_model->update_offer(*o, n);
        }

        this->market_updated();

        return true;
    }

    return false;
}

bool main_window::cancel_buy_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    if (QMessageBox::question(this, "Offer Removal", "Are you sure you want to cancel this offer?") != QMessageBox::Yes)
    {
        return false;
    }

    m_buy_model->cancel_offer(id);
    this->market_updated();

    return true;
}

offer_info* main_window::flag_buy_offer_by_id(qlonglong id, int update)
{
    if (id == -1)
    {
        return nullptr;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return nullptr;
    }

    m_buy_model->flag_offer(*o, update);

    return o;
}

bool main_window::select_buy_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    QModelIndex buy = m_buy_model->find_offer_by_id(id);
    ui->tbBuy->setCurrentIndex(buy);

    return buy.isValid();
}

bool main_window::accept_sell_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    bool ok = false;
    int amount = QInputDialog::getInt(this, "Amount", "Amount:", 1, 1, o->amount(), 1, &ok);

    if (!ok)
    {
        return false;
    }

    bool ret = m_sell_model->accept_offer(*o, amount);

    this->market_updated();

    return ret;
}

bool main_window::edit_sell_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return false;
    }

    offer_editor_dialog editor(m_market, offer_info::type::sell, this);
    editor.from(*o);

    if (editor.exec() == QDialog::Accepted)
    {
        offer_info n = editor.get_offer();

        if (o->target() != n.target())
        {
            m_sell_model->cancel_offer(o->id());
            m_buy_model->import_offer(n);
        }
        else
        {
            m_sell_model->update_offer(*o, n);
        }

        this->market_updated();

        return true;
    }

    return false;
}

bool main_window::cancel_sell_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    if (QMessageBox::question(this, "Offer Removal", "Are you sure you want to cancel this offer?") != QMessageBox::Yes)
    {
        return false;
    }

    m_sell_model->cancel_offer(id);
    this->market_updated();

    return true;
}

offer_info* main_window::flag_sell_offer_by_id(qlonglong id, int update)
{
    if (id == -1)
    {
        return nullptr;
    }

    offer_info* o = m_market.offer_by_id(id);

    if (o == nullptr)
    {
        return nullptr;
    }

    m_sell_model->flag_offer(*o, update);

    return o;
}

bool main_window::select_sell_offer_by_id(qlonglong id)
{
    if (id == -1)
    {
        return false;
    }

    QModelIndex sell = m_sell_model->find_offer_by_id(id);
    ui->tbSell->setCurrentIndex(sell);

    return sell.isValid();
}

void main_window::update_balance()
{
    bool ok = false;
    int balance = QInputDialog::getInt(this, "Balance", "Balance:", m_market.balance(), 0, INT_MAX, 1, &ok);

    if (ok)
    {
        m_market.set_balance(balance);
        this->market_updated();
    }
}

void main_window::market_updated()
{
    market_info::details d = m_market.get_details();

    int capital = m_market.balance() + d.buy_total;

    ui->sbBuyOffers->setValue(m_market.buy_offers().size());
    ui->sbBuyItems->setValue(d.buy_items);
    ui->sbBuyTotal->setValue(d.buy_total);

    ui->sbSellOffers->setValue(m_market.sell_offers().size());
    ui->sbSellItems->setValue(d.sell_items);
    ui->sbSellTotal->setValue(d.sell_total);

    ui->sbBalance->setValue(m_market.balance());
    ui->sbBalanceItems->setValue(capital);

    ui->dtLastTransaction->setDateTime(m_market.last_transaction());

    QString msg = QString("Current profit: %1 gp (%2 gp).");
    msg = msg.arg(this->locale().toString(m_market.balance() - m_market.initial_balance()));
    msg = msg.arg(capital - (m_market.initial_balance() + m_market.initial_capital()));
    ui->statusBar->showMessage(msg);

    m_has_changes = true;

    ui->tbBuy->resizeColumnsToContents();
    ui->tbSell->resizeColumnsToContents();
}

void main_window::view_report()
{
    transactions_report_dialog report(m_market, this);;
    report.exec();
}

void main_window::reset()
{
    m_buy_model->clear();
    m_sell_model->clear();

    m_market.reset();

    this->market_updated();

    m_has_changes = false;
}

bool main_window::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::KeyPress)
    {
        QKeyEvent* k = static_cast<QKeyEvent*>(e);

        if (k != nullptr)
        {
            QTableView* tbl = qobject_cast<QTableView*>(o);

            if (k->key() == Qt::Key_Space)
            {
                if (tbl == ui->tbBuy)
                {
                    this->buy_offer();
                }
                else if (tbl == ui->tbSell)
                {
                    this->sell_offer();
                }
            }
            else if (k->key() == Qt::Key_E && (k->modifiers() & Qt::SHIFT))
            {
                if (tbl == ui->tbBuy)
                {
                    this->buy_edit();

                    if (ui->cbSync->isChecked())
                    {
                        this->sell_edit();
                    }
                }
                else if (tbl == ui->tbSell)
                {
                    this->sell_edit();

                    if (ui->cbSync->isChecked())
                    {
                        this->buy_edit();
                    }
                }
            }
            else if (k->key() == Qt::Key_S && (k->modifiers() & Qt::SHIFT))
            {
                if (tbl == ui->tbBuy || tbl == ui->tbSell)
                {
                    this->sell_edit();
                }
            }
            else if (k->key() == Qt::Key_B && (k->modifiers() & Qt::SHIFT))
            {
                if (tbl == ui->tbBuy || tbl == ui->tbSell)
                {
                    this->buy_edit();
                }
            }
            else if (k->key() == Qt::Key_Delete)
            {
                if (tbl == ui->tbBuy)
                {
                    this->buy_del();
                }
                else if (tbl == ui->tbSell)
                {
                    this->sell_del();
                }
            }
            else if (k->key() == Qt::Key_Asterisk)
            {
                if (tbl == ui->tbBuy)
                {
                    offer_info* o = this->buy_flag();

                    if (o != nullptr && ui->cbSync->isChecked())
                    {
                        QModelIndex sell = m_sell_model->find_offer_by_item_id(o->item_id());

                        if (sell.isValid())
                        {
                            bool ok = false;
                            qlonglong id = sell.data(Qt::UserRole).toLongLong(&ok);

                            if (ok && id != -1)
                            {
                                this->flag_sell_offer_by_id(id, (o->flag() == true ? market_model::set_flag : market_model::clear_flag));
                            }
                        }
                    }
                }
                else if (tbl == ui->tbSell)
                {
                    offer_info* o = this->sell_flag();

                    if (o != nullptr && ui->cbSync->isChecked())
                    {
                        QModelIndex buy = m_buy_model->find_offer_by_item_id(o->item_id());

                        if (buy.isValid())
                        {
                            bool ok = false;
                            qlonglong id = buy.data(Qt::UserRole).toLongLong(&ok);

                            if (ok && id != -1)
                            {
                                this->flag_buy_offer_by_id(id, (o->flag() == true ? market_model::set_flag : market_model::clear_flag));
                            }
                        }
                    }
                }
            }
            else if (k->key() == Qt::Key_Slash)
            {
                this->clear_buy_flags();
                this->clear_sell_flags();
            }
        }
    }

    return QMainWindow::eventFilter(o, e);
}

void main_window::closeEvent(QCloseEvent* e)
{
    if (!this->close_file())
    {
        e->ignore();
        return;
    }

    QMainWindow::closeEvent(e);
}
