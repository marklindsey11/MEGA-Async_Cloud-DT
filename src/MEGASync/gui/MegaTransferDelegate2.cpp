#include "MegaTransferDelegate2.h"
#include "control/Utilities.h"
#include "Preferences.h"
#include "gui/QMegaMessageBox.h"
#include "megaapi.h"
#include "QTransfersModel2.h"
#include "MegaApplication.h"
#include "platform/Platform.h"
#include "TransfersWidget.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QToolTip>
#include <QSortFilterProxyModel>

using namespace mega;

MegaTransferDelegate2::MegaTransferDelegate2(QAbstractItemModel* model, QWidget* view, QObject *parent)
    : QStyledItemDelegate(parent),
      mModel(model),
      mView(view)
{
    QObject::connect(static_cast<TransfersWidget*>(parent), &TransfersWidget::clearTransfers,
            this, &MegaTransferDelegate2::onClearTransfers);
}

void MegaTransferDelegate2::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto transferItem (qvariant_cast<TransferItem2>(index.data(Qt::DisplayRole)));

        const auto nbRowsMaxInView (mView->height() / option.rect.height() + 1);
        const QString widgetName (QLatin1Literal("r")+QString::number(index.row() % nbRowsMaxInView));

        auto w (mView->findChild<TransferManagerItem2 *>(widgetName));

        if (!w)
        {
            w = new TransferManagerItem2(mView);
            w->setObjectName(widgetName);
            connect(w, &TransferManagerItem2::clearTransfers,
                    this, &MegaTransferDelegate2::onClearTransfers);
        }
        w->resize(option.rect.size());
        w->move(option.rect.topLeft());

        w->updateUi(transferItem.getTransferData(), index.row());

        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, QColor(247, 247, 247));
        }
        painter->save();

        painter->translate(option.rect.topLeft());
        w->render(painter, QPoint(0, 0), QRegion(0, 0, option.rect.width(), option.rect.height()));

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize MegaTransferDelegate2::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(720, 64);
}

void MegaTransferDelegate2::onClearTransfers(int firstRow, int amount)
{
    mModel->removeRows(firstRow, amount, QModelIndex());
}

void MegaTransferDelegate2::processCancel(int tag)
{
//    if (model->getModelType() == QTransfersModel::TYPE_FINISHED)
//    {
//        model->removeTransferByTag(tag);
//    }
//    else
//    {
//        QPointer<QTransfersModel> modelPointer = model;

//        QMessageBox warning;
//        HighDpiResize hDpiResizer(&warning);
//        warning.setWindowTitle(QString::fromUtf8("MEGAsync"));
//        warning.setText(tr("Are you sure you want to cancel this transfer?"));
//        warning.setIcon(QMessageBox::Warning);
//        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
//        warning.setDefaultButton(QMessageBox::No);
//        int result = warning.exec();
//        if (modelPointer && result == QMessageBox::Yes)
//        {
//            model->megaApi->cancelTransferByTag(tag);
//        }
//    }
}

bool MegaTransferDelegate2::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (index.isValid())
    {
        switch (event->type())
        {
            case QEvent::MouseButtonPress:
            {
                QMouseEvent* me = static_cast<QMouseEvent*>(event);
                if( me->button() == Qt::LeftButton )
                {
                    // Get TransferManagerItem2 widget under cursor
                    const auto nbRowsMaxInView (mView->height() / option.rect.height() + 1);
                    const QString widgetName (QLatin1Literal("r")+QString::number(index.row() % nbRowsMaxInView));
                    auto currentRow (option.widget->findChild<TransferManagerItem2 *>(widgetName));
                    if (currentRow)
                    {
                        currentRow->forwardMouseEvent(me);
                    }
                }
                break;
            }
            default:
                break;
        }
    }


    //    if (QEvent::MouseButtonPress ==  event->type())
//    {
//        int tag = index.internalId();
//        TransferItem *item = model->transferItems[tag];
//        if (!item)
//        {
//            return true;
//        }

//        if (item->cancelButtonClicked(((QMouseEvent *)event)->pos() - option.rect.topLeft()))
//        {
//            processCancel(tag);
//            return true; // click consumed
//        }
//        else if (item && item->checkIsInsideButton(((QMouseEvent *)event)->pos() - option.rect.topLeft(), TransferItem::ACTION_BUTTON))
//        {
//            int modelType = model->getModelType();
//            if (modelType == QTransfersModel::TYPE_FINISHED
//                    || modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
//            {
//                const auto &ti = model->transferItems[tag];
//                if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS && !ti->getIsLinkAvailable() && !ti->getTransferError())
//                {
//                    processShowInFolder(tag);
//                }
//                else if (MegaTransfer *transfer = model->getTransferByTag(tag) )
//                {
//                    if (!transfer->getLastError().getErrorCode())
//                    {
//                        QList<MegaHandle> exportList;
//                        QStringList linkList;

//                        MegaNode *node = transfer->getPublicMegaNode();
//                        if (!node || !node->isPublic())
//                        {
//                            exportList.push_back(transfer->getNodeHandle());
//                        }
//                        else
//                        {
//                            char *handle = node->getBase64Handle();
//                            char *key = node->getBase64Key();
//                            if (handle && key)
//                            {
//                                QString link = Preferences::BASE_URL + QString::fromUtf8("/#!%1!%2")
//                                        .arg(QString::fromUtf8(handle)).arg(QString::fromUtf8(key));
//                                linkList.append(link);
//                            }
//                            delete [] handle;
//                            delete [] key;
//                        }
//                        delete node;

//                        if (exportList.size() || linkList.size())
//                        {
//                            ((MegaApplication*)qApp)->exportNodes(exportList, linkList);
//                        }
//                    }
//                    else
//                    {
//                        ((MegaApplication*)qApp)->getMegaApi()->retryTransfer(transfer);
//                    }

//                    delete transfer;
//                }
//             }
//             return true; // click consumed
//        }
//        else if (item && item->checkIsInsideButton(((QMouseEvent *)event)->pos() - option.rect.topLeft(), TransferItem::SHOW_IN_FOLDER_BUTTON))
//        {
//            if (model->getModelType() == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
//            {
//                processShowInFolder(tag);
//                return true; // click consumed
//            }
//            // we are not consuming the click; fall through to do the usual thing (of selecting the clicked row)
//        }
//    }
//    else if (QEvent::MouseButtonDblClick == event->type() && model->getModelType() == QTransfersModel::TYPE_FINISHED)
//    {
//        int tag = index.internalId();
//        processShowInFolder(tag);
//        return true; // double-click consumed
//    }

    return QAbstractItemDelegate::editorEvent(event, mModel, option, index);
}

bool MegaTransferDelegate2::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::ToolTip && index.isValid())
    {
        // Get TransferManagerItem2 widget under cursor
        const auto nbRowsMaxInView (mView->height()/option.rect.height() + 1);
        const QString widgetName (QLatin1Literal("r")+QString::number(index.row() % nbRowsMaxInView));
        auto currentRow (view->findChild<TransferManagerItem2 *>(widgetName));
        if (currentRow)
        {
            // Get widget inside TransferManagerItem2 under cursor, and display its tooltip
            auto widget (currentRow->childAt(event->pos() - currentRow->pos()));
            if (widget)
            {
                QToolTip::showText(event->globalPos(), widget->toolTip());
            }
        }
    }
    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

void MegaTransferDelegate2::processShowInFolder(int tag)
{
//    MegaTransfer *transfer = NULL;
//    transfer = model->getTransferByTag(tag);
//    if (transfer && transfer->getState() == MegaTransfer::STATE_COMPLETED
//                 && transfer->getPath())
//    {
//        QString localPath = QString::fromUtf8(transfer->getPath());
//        #ifdef WIN32
//        if (localPath.startsWith(QString::fromAscii("\\\\?\\")))
//        {
//            localPath = localPath.mid(4);
//        }
//        #endif
//        Platform::showInFolder(localPath);
//    }
//    delete transfer;
}


