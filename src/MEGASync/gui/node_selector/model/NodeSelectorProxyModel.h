#ifndef NODESELECTORPROXYMODEL_H
#define NODESELECTORPROXYMODEL_H

#include "megaapi.h"

#include <QSortFilterProxyModel>
#include <QCollator>
#include <QFutureWatcher>
#include <QEventLoop>

#include <memory>


namespace mega{
class MegaNode;
}
class NodeSelectorModel;

class NodeSelectorProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    struct Filter{
        bool showReadOnly = true;
        bool showReadWriteFolders = true;
        Filter() : showReadOnly(true),
                    showReadWriteFolders(true){};
    };

    explicit NodeSelectorProxyModel(QObject* parent = nullptr);

    void showReadOnlyFolders(bool value);
    void showReadWriteFolders(bool value);
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    mega::MegaHandle getHandle(const QModelIndex &index);
    std::shared_ptr<mega::MegaNode> getNode(const QModelIndex& index);
    void addNode(std::unique_ptr<mega::MegaNode> node, const QModelIndex& parent);
    QModelIndex getIndexFromSource(const QModelIndex& index);
    QModelIndex getIndexFromHandle(const mega::MegaHandle& handle);
    QModelIndex getIndexFromNode(const std::shared_ptr<mega::MegaNode> node);
    QVector<QModelIndex> getRelatedModelIndexes(const std::shared_ptr<mega::MegaNode> node);
    void removeNode(const QModelIndex &item);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    void setExpandMapped(bool value){mExpandMapped = value;}
    NodeSelectorModel* getMegaModel();
    bool isModelProcessing() const;

    virtual bool canBeDeleted() const;

signals:
    void expandReady();
    void navigateReady(const QModelIndex& index);
    void modelAboutToBeChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QVector<QModelIndex> forEach(std::shared_ptr<mega::MegaNodeList> parentNodeList, QModelIndex parent = QModelIndex());
    Filter mFilter;
    QCollator mCollator;
    int mSortColumn;
    Qt::SortOrder mOrder;
    QFutureWatcher<void> mFilterWatcher;
    QEventLoop loop;
    QModelIndexList itemsToMap;
    bool mExpandMapped;


private slots:
    void invalidateModel(const QModelIndexList &parents);
    void onModelSortedFiltered();
};

#endif // NODESELECTORPROXYMODEL_H
