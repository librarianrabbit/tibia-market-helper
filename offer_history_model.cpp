#include "offer_history_model.h"
#include <QStringList>
#include <QLocale>

static QStringList g_columns = {
    "Name"
};

offer_history_model::offer_history_model(QObject* parent) :
    QAbstractTableModel(parent)
{
}

int offer_history_model::columnCount(const QModelIndex&) const
{
    return g_columns.size();
}

int offer_history_model::rowCount(const QModelIndex&) const
{
    return 0;
}

QVariant offer_history_model::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            return g_columns.value(section);
        }
    }

    return QVariant();
}

QVariant offer_history_model::data(const QModelIndex& index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.row() < 0 || index.row() > 0)
        {
            return QVariant();
        }

        switch (index.column())
        {
            case name_column:
                return "";
        }
    }
    else if (role == Qt::UserRole)
    {
        if (index.row() < 0 || index.row() > 0)
        {
            return QVariant();
        }

        return 0;
    }
    else if (role == Qt::UserRole + 1)
    {
        if (index.row() < 0 || index.row() > 0)
        {
            return QVariant();
        }

        return 0;
    }

    return QVariant();
}

Qt::ItemFlags offer_history_model::flags(const QModelIndex& index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void offer_history_model::sort(int column, Qt::SortOrder order)
{
    m_sort_column = column;
    m_sort_order = order;
}
