/*
 *  KTone
 *  Copyright 2013 Giulietta. Duec i diric reservees.
 *
 */

#ifdef SAILFISHOS
#include <mlite5/MDesktopEntry>
#include <QRegularExpression>
#else
#include <MDesktopEntry>
#endif
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSettings>

#include "MDebotaExeSync.h"
#include "debotautils.h"
#include "global.h"

MDebotaExeSync* MDebotaExeSync::Instance(){
    static MDebotaExeSync* s_instance = NULL;
    if (s_instance == NULL)
        s_instance = new MDebotaExeSync;
    return s_instance;
}

QString MDebotaExeSync::matchCmdLine(QString section, QString cmd,
                                     QSettings* pSettings){
    QString de("");
    if (pSettings && pSettings->childGroups().contains(section)){        
        pSettings->beginGroup(section);
        QStringList keys(pSettings->allKeys());

        foreach (const QString &key, keys){
            QRegularExpression regExp(key);            
            if (regExp.match(cmd).hasMatch()){
                de = DebotaUtils::fejStradaFile(pSettings->value(key).toString());
                break;
            }
        }
        if (de == ""){ // Nia ciafe; -> met en default (le proem)?
            //FIXME : ciara da ciafe en modo por pie le dert .desktop
            QString tempDe = pSettings->value(keys[0]).toString();
            de = DebotaUtils::fejStradaFile(tempDe);
        }
        pSettings->endGroup();
    }

    qDebug() << "DE E " << de;
    return de;
}

QString MDebotaExeSync::isSysParam(QString arg){
    if ((arg == "%f") || (arg == "%F")
            || (arg == "%u") || (arg == "%U")
            || (arg == "%c") || (arg == "%k")){
        return ".*"; //Url / Files
    }else if (arg =="%i")
        return "--icon";
    return "";
}

QString MDebotaExeSync::processCmdLine(QStringList & execArgs,
                                       int &exeIndex, QString icon){
    //QStringList execArgs = exec.split(" ");
    QString cmdString("");
    exeIndex = -1;
    for (int i=0; i<execArgs.size(); i++){
        QString _tempArg = execArgs[i];
        if (MDebotaExeSync::isExeFile(_tempArg)){
            exeIndex = i;
            if (cmdString != "")
                cmdString += "_";
            cmdString+= QString(".*") + _tempArg;
        }else {
            QString expandParam = MDebotaExeSync::isSysParam(_tempArg);
            if (expandParam != ""){
                if (expandParam.startsWith("--icon")){
                    cmdString += "_" + expandParam;
                    cmdString += "_" + icon;
                }
                else cmdString += expandParam;
            }else {
                if (cmdString != "")
                    cmdString += "_";
                if (i == 0 && _tempArg.startsWith("invoker"))
                    cmdString+= QString(".*") + _tempArg;
                else
                    cmdString+= _tempArg;
            }
        }
    }
    return cmdString;
}

bool MDebotaExeSync::isExeFile(QString arg){
    if (arg.indexOf("invoker")== -1){
        if (!arg.startsWith("%%") &&(
                (arg.startsWith("/usr/bin") && QFile::exists(arg))
                || (!arg.startsWith("/usr/bin")
                    && QFile::exists("/usr/bin/" + arg))
                )){
            return true;
        }
    }
    return false;
}

void MDebotaExeSync::syncExe(QString stradaCartella, bool _reset){
    QStringList filters;
    QDir tempDir (stradaCartella);
    filters << "*.desktop";
    tempDir.setNameFilters(filters);

    QSettings tempSettings (ENNOM_ORGANISAZIUN, ENNOM_APPLICAZIUN);
    QMap <QString,SettingsMap*> cmdLines;

    SettingsMap servicePaths(SEZIUN_STRADA);
    SettingsMap serviceMethods(SEZIUN_METODI);

    if (_reset)
        tempSettings.clear();
    QStringList files;
    files = tempDir.entryList(filters);	// ciafa i files co se roea por .desktop
    int index = 0;

    QStringList serviceList;
    foreach (QString ennomFile, files){
        if ((ennomFile.toLower().indexOf("quicklaunch") == -1)
                && (ennomFile.toLower() != QString("lancia_harmattan.desktop"))){
            //no includere i quick launch, y nianca nos enstesc
            QString ennomDE = stradaCartella + QString("/") + ennomFile;

            MDesktopEntry tempDE(ennomDE);            
            QString noDisplay = tempDE.value("Desktop Entry", "NoDisplay");
            QString isVisible = tempDE.value("Desktop Entry", "NotShownIn");
            QString icon = tempDE.value("Desktop Entry", "Icon");
            QString apkFile = tempDE.value("Desktop Entry", "X-apkd-apkfile");
            QString exec = tempDE.exec();
            if (apkFile != ""){
                QString last = exec.split(" ").last();
                if (last != apkFile )
                    exec = "apkd-launcher " + apkFile + " " + last;
                else
                    exec = "apkd-launcher " + apkFile;
            }

            QString service;
            QString method;
            QString path;

            service = tempDE.value ("Desktop Entry", "X-Maemo-Service");
            method = tempDE.value("Desktop Entry", "X-Maemo-Method");
            path = tempDE.value("Desktop Entry", "X-Maemo-Object-Path");

            if (service == "" && tempDE.contains("Desktop Entry","X-Osso-Service" ))
                service = tempDE.value ("Desktop Entry", "X-Osso-Service");

            if (method != ""){
                service = method.left(method.lastIndexOf("."));
                serviceMethods[service] = method;
            }

            if (path != "")
                servicePaths[service] = path;

            if (service != ""
                    && serviceList.indexOf(service) == -1){
                //Sce le servizio e defini, libcontentaction ignoreia le exec
                serviceList.push_back(service);
            }

            if (service == ""
                    || (service != ""
                        && noDisplay == ""
                        && isVisible == "")){
                int exeIndex = -1;

                QStringList execArgs = exec.split(" ");
                QString cmdString = MDebotaExeSync::processCmdLine(execArgs,
                                                                   exeIndex, icon);

                if (exeIndex > -1){
                    QString exe = DebotaUtils::convertEnnom(execArgs[exeIndex]);
                    SettingsMap * tempMap;
                    if (cmdLines.keys().indexOf(exe) > -1){
                        tempMap = cmdLines[exe];
                    }else{
                        tempMap = new SettingsMap(exe);
                        cmdLines[exe] = tempMap;
                    }
                    (*tempMap)[cmdString] =DebotaUtils::convertEnnom(ennomDE);
                }
            }
        }
        index++;
    }

    //Salva les .desktop eseguibili ciafades..
    QMap <QString,SettingsMap*>::iterator tempIter = cmdLines.begin();
    while (tempIter != cmdLines.end()){
        (tempIter).value()->save(&tempSettings);
        (tempIter).value() = NULL;
        tempIter++;
    }

    //Ciaria DBus service configurations..
    QDir assetsFolder = QDir(STRADA_DBUS_SERVIZI);
    filters << "*.service";
    assetsFolder.setNameFilters(filters);
    assetsFolder.setFilter(QDir::Files);

    QList<QSettings*> settings_list;
    QFileInfoList fileinfos = assetsFolder.entryInfoList();
    foreach (QFileInfo fi, fileinfos) {
        QSettings* settings = new QSettings(fi.absoluteFilePath(),
                                            QSettings::IniFormat);
        settings_list.push_back(settings);
    }

    //Associeia en .desktop file ai servizi co n'en a oen
    foreach (QString service, serviceList){
        QList<QSettings*>::iterator tempIter = settings_list.begin();
        QString exeCmd = "";
        //Ciafa cmdLine co corrispondeia al servizio da D-BUS
        while (tempIter != settings_list.end()){
            QSettings*tempSet = (*tempIter);
            QString tempName = tempSet->value("D-BUS Service/Name").toString();
            if (tempName == service){
                exeCmd = tempSet->value("D-BUS Service/Exec").toString();
                break;
            }
            tempIter ++;
        }

        if (exeCmd != ""){
            QStringList execArgs = exeCmd.split(" ");
            int exeIndex = -1;
            QString cmdString = MDebotaExeSync::processCmdLine(execArgs,
                                                               exeIndex, "");
            if (exeIndex > -1){
                QString exe = DebotaUtils::convertEnnom(execArgs[exeIndex]);

                //Ciafa desktop de default...
                QString de("");
                de = MDebotaExeSync::matchCmdLine(exe, cmdString, &tempSettings);
                if (de != ""){
                    //qDebug() << " MATCHING SERVIZIO " << service << "/" << de;
                    tempSettings.setValue(SEZIUN_SERVIZI + QString("/") + service,
                                          DebotaUtils::convertEnnom(de));
                }
                else{
                    qDebug() << " ERROR : EXE " << exe
                             << " NIA ASSOCIADA; SERVIZIO " << service;
                }
            }
        }
    }    

    QList<QSettings*>::iterator tempIter2 = settings_list.begin();
    while (tempIter2 != settings_list.end()){
        (*tempIter2)->deleteLater();
        tempIter2++;

    }
    settings_list.clear();

    serviceMethods.save(&tempSettings);
    servicePaths.save(&tempSettings);
    tempSettings.sync();

}


bool MDebotaExeSync::mesFaSync(){
    QSettings tempSettings (ENNOM_ORGANISAZIUN, ENNOM_APPLICAZIUN);

    bool aVer = tempSettings.childGroups().contains(SEZIUN_VER);
    QString version = "";
    if (aVer){
        tempSettings.beginGroup(SEZIUN_VER);
        version = tempSettings.value(SEZIUN_VER).toString();
        tempSettings.endGroup();
    }
    bool res = tempSettings.childGroups().contains(SEZIUN_EXES)
            && (aVer && version == QString(LANCIA_VERSCIUN));
    if (!res){
        tempSettings.beginGroup(SEZIUN_VER);
        tempSettings.setValue("versciun", QString(LANCIA_VERSCIUN));
        tempSettings.endGroup();
        tempSettings.sync();
    }
    return !res;
}

void MDebotaExeSync::reload_desktops(const QString& _dir){
    qDebug () << "RELOADING DESKTOPS...";
    this->syncExe(STRADA_CIASA_SYS_DESKTOP_FILES);
    qDebug () << "DONE...";
}
