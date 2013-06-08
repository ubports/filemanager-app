#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

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

#endif // IMAGEPROVIDER_H
