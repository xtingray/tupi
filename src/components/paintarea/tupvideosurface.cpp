#include "tupvideosurface.h"
#include "tdebug.h"

struct TupVideoSurface::Private
{
    QWidget* targetWidget;
    VideoIF* videoIF;
    QVideoFrame frame;
    QImage::Format imageFormat;
    QSize displaySize;
    QList<QImage> history;
    int widgetWidth;
    int widgetHeight;

    bool isScaled;
    bool showPrevious;
    bool safeArea;
    bool grid;
    int opacity;
    int historySize;
    int gridSpace;
    int historyInit;
    int historyEnd;

    QPen gridPen;
    QPen whitePen;
    QPen grayPen;
    QPen greenThickPen;
    QPen greenThinPen;
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
    k->opacity = 127;
    k->historySize = 1; 
    k->gridSpace = 10;
    k->historyInit = 0;
    k->historyEnd = 0;

    k->gridPen = QPen(QColor(0, 0, 180, 50), 1);
    k->whitePen = QPen(QColor(255, 255, 255, 255), 1);
    k->grayPen = QPen(QColor(150, 150, 150, 255), 1);
    k->greenThickPen = QPen(QColor(0, 135, 0, 255), 3);
    k->greenThinPen = QPen(QColor(0, 135, 0, 255), 1);

    QRect rect = k->targetWidget->rect();
    k->widgetWidth = rect.size().width();
    k->widgetHeight = rect.size().height();
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

         QPoint leftTop((qAbs(k->widgetWidth - width))/2, (qAbs(k->widgetHeight - height))/2);

         if (!image.isNull()) 
             painter->drawImage(leftTop, image);

         if (k->showPrevious && !k->history.empty() && k->historySize > 0) {
             for (int i=k->historyInit; i <= k->historyEnd; i++) {
                  QImage image = k->history.at(i);
                  QPixmap transparent(image.size());
                  transparent.fill(Qt::transparent);
                  QPainter p;
                  p.begin(&transparent);
                  p.setCompositionMode(QPainter::CompositionMode_Source);
                  p.drawPixmap(0, 0, QPixmap::fromImage(image));
                  p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
                  p.fillRect(transparent.rect(), QColor(0, 0, 0, k->opacity));
                  p.end();
                  painter->drawPixmap(leftTop, transparent);
             }
         }

         if (k->grid) {
             painter->setPen(k->greenThinPen);
             int midX = width/2; 
             int midY = height/2;

             painter->drawLine(midX, 0, midX, height);
             painter->drawLine(0, midY, width, midY);  

             painter->setPen(k->gridPen);
             int initX = midX - k->gridSpace;
             for (int i=initX; i > 0; i -= k->gridSpace)
                  painter->drawLine(i, 0, i, height);
             initX = midX + k->gridSpace;
             for (int i=initX; i < width; i += k->gridSpace)
                  painter->drawLine(i, 0, i, height);

             int initY = midY - k->gridSpace;
             for (int i=initY; i > 0; i -= k->gridSpace)
                  painter->drawLine(0, i, width, i);
             initY = midY + k->gridSpace;
             for (int i=initY; i < height; i += k->gridSpace)
                  painter->drawLine(0, i, width, i);
         }

         if (k->safeArea) {
             painter->setPen(k->whitePen);
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

             painter->setPen(k->grayPen);
             painter->drawRect(outerRect);

             int leftY = left.y();
             int leftX = left.x();
             int rightY = right.y();
             int rightX = right.x();

             painter->setPen(k->greenThickPen);
             painter->drawLine(QPoint(hSpace, leftY - 8), QPoint(hSpace, leftY + 8));
             painter->drawLine(QPoint(hSpace - 5, leftY), QPoint(hSpace + 5, leftY));
             painter->drawLine(QPoint(hSpace2, leftY - 8), QPoint(hSpace2, leftY + 8));
             painter->drawLine(QPoint(hSpace2 - 5, leftY), QPoint(hSpace2 + 5, leftY));

             painter->drawLine(QPoint(hSpace, rightY - 8), QPoint(hSpace, rightY + 8));
             painter->drawLine(QPoint(hSpace - 5, rightY), QPoint(hSpace + 5, rightY));
             painter->drawLine(QPoint(hSpace2, rightY - 8), QPoint(hSpace2, rightY + 8));
             painter->drawLine(QPoint(hSpace2 - 5, rightY), QPoint(hSpace2 + 5, rightY));

             painter->drawLine(QPoint(leftX - 8, vSpace), QPoint(leftX + 8, vSpace));
             painter->drawLine(QPoint(leftX, vSpace - 5), QPoint(leftX, vSpace + 5));
             painter->drawLine(QPoint(leftX - 8, vSpace2), QPoint(leftX + 8, vSpace2));
             painter->drawLine(QPoint(leftX, vSpace2 - 5), QPoint(leftX, vSpace2 + 5));

             painter->drawLine(QPoint(rightX - 8, vSpace), QPoint(rightX + 8, vSpace));
             painter->drawLine(QPoint(rightX, vSpace - 5), QPoint(rightX, vSpace + 5));
             painter->drawLine(QPoint(rightX - 8, vSpace2), QPoint(rightX + 8, vSpace2));
             painter->drawLine(QPoint(rightX, vSpace2 - 5), QPoint(rightX, vSpace2 + 5));

             painter->setPen(k->greenThinPen);

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
    QImage pic = image; 
    if (k->isScaled) {
        // pic = pic.scaledToWidth(k->displaySize.width(), Qt::SmoothTransformation);

        tError() << "TupVideoSurface::setLastImage() - Display Res: " << k->displaySize.width() << ", " << k->displaySize.height();
        tError() << "TupVideoSurface::setLastImage() - Image Res: " << pic.width() << ", " << pic.height();

        tError() << "";
        int height = pic.height();
        int width = (k->displaySize.width() * height) / k->displaySize.height();
        int posX = (pic.width() - width)/2;
        int posY = 0;
        if (width > pic.width()) {
            width = pic.width();
            height = (k->displaySize.height() * width) / k->displaySize.width();
            posX = 0;
            posY = (pic.height() - height)/2;
        }
        QImage mask = pic.copy(posX, posY, width, height);
        pic = mask.scaledToWidth(k->displaySize.width(), Qt::SmoothTransformation);
    }

    k->history << pic;
    if (k->history.count() > 5)
        k->history.removeFirst();

    calculateImageDepth();
}

void TupVideoSurface::showHistory(bool flag)
{
    k->showPrevious = flag;
    k->videoIF->updateVideo();
}

void TupVideoSurface::updateImagesOpacity(double opacity)
{
    k->opacity = (int) (255*opacity);
    k->videoIF->updateVideo();
}

void TupVideoSurface::updateImagesDepth(int depth)
{
    k->historySize = depth;
    calculateImageDepth();
    k->videoIF->updateVideo();
}

void TupVideoSurface::updateGridSpacing(int space)
{
    k->gridSpace = space;
    k->videoIF->updateVideo();
}

void TupVideoSurface::calculateImageDepth()
{
    int times = k->historySize;
    int limit = k->history.count();
    if (times > limit)
        times = limit;
    k->historyInit = limit - times;
    k->historyEnd = limit-1;
}
