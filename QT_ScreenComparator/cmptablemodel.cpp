#include "cmptablemodel.h"
#include <QIcon>

CmpTableModel::CmpTableModel(QPixmap _pix, QObject *parent)
    : QAbstractTableModel(parent), pix(_pix), m_data()
{
}

QVariant CmpTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    if(orientation == Qt::Orientation::Horizontal && role == Qt::DisplayRole)
    {
        switch(section){
        case 0:
            return QString("Screenshot");
        case 1:
            return QString("Similarity (%)");
        default:
            return QVariant();
        }
    }

    return QVariant();
}

int CmpTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.count();

}

int CmpTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 2;
}

bool CmpTableModel::hasChildren(const QModelIndex &parent) const
{

}

bool CmpTableModel::canFetchMore(const QModelIndex &parent) const
{
    return false;
}

void CmpTableModel::fetchMore(const QModelIndex &parent)
{

}

QVariant CmpTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DecorationRole && index.column() == 0)
    {
        QPixmap screen;
        screen.loadFromData(m_data[index.row()].first, "PNG");
        if(screen.isNull())
            return QVariant();
        screen = screen.scaled(screen.size()/10, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return QVariant(screen);
    }
    else if(role == Qt::DisplayRole && index.column() == 1)
    {
        return m_data[index.row()].second;
    }

    return QVariant();
}

bool CmpTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);



    endInsertRows();
    return true;
}

bool CmpTableModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);

    endInsertColumns();
    return true;
}

void CmpTableModel::updateCaptures(const QVector<QPair<QByteArray, ushort>>& data)
{
    m_data = data;
    emit dataChanged(index(0,0), index(m_data.count(), 1));
    emit layoutChanged();
}
