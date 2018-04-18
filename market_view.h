#ifndef MARKET_VIEW_H
#define MARKET_VIEW_H

#include <QTableView>

class market_view : public QTableView
{
    Q_OBJECT

public:
    explicit market_view(QWidget* parent = 0);

protected slots:
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

signals:
    void current_index_changed(const QModelIndex& current, const QModelIndex& previous);
};

#endif // MARKET_VIEW_H
