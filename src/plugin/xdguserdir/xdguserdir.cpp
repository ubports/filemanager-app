#include "xdguserdir.h"
#include <QDebug>
#include <QDir>

XdgUserDir::XdgUserDir(QObject *parent) :
    QObject(parent)
{

}

XdgUserDir::~XdgUserDir() {

}

QString XdgUserDir::getLocation(XdgUserDir::Location location)
{
    Q_UNUSED(location)

    return QString("Test!");
}



