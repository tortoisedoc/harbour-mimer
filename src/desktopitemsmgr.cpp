#include "desktopitemsmgr.h"
#include <QDir>
#include <QStringList>
#include <QIcon>
#include <QDebug>
#include <QSettings>
#include <QProcess>

#include <mlite5/MDesktopEntry>
#include <contentaction5/contentaction.h>

#define APPS_HOME "/usr/share/applications"
#define SECTION_DEFAULT_APPS "Default Applications"
#define USER_DEFAULTS_HOME "/home/nemo/.local/share/applications/mimeapps.list"

enum Roles {
    NameRole = Qt::UserRole+1,
    IconFileRole, //258
    DesktopFileRole //259
};

DesktopItemsMgr::DesktopItemsMgr(QObject *parent) : QAbstractListModel(parent){    
}

QHash<int, QByteArray>DesktopItemsMgr::roleNames() const{
    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[IconFileRole] = "icon";
    names[DesktopFileRole] = "desktopFile";
    return names;
}

void DesktopItemsMgr::loadAppsFromMime(QString mime, bool user){
    this->clear();
    QSettings userDefaults(USER_DEFAULTS_HOME, QSettings::IniFormat);
    qDebug () << userDefaults.allKeys();
    QList<ContentAction::Action> actions =
            ContentAction::actionsForMime(mime);
    if (mime =="text/plain")
        mime = mime + "";
    foreach (ContentAction::Action action, actions){
        qDebug() << action.name();
        QString key = QString(SECTION_DEFAULT_APPS"/")+
                            mime;
        QString value = userDefaults.value(key, "").toString();
        bool userDefined = value == action.name() + ".desktop";
        if ((user && userDefined) || (!user && !userDefined)){
            MDesktopEntry * tempDesk = new MDesktopEntry(QString(APPS_HOME) +
                                                     QString("/")+
                                                 action.name() +
                                                 QString(".desktop"));
            m_desktops.push_back(tempDesk);
        }
    }
}

int DesktopItemsMgr::getCount(){
    return m_desktops.count();
}

void DesktopItemsMgr::loadAppsFromPath(QString path){
    QDir tempDir (path);
    QStringList filters;
    filters << "*.desktop";
    tempDir.setNameFilters(filters);

    QStringList files;
    // ciafa i files co se roea por .desktop
    files = tempDir.entryList(filters);
    foreach (QString file, files){
        MDesktopEntry* tempEntry = new MDesktopEntry(path + "/" + file);
        if (tempEntry->isValid() && !tempEntry->hidden()
                && !tempEntry->noDisplay())
            m_desktops.push_back(tempEntry);
        else free(tempEntry);
    }
}

int DesktopItemsMgr::rowCount(const QModelIndex &parent) const{
    Q_UNUSED(parent);
    return m_desktops.count();
}

QVariant DesktopItemsMgr::data(const QModelIndex &index, int role) const{
    if (index.row() < m_desktops.count()){
       MDesktopEntry * tempDesk = m_desktops[index.row()];
       QFileInfo tempInfo;
       switch (role){
            case NameRole:
                return tempDesk->name();
            case IconFileRole:
                if (QIcon::hasThemeIcon(tempDesk->icon()))
                    return tempDesk->icon();
                else {
                    if (!QFile::exists(tempDesk->icon()))
                        return QString ("image://theme/" +
                                   tempDesk->icon());
                    else return tempDesk->icon();
                }
            case DesktopFileRole:
                tempInfo = QFileInfo (tempDesk->fileName());
                return tempInfo.fileName();
            default: return QVariant();
        }

    }
    else return QVariant();
}

DesktopItemsMgr::~DesktopItemsMgr(){
    this->clear();
}

void DesktopItemsMgr::clear(){
    foreach (MDesktopEntry* tempEntry, m_desktops){
        free (tempEntry);
    }
    m_desktops.clear();
}

Qt::ItemFlags DesktopItemsMgr::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable
            | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool DesktopItemsMgr::setData(const int &index,
                              const QVariant &value,
                              const QVariant &mime){


    MDesktopEntry*tempDesk  = NULL;
    if (index > -1){
        tempDesk = m_desktops[index];
        free(tempDesk);
        tempDesk = NULL;
    }

    QString desktopFile = QString(APPS_HOME) + QString("/") +
                            value.toString();

    tempDesk = new MDesktopEntry(desktopFile);
    if (index > -1)
        m_desktops[index] = tempDesk;
    else
        m_desktops.push_back(tempDesk);

    QVector<int> tempVect;

    if (value != "" && mime != ""){
        QStringList args;
        args << "default"
             << value.toString()
             << mime.toString();
        QProcess::execute("xdg-mime", args);
    }
    if (index > -1)
        emit dataChanged(this->index(index), this->index(index), tempVect);
    else
        emit dataChanged(this->index(0), this->index(0), tempVect);
    return true;
}
