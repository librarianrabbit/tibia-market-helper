#ifndef OFFER_HISTORY_MODEL_H
#define OFFER_HISTORY_MODEL_H

#include <QAbstractTableModel>

class offer_history_model : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum offer_history_columns
    {
        name_column = 0
    };

public:
    explicit offer_history_model(QObject* parent = 0);

    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

protected:
    int m_sort_column = name_column;
    Qt::SortOrder m_sort_order = Qt::AscendingOrder;
};

#endif // OFFER_HISTORY_MODEL_H
