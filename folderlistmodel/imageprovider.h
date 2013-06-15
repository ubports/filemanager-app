#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#include <QQmlEngine>
#include <QQuickImageProvider>
#include <QFileInfo>
#include <QImage>
#include <QPainter>

class CoverArtImageProvider : public QQuickImageProvider
{
public:
    explicit CoverArtImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

};

class CoverArtFullImageProvider : public QQuickImageProvider
{
public:
    explicit CoverArtFullImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

};

#endif

#endif // IMAGEPROVIDER_H
