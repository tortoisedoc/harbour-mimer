#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <sailfishapp.h>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QtQml/QQmlContext>
#include <QQuickView>
#include "mimecache.h"
#include "desktopitemsmgr.h"

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/template.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    QGuiApplication * app = SailfishApp::application(argc, argv);
    QQuickView * mainView = SailfishApp::createView();

    QQmlContext * root = mainView->engine()->rootContext();
    MimeCache * tempCache = new MimeCache();
    if (root){
        root->setContextProperty("MimeCache", tempCache);
        root->setContextProperty("appsModel",
                                 new DesktopItemsMgr((QObject*)app));
        root->setContextProperty("defaultAppsModel",
                                 (QObject*)new DesktopItemsMgr((QObject*)app));
        root->setContextProperty("userDefaultAppsModel",
                                 (QObject*)new DesktopItemsMgr((QObject*)app));
        root = NULL;
    }
    mainView->setSource(SailfishApp::pathTo("qml/harbour-mimer.qml"));
    tempCache->onMimeCacheChanged();
    mainView->show();
    return app->exec();
}

