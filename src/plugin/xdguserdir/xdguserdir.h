#ifndef XDGUSERDIR_H
#define XDGUSERDIR_H

#include <QObject>

class XdgUserDir : public QObject
{
    Q_OBJECT

public:
    explicit XdgUserDir(QObject *parent = 0);
    ~XdgUserDir();
    enum Location {
        // TODO: add all possible locations
        Home,
        Documents,
        Music
    };
    QString getLocation(XdgUserDir::Location location);
};

#endif // XDGUSERDIR_H



