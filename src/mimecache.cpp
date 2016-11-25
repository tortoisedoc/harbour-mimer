#include "mimecache.h"

#include <QFileSystemWatcher>
#include <QSettings>
#include <QFile>
#include <QMap>
#include <QIcon>
#include <QDebug>

#include <contentaction5/contentaction.h>
#include <contentaction5/contentinfo.h>

QMap<QString, QStringList>* s_iconsCache = NULL;

#define DESKTOPS_HOME "/usr/share/applications"
#define MIME_CACHE DESKTOPS_HOME"/mimeinfo.cache"
#define CACHE_HEADER "MIME Cache"

MimeCache::MimeCache(QObject *parent) : QObject(parent){
    m_watcher = new QFileSystemWatcher();
    QString filename = QString(MIME_CACHE);
    m_watcher->addPath(filename);
    m_mimecache = NULL;
    s_iconsCache = new QMap<QString, QStringList>();
    connect(m_watcher, SIGNAL(fileChanged(QString)), this,
            SLOT(onMimeCacheChanged()));
}

MimeCache::~MimeCache(){
    //disconnect(m_watcher, SIGNAL(fileChanged(QString)), this);
    m_watcher = NULL;
    s_iconsCache->clear();
    free(s_iconsCache);
}

void MimeCache::onMimeCacheChanged(){
    if (m_mimecache){
        m_mimecache->deleteLater();
        m_mimecache = NULL;
    }
    m_mimecache = new QSettings(QString(MIME_CACHE),
                                QSettings::IniFormat, this);
    emit cacheUpdated();
}

QStringList MimeCache::getMimeKeys(){
    QStringList keys = QStringList();
    if (m_mimecache){
        m_mimecache->beginGroup(QString(CACHE_HEADER));
        keys = m_mimecache->allKeys();
        m_mimecache->endGroup();
    }
    return keys;
}

void MimeCache::uncacheMime(QString mime){
    if (mime != ""){
        if (s_iconsCache->find(mime) != s_iconsCache->end()){
            (*s_iconsCache)[mime].clear();
            s_iconsCache->remove(mime);
        }
    }
}

QStringList MimeCache::getIconPathsForMime(QString mime){
    QStringList picPaths;
    QStringList desktops;
    if (mime != ""){
        if (s_iconsCache->find(mime) == s_iconsCache->end()){
            //Defaults are always the first ones..

            QList<ContentAction::Action> actions =
                    ContentAction::actionsForMime(mime);
            foreach (ContentAction::Action action, actions){
                if (QIcon::hasThemeIcon(action.icon()))
                    picPaths << QString ("image://theme/") + action.icon();
                else {
                    if (!QFile::exists(action.icon()))
                        picPaths << QString ("image://theme/") + action.icon();
                    else picPaths << action.icon();
                }

                desktops << action.name();
            }
            qDebug() << picPaths << QString("##")
                     << mime << QString(" // ") << desktops;
            (*s_iconsCache)[mime] = picPaths;
            return picPaths;
        }else return (*s_iconsCache)[mime];
    }
    return QStringList();
}
