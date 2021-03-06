#ifndef MIMECACHE_H
#define MIMECACHE_H

#include <QObject>
#include <QStringList>

class QFileSystemWatcher;
class QSettings;

class MimeCache : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList keys READ getMimeKeys NOTIFY cacheUpdated)
private:
    QFileSystemWatcher * m_watcher;
    QSettings * m_mimecache;
    QStringList getMimeKeys();
public:
    explicit MimeCache(QObject *parent = 0);
    Q_INVOKABLE QStringList getIconPathsForMime(QString mime);
    Q_INVOKABLE void uncacheMime(QString mime);
    ~MimeCache();
signals:
    void cacheUpdated();
    //void valueUpdated();

public slots:
    void onMimeCacheChanged();
};

#endif // MIMECACHE_H
