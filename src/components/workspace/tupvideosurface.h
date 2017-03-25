#ifndef TUPVIDEOSURFACE_H
#define TUPVIDEOSURFACE_H

#include "tglobal.h"

#include <QtGui>
#include <QWidget>
#include <QAbstractVideoSurface>
#include <QVideoRendererControl>
#include <QVideoSurfaceFormat>

class TUPI_EXPORT VideoIF {
    public:
        virtual void updateVideo() = 0;
};

class TUPI_EXPORT TupVideoSurface: public QAbstractVideoSurface
{
    Q_OBJECT

    public:
        TupVideoSurface(QWidget *widget, VideoIF *target, const QSize &displaySize, bool isScaled, QObject *parent = 0);
        ~TupVideoSurface();

        bool start(const QVideoSurfaceFormat &format);
        bool present(const QVideoFrame &frame);
        QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;
        void paint(QPainter *painter);
        void drawGrid(bool flag);
        void drawActionSafeArea(bool flag);
        void setLastImage(const QImage &image);
        void showHistory(bool flag);
        void updateImagesOpacity(double opacity);
        void updateImagesDepth(int depth);
        void updateGridSpacing(int space);
        void updateGridColor(const QColor color);

    private:
        void calculateImageDepth();
        struct Private;
        Private *const k;
};

#endif // TUPVIDEOSURFACE_H
