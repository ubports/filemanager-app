#include <QtQml>
#include <QtQml/QQmlContext>
#include "xdguserdir_plugin.h"
#include "xdguserdir.h"


void BackendPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String(QUOTES(PLUGIN_URI)));

    qmlRegisterType<XdgUserDir>(uri, 1, 0, "XdgUserDir");
}

void BackendPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}
