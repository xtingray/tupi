#include "tupvideosurface.h"

struct TupVideoSurface::Private
{
    QWidget* targetWidget;
    VideoIF* videoIF;
    QVideoFrame frame;
    QImage::Format imageFormat;
    QSize displaySize;
    QImage previousImage;
    bool isScaled;
    bool showPrevious;
    bool safeArea;
    bool grid;
    double opacity;
};

TupVideoSurface::TupVideoSurface(QWidget *widget, VideoIF *target, const QSize &camResolution, 
                                 bool isScaled, QObject *parent) : QAbstractVideoSurface(parent), k(new Private)
{
    setNativeResolution(camResolution);

    k->isScaled = isScaled;
    k->targetWidget = widget;
    k->videoIF = target;
    k->imageFormat = QImage::Format_Invalid;
    k->displaySize = camResolution;
    k->safeArea = false;
    k->grid = false;
    k->showPrevious = false;
    k->opacity = 0.5;
}

TupVideoSurface::~TupVideoSurface()
{
}

bool TupVideoSurface::start(const QVideoSurfaceFormat &format)
{
    const QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();

    if (imageFormat != QImage::Format_Invalid && !size.isEmpty()) {
        k->imageFormat = imageFormat;
        QAbstractVideoSurface::start(format);
        return true;
    } else {
        return false;
    }
}

bool TupVideoSurface::present(const QVideoFrame &frame)
{
    k->frame = frame;
    if (surfaceFormat().pixelFormat() != k->frame.pixelFormat() ||
        surfaceFormat().frameSize() != k->frame.size()) {
        stop();
        return false;
    } else {
        k->videoIF->updateVideo();
        return true;
    }
}

void TupVideoSurface::paint(QPainter *painter)
{
     if (k->frame.map(QAbstractVideoBuffer::ReadOnly)) {
         int width = k->frame.width();
         int height = k->frame.height();
         QImage image(k->frame.bits(), width, height, k->frame.bytesPerLine(), k->imageFormat);

         if (k->isScaled) {
             image = image.scaledToWidth(k->displaySize.width(), Qt::SmoothTransformation);
             width = image.width();
             height = image.height();
         }

         QRect rect = k->targetWidget->rect();
         QPoint leftTop((qAbs(rect.size().width() - width)) / 2, 
                        (qAbs(rect.size().height() - height)) / 2);

         if (!image.isNull()) 
             painter->drawImage(leftTop, image);

         if (k->showPrevious) {
             if (!k->previousImage.isNull()) {
                 QPixmap transparent(k->previousImage.size());
                 transparent.fill(Qt::transparent);
                 QPainter p;
                 p.begin(&transparent);
                 p.setCompositionMode(QPainter::CompositionMode_Source);
                 p.drawPixmap(0, 0, QPixmap::fromImage(k->previousImage));
                 p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                 p.fillRect(transparent.rect(), QColor(0, 0, 0, opacity(k->opacity)));
                 p.end();
                 if (k->isScaled)
                     transparent = transparent.scaledToWidth(k->displaySize.width(), Qt::SmoothTransformation);
                 painter->drawPixmap(leftTop, transparent);
             }
         }

         if (k->grid) {
             painter->setPen(QPen(QColor(0, 0, 180, 50), 1));
             for (int i = 0; i <= width; i += 10)
                  painter->drawLine(i, 0, i, height);
             for (int i = 0; i <= height; i += 10)
                  painter->drawLine(0, i, width, i);
         }

         if (k->safeArea) {
             painter->setPen(QPen(QColor(255, 255, 255, 255), 1));
             int outerBorder = width/19;
             int innerBorder = width/6;

             int hSpace = width/3;
             int vSpace = height/3;
             int hSpace2 = hSpace*2;
             int vSpace2 = vSpace*2;

             QPoint rectLeft = image.rect().topLeft();
             QPoint rectRight = image.rect().bottomRight();

             QPointF left = rectLeft + QPointF(outerBorder, outerBorder);
             QPointF right = rectRight - QPointF(outerBorder, outerBorder);
             QRectF outerRect(left, right);

             painter->setPen(QPen(QColor(150, 150, 150, 255), 1));
             painter->drawRect(outerRect);

             painter->setPen(QPen(QColor(0, 135, 0, 255), 3));
             painter->drawLine(QPoint(hSpace, left.y() - 8), QPoint(hSpace, left.y() + 8));
             painter->drawLine(QPoint(hSpace - 5, left.y()), QPoint(hSpace + 5, left.y()));
             painter->drawLine(QPoint(hSpace2, left.y() - 8), QPoint(hSpace2, left.y() + 8));
             painter->drawLine(QPoint(hSpace2 - 5, left.y()), QPoint(hSpace2 + 5, left.y()));

             painter->drawLine(QPoint(hSpace, right.y() - 8), QPoint(hSpace, right.y() + 8));
             painter->drawLine(QPoint(hSpace - 5, right.y()), QPoint(hSpace + 5, right.y()));
             painter->drawLine(QPoint(hSpace2, right.y() - 8), QPoint(hSpace2, right.y() + 8));
             painter->drawLine(QPoint(hSpace2 - 5, right.y()), QPoint(hSpace2 + 5, right.y()));

             painter->drawLine(QPoint(left.x() - 8, vSpace), QPoint(left.x() + 8, vSpace));
             painter->drawLine(QPoint(left.x(), vSpace - 5), QPoint(left.x(), vSpace + 5));
             painter->drawLine(QPoint(left.x() - 8, vSpace2), QPoint(left.x() + 8, vSpace2));
             painter->drawLine(QPoint(left.x(), vSpace2 - 5), QPoint(left.x(), vSpace2 + 5));

             painter->drawLine(QPoint(right.x() - 8, vSpace), QPoint(right.x() + 8, vSpace));
             painter->drawLine(QPoint(right.x(), vSpace - 5), QPoint(right.x(), vSpace + 5));
             painter->drawLine(QPoint(right.x() - 8, vSpace2), QPoint(right.x() + 8, vSpace2));
             painter->drawLine(QPoint(right.x(), vSpace2 - 5), QPoint(right.x(), vSpace2 + 5));

             painter->setPen(QPen(QColor(0, 135, 0, 255), 1));

             left = rectLeft + QPointF(innerBorder, innerBorder);
             right = rectRight - QPointF(innerBorder, innerBorder);
             QRectF innerRect(left, right);

             painter->drawRect(innerRect);
         }

         k->frame.unmap();
     }
}

QList<QVideoFrame::PixelFormat> TupVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        return QList<QVideoFrame::PixelFormat>()
                << QVideoFrame::Format_RGB32
                << QVideoFrame::Format_ARGB32
                << QVideoFrame::Format_ARGB32_Premultiplied
                << QVideoFrame::Format_RGB565
                << QVideoFrame::Format_RGB555;
    } else {
        return QList<QVideoFrame::PixelFormat>();
    }
}

void TupVideoSurface::drawGrid(bool flag)
{
    k->grid = flag;
    k->videoIF->updateVideo();
}

void TupVideoSurface::drawActionSafeArea(bool flag)
{
    k->safeArea = flag;
    k->videoIF->updateVideo();
}

void TupVideoSurface::setLastImage(const QImage &image)
{
    k->previousImage = image;
}

int TupVideoSurface::opacity(double opacity)
{
    double value = opacity*255;
    return (int) value;
}

void TupVideoSurface::showHistory(bool flag)
{
    k->showPrevious = flag;
    k->videoIF->updateVideo();
}
