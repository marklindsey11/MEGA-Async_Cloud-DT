#ifndef ALERTITEM_H
#define ALERTITEM_H

#include <QWidget>
#include "megaapi.h"
#include <QFutureWatcher>
#include <memory>

namespace Ui {
class AlertItem;
}

namespace UserAttributes{
class FullNameAttributeRequest;
}

class AlertItem : public QWidget
{
    Q_OBJECT

public:
    explicit AlertItem(QWidget *parent = 0);
    ~AlertItem();

    void setAlertData(mega::MegaUserAlert *alert);
    void setAlertType(int type);
    void setAlertHeading(mega::MegaUserAlert *alert);
    void setAlertContent(mega::MegaUserAlert *alert);
    void setAlertTimeStamp(int64_t ts);

    QString getHeadingString();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    void refreshAlertItem(unsigned item);

protected:
    void changeEvent(QEvent *event);

private slots:
    void onAttributesReady();

private:
    void setAvatar(mega::MegaUserAlert *alert);
    QString formatRichString(QString str);

private:
    Ui::AlertItem *ui;
    mega::MegaApi *megaApi;
    QString mNotificationHeading;
    std::unique_ptr<mega::MegaNode> mAlertNode;
    std::unique_ptr<mega::MegaUserAlert> mAlertUser;
    std::shared_ptr<UserAttributes::FullNameAttributeRequest> mUserAttributes;
    QFutureWatcher<mega::MegaNode*> mAlertNodeWatcher;
};

#endif // ALERTITEM_H
