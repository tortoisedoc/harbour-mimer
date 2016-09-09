#include "mimecache.h"

#include <QFileSystemWatcher>
#include <QSettings>
#include <QMap>
#include <QIcon>

#include <QDebug>

#include <contentaction5/contentaction.h>
#include <contentaction5/contentinfo.h>

#define DESKTOPS_HOME "/usr/share/applications"
#define MIME_CACHE DESKTOPS_HOME"/mimeinfo.cache"
#define CACHE_HEADER "MIME Cache"


typedef QMap<QString, MimeObject*> QMimeInfoCache;
QMimeInfoCache* s_mimeInfoCache = NULL;

void clear_mime_cache(){
    QMimeInfoCache::iterator tempIter = s_mimeInfoCache->begin();
    while (tempIter != s_mimeInfoCache->end()){
        (*tempIter)->m_actions.clear();
        free(*tempIter);
        tempIter++;
    }
    s_mimeInfoCache->clear();
}


MimeCache::MimeCache(QObject *parent) : QObject(parent){
    m_watcher = new QFileSystemWatcher();
    QString filename = QString(MIME_CACHE);
    m_watcher->addPath(filename);
    m_mimecache = NULL;
    s_mimeInfoCache = new QMimeInfoCache();
    connect(m_watcher, SIGNAL(fileChanged(QString)), this,
            SLOT(onMimeCacheChanged()));
}

MimeCache::~MimeCache(){
    m_watcher = NULL;
    clear_mime_cache();
    delete(s_mimeInfoCache);
}

void MimeCache::onMimeCacheChanged(){
    if (m_mimecache){
        m_mimecache->deleteLater();
        m_mimecache = NULL;
    }
    m_mimecache = new QSettings(QString(MIME_CACHE),
                                QSettings::IniFormat, this);
    clear_mime_cache();

    foreach(QString mime, this->getMimeKeys()){
        if (s_mimeInfoCache->find(mime) == s_mimeInfoCache->end()){
            //Defaults are always the first ones..

            s_mime_struct * tempStr = new MimeObject();
            tempStr->actions = ContentAction::actionsForMime(mime);
            tempStr->content_info = ContentInfo::forMime(mime);
        }
    }
    emit cacheUpdated();
}

QStringList MimeCache::getMimeKeys(){
    if (m_mimecache){
        QStringList keys = QStringList();
        m_mimecache->beginGroup(QString(CACHE_HEADER));
        keys = m_mimecache->allKeys();
        m_mimecache->endGroup();
        return keys;
    }
    return QStringList();

}

QString MimeCache::getMimeValue(QString key){
    if (m_mimecache){
        QString mykey = QString(CACHE_HEADER"/") + key;
        return m_mimecache->value(mykey).toString();
    }
    else return QString();
}

void MimeCache::setMimeValue(QString key, QString value){
    if (m_mimecache){
        m_mimecache->setValue(QString(CACHE_HEADER"/") + key, value);
    }
}

void MimeCache::appendMimeValue(QString key, QString value){
    if (m_mimecache){
        this->setMimeValue(key,
                           this->getMimeValue(key)
                               + QString(";") + value
                           );
    }
}

QVariant MimeCache::getMimeObjectByKey(QString mime){
    if (s_mimeInfoCache->find(mime) != s_mimeInfoCache->end()){
        return QVariant((*s_mimeInfoCache)[mime]);
    }
}


QStringList MimeObject::getIconPaths(){
    QStringList picPaths;

    //Defaults are always the first ones..
    ContentInfo tempInfo = this->m_content_info;
    if (QIcon::hasThemeIcon("icon-l-" +
                            tempInfo.typeIcon()))
        picPaths << QString ("image://theme/icon-l-" +
                          tempInfo.typeIcon());
    else {
        picPaths << QString ("image://theme/" +
                       tempInfo.typeIcon());
    }

    qDebug() << picPaths << QString("##") << mime << QString(" // ") ;
    return picPaths;
}

