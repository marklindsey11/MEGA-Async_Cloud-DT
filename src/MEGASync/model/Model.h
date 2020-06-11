#pragma once

#include <memory>

#include <QString>
#include <QStringList>
#include <QMutex>

#include "control/Preferences.h"
#include "SyncSettings.h"

#include "megaapi.h"

class Preferences;

/**
 * @brief The Model class
 *
 * The idea of this class is to hold any state variable data.
 * and proceed to to persist that data when required.
 * It indens to alliviate Preferences from that burden.
 *
 * The Model would cover the funcionality related with with
 * updating of the data it manages (e.g: when a SyncConfiguration is updated,
 * it will call platform specific callbacks that deal with that)
 *
 *
 * The model will be updated when required and trigger events
 * when changes occur that should be noted by UI classes.
 *
 */
class Model : public QObject
{
    Q_OBJECT

signals:
    void syncStateChanged(std::shared_ptr<SyncSetting> syncSettings);
    void syncRemoved(std::shared_ptr<SyncSetting> syncSettings);

private:
    static Model *model;
    Model();

    ///////////////// SYNCS ///////////////////////
    Preferences *preferences;
    bool isFirstSyncDone = false;
    ///////////// END OF SYNCS ////////////////////

protected:
    QMutex syncMutex;

    ///////////////// SYNCS ///////////////////////
    void removeSyncSetting(std::shared_ptr<SyncSetting> syncSettings);
    void writeSyncSetting(std::shared_ptr<SyncSetting> syncSettings);

    QList<int> configuredSyncs; //Tags of configured syncs
    QMap<int, std::shared_ptr<SyncSetting>> configuredSyncsMap;
    QMap<int, QString> unconfiguredSyncsNames;
    ///////////// END OF SYNCS ////////////////////


public:
    void reset();
    static Model *instance();

    ///////////////// SYNCS ///////////////////////

    /**
     * @brief Updates sync model
     * @param sync MegaSync object with the required information
     * @param addingState to distinguish adding cases (from onSyncAdded)
     * @return
     */
    std::shared_ptr<SyncSetting> updateSyncSettings(mega::MegaSync *sync, int addingState = 0);
    void setSyncName(int tag, const QString &newName);

    // transition sync to active: will trigger platform dependent behaviour
    void activateSync(std::shared_ptr<SyncSetting> cs);
    // transition sync to inactive: will trigger platform dependent behaviour
    void deactivateSync(std::shared_ptr<SyncSetting> cs);

    // load into sync model the information from an old cached sync
    void pickInfoFromOldSync(const SyncData &osd, int tag);

    // remove syncs from model
    void removeSyncedFolder(int num);
    void removeSyncedFolderByTag(int tag);
    void removeAllFolders();

    // Getters
    std::shared_ptr<SyncSetting> getSyncSetting(int num);
    std::shared_ptr<SyncSetting> getSyncSettingByTag(int num);

    int getNumSyncedFolders();

    QStringList getSyncNames();
    QStringList getSyncIDs();
    QStringList getMegaFolders();
    QStringList getLocalFolders();
    QList<long long> getMegaFolderHandles();

    ///////////// END OF SYNCS ////////////////////

};
