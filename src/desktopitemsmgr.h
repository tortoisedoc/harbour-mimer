#ifndef DESKTOPITEMSMGR_H
#define DESKTOPITEMSMGR_H

#include <QAbstractListModel>
#include <QList>
#include <QHash>
#include <QByteArray>

class MDesktopEntry;

class DesktopItemsMgr : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ getCount())
    QList<MDesktopEntry*> m_desktops;
private:
    void clear();
    int getCount();
public:
    explicit DesktopItemsMgr(QObject *parent = 0);
    Q_INVOKABLE void loadAppsFromPath(QString path);
    Q_INVOKABLE void loadAppsFromMime(QString mime, bool user=false);
    Q_INVOKABLE virtual bool setData(const int &index,
                         const QVariant &value, const QVariant &mime);
    ~DesktopItemsMgr();
protected:
    virtual QHash<int, QByteArray>roleNames() const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // DESKTOPITEMSMGR_H
