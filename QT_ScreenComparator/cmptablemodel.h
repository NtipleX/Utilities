#ifndef CMPTABLEMODEL_H
#define CMPTABLEMODEL_H

#include <QAbstractTableModel>
#include <QPixmap>

class CmpTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CmpTableModel(QPixmap pix, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // Fetch data dynamically:
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent) const override;
    void fetchMore(const QModelIndex &parent) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    QPixmap pix;
    int adaw = 0;

    void updateCaptures(const QVector<QPair<QByteArray, ushort>>& data);

private:
    QVector<QPair<QByteArray, ushort>> m_data;
};

#endif // CMPTABLEMODEL_H
