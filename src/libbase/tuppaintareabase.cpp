/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "tuppaintareabase.h"
#include "tupbrushmanager.h"
#include "tupinputdeviceinformation.h"
#include "tuptextitem.h"
#include "tuppaintarearotator.h"
#include "tupgraphicsscene.h"
#include "tupgraphicalgorithm.h"

// Tupi Framework 
#include "tupscene.h"
#include "tconfig.h"
#include "tdebug.h"
#include "tapplication.h"
#include "tosd.h"

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPolygon>
#include <QApplication>
#include <QTimer>
#include <QStyleOptionGraphicsItem>
#include <QClipboard>

#include <cmath>

#ifdef QT_OPENGL_LIB

#include <QGLWidget>
#include <QGLFramebufferObject>

/**
 * This class defines the global paint area behavoir in the Ilustration Mode.
 * Here is where general events about the paint area are processed.
 * @author David Cuadrado & Jorge Cuadrado
*/

class GLDevice : public QGLWidget
{
    public:
        GLDevice() : QGLWidget()
                {
                     makeCurrent();
                }
        ~GLDevice() {};

    protected:
        void initializeGL()
                {
                     glDisable(GL_DEPTH_TEST);
                }
};

#endif

struct TupPaintAreaBase::Private
{
    QGraphicsRectItem *grid;
    QRectF drawingRect;
    QPointF position;
    QColor bgcolor;

    bool gridFlag;
    bool actionSafeAreaFlag;
    double angle;

    TupPaintAreaRotator *rotator;
    QStringList copiesXml;
    TupGraphicsScene *scene;

    QPen greenThickPen;
    QPen gridPen;
    QPen grayPen;
    QPen greenBoldPen;
    QPen greenThinPen;
    QPen blackPen;
};

TupPaintAreaBase::TupPaintAreaBase(QWidget *parent, QSize dimension) : QGraphicsView(parent), k(new Private)
{
    k->scene = new TupGraphicsScene();
    k->grid = 0;

    k->greenThickPen = QPen(QColor(0, 135, 0, 255), 2);
    k->gridPen = QPen(QColor(0, 0, 180, 50), 1);
    k->grayPen = QPen(QColor(150, 150, 150, 255), 1);
    k->greenBoldPen = QPen(QColor(0, 135, 0, 255), 3);
    k->greenThinPen = QPen(QColor(0, 135, 0, 255), 1);
    k->blackPen = QPen(QColor(0, 0, 0, 180), 2);

    k->gridFlag = false;
    k->actionSafeAreaFlag = false;
    k->angle = 0;

    k->rotator = new TupPaintAreaRotator(this, this);
    k->drawingRect = QRectF(QPointF(0, 0), dimension);

    k->scene->setSceneRect(k->drawingRect);
    setScene(k->scene);
    centerDrawingArea();
    setUseOpenGL(false);
    setInteractive(true);
    setMouseTracking(true); 

    restoreState();
}

void TupPaintAreaBase::setBgColor(const QColor color)
{
    k->bgcolor = color;
}

void TupPaintAreaBase::saveState()
{
    TConfig *config = kApp->config("PaintArea");
    config->setValue("RenderHints", int(renderHints()));
}

void TupPaintAreaBase::restoreState()
{
    TConfig *config = kApp->config("PaintArea");

    int renderHints = config->value("RenderHints", int(this->renderHints())).toInt();
    setRenderHints(QPainter::RenderHints(renderHints));
}

TupPaintAreaBase::~TupPaintAreaBase()
{
    saveState();
    delete k;
}

void TupPaintAreaBase::setAntialiasing(bool use)
{
#ifdef QT_OPENGL_LIB
    if (QGLWidget *gl = dynamic_cast<QGLWidget *>(viewport())) {
        gl->setUpdatesEnabled(false); // works better
        // gl->setFormat(QGLFormat(QGL::SampleBuffers | QGL::HasOverlay /*| QGL::DirectRendering 
        // | QGL::AccumBuffer | QGL::Rgba */));
        gl->setFormat(QGLFormat(QGL::SampleBuffers | QGL::HasOverlay));
        gl->setUpdatesEnabled(true);
    }
#endif

    setRenderHint(QPainter::Antialiasing, use);
    setRenderHint(QPainter::TextAntialiasing, use);
}

void TupPaintAreaBase::setUseOpenGL(bool opengl)
{
    T_FUNCINFO << opengl;

    QCursor cursor(Qt::ArrowCursor);
    if (viewport())
        cursor = viewport()->cursor();
	
#ifdef QT_OPENGL_LIB
        if (opengl) {
            setViewport(new GLDevice());
        } else {
            // setViewport(new TupImageDevice());
        }
#else
        Q_UNUSED(opengl);
        kWarning() << tr("OpenGL isn't supported");
#endif

    // to restore the cursor.
    if (viewport()) {
        viewport()->setCursor(cursor);
        viewport()->setAcceptDrops(true);
    }
}

void TupPaintAreaBase::drawGrid(bool draw)
{
    k->gridFlag = draw;
    viewport()->update();
}

void TupPaintAreaBase::drawActionSafeArea(bool draw)
{
    k->actionSafeAreaFlag = draw;
    viewport()->update();
}

void TupPaintAreaBase::setTool(TupToolPlugin *tool)
{
    if (!scene()) {
        #ifdef K_DEBUG
               tDebug() << "TupPaintAreaBase::setTool() - Fatal Error: No scene available";
        #endif
        return;
    }

    if (tool)
        disconnect(tool, SIGNAL(requested(const TupProjectRequest *)), 
                   this, SIGNAL(requestTriggered(const TupProjectRequest *)));

    k->scene->setTool(tool);
    connect(tool, SIGNAL(requested(const TupProjectRequest *)), 
            this, SIGNAL(requestTriggered(const TupProjectRequest*)));
}

bool TupPaintAreaBase::gridFlag() const
{
    return k->gridFlag;
}

bool TupPaintAreaBase::actionSafeAreaFlag() const
{
    return k->actionSafeAreaFlag;
}

void TupPaintAreaBase::mousePressEvent(QMouseEvent * event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!canPaint()) { 
        #ifdef K_DEBUG
               tDebug() << "TupPaintAreaBase::mousePressEvent -> I can't paint right now!";
        #endif
        return;
    }

    k->scene->aboutToMousePress();
    QGraphicsView::mousePressEvent(event);
}

void TupPaintAreaBase::mouseMoveEvent(QMouseEvent * event)
{
    if (!canPaint()) { 
        #ifdef K_DEBUG
               tWarning() << "TupPaintAreaBase::mouseMoveEvent() - The canvas is busy. Can't paint!";
        #endif
        return;
    }

    // Rotate WorkSpace
    if (!k->scene->isDrawing() && event->buttons() == Qt::LeftButton 
        && (event->modifiers () == (Qt::ShiftModifier | Qt::ControlModifier))) {
        setUpdatesEnabled(false);
        setDragMode(QGraphicsView::NoDrag);

	QPointF p1 = event->pos();
        QPointF p2 = k->drawingRect.center();
        int angle = (int)(-(180 * TupGraphicalAlgorithm::angleForPos(p1, p2)) / M_PI);
        k->rotator->rotateTo(angle);
        emit rotated(-angle);
        setUpdatesEnabled(true);
    } else {
        QGraphicsView::mouseMoveEvent(event);

        if (!k->scene->mouseGrabberItem() && k->scene->isDrawing()) { // HACK
            QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
            mouseEvent.setWidget(viewport());
            mouseEvent.setScenePos(mapToScene(event->pos()));
            mouseEvent.setScreenPos(event->globalPos());
            mouseEvent.setButtons(event->buttons());
            mouseEvent.setButton(event->button());
            mouseEvent.setModifiers(event->modifiers());
            mouseEvent.setAccepted(false);
            // QApplication::sendEvent(k->scene, &mouseEvent);
            k->scene->mouseMoved(&mouseEvent);
        }
    }

    k->position = mapToScene(event->pos()); 
    emit cursorPosition(k->position);
}

void TupPaintAreaBase::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);

    if (! k->scene->mouseGrabberItem() && k->scene->isDrawing()) { // HACK
        QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
        mouseEvent.setWidget(viewport());
        mouseEvent.setScenePos(mapToScene(event->pos()));
        mouseEvent.setScreenPos(event->globalPos());
        mouseEvent.setButtons(event->buttons());
        mouseEvent.setButton(event->button());
        mouseEvent.setModifiers(event->modifiers());
        mouseEvent.setAccepted(false);
        // QApplication::sendEvent(k->scene, &mouseEvent);
        k->scene->mouseReleased(&mouseEvent);
    }
}

void TupPaintAreaBase::tabletEvent(QTabletEvent *event)
{
    QGraphicsView::tabletEvent(event);
}

void TupPaintAreaBase::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QGraphicsView::enterEvent(event);
}

void TupPaintAreaBase::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QGraphicsView::leaveEvent(event);
}

void TupPaintAreaBase::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->save();

    bool hasAntialiasing = painter->renderHints() & QPainter::Antialiasing;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(k->blackPen);
    painter->fillRect(k->drawingRect, k->bgcolor);
    painter->drawRect(k->drawingRect);

    emit changedZero(painter->worldTransform().map(QPointF(0, 0)));

    painter->setRenderHint(QPainter::Antialiasing, hasAntialiasing);
    painter->restore();
}

void TupPaintAreaBase::drawForeground(QPainter *painter, const QRectF &rect)
{
    TupScene *currentScene = k->scene->scene();

    if (!currentScene) {
        drawPadLock(painter, rect, tr("No Scene!"));
    } else {
        if (currentScene->layersTotal() > 0) {
            if (currentScene->framesTotal() > 0) {
                if (TupFrame *frame = k->scene->currentFrame()) {
                    if (frame) {
                        if (frame->isLocked()) {
                            drawPadLock(painter, rect, tr("Locked!"));
                        } else {
                            // if enabled draw grid
                            if (k->gridFlag) {
                                painter->setPen(k->gridPen);
                                int maxX = k->drawingRect.width() + 100;
                                int maxY = k->drawingRect.height() + 100;
                                for (int i = -100; i <= maxX; i += 10)
                                     painter->drawLine(i, -100, i, maxY);
                                for (int i = -100; i <= maxY; i += 10)
                                     painter->drawLine(-100, i, maxX, i);
                            }
                            // if enabled action safe area
                            if (k->actionSafeAreaFlag) {
                                painter->setPen(k->greenThickPen);
                                painter->drawRect(k->drawingRect);

                                int w = k->drawingRect.width();
                                int outerBorder = w/19;
                                int innerBorder = w/6;

                                int hSpace = w/3;
                                int vSpace = k->drawingRect.height()/3;

                                QPointF left = k->drawingRect.topLeft() + QPointF(outerBorder, outerBorder); 
                                QPointF right = k->drawingRect.bottomRight() - QPointF(outerBorder, outerBorder);
                                int leftX = left.x();
                                int leftY = left.y();
                                int rightX = right.x();
                                int rightY = right.y();

                                QRectF outerRect(left, right);

                                painter->setPen(k->grayPen);
                                painter->drawRect(outerRect);

                                painter->setPen(k->greenBoldPen);
                                painter->drawLine(QPoint(hSpace, leftY - 8), QPoint(hSpace, leftY + 8));
                                painter->drawLine(QPoint(hSpace - 5, leftY), QPoint(hSpace + 5, leftY));
                                painter->drawLine(QPoint(hSpace*2, leftY - 8), QPoint(hSpace*2, leftY + 8));
                                painter->drawLine(QPoint(hSpace*2 - 5, leftY), QPoint(hSpace*2 + 5, leftY));

                                painter->drawLine(QPoint(hSpace, rightY - 8), QPoint(hSpace, rightY + 8));
                                painter->drawLine(QPoint(hSpace - 5, rightY), QPoint(hSpace + 5, rightY));
                                painter->drawLine(QPoint(hSpace*2, rightY - 8), QPoint(hSpace*2, rightY + 8));
                                painter->drawLine(QPoint(hSpace*2 - 5, rightY), QPoint(hSpace*2 + 5, rightY));

                                painter->drawLine(QPoint(leftX - 8, vSpace), QPoint(leftX + 8, vSpace));
                                painter->drawLine(QPoint(leftX, vSpace - 5), QPoint(leftX, vSpace + 5));
                                painter->drawLine(QPoint(leftX - 8, vSpace*2), QPoint(leftX + 8, vSpace*2));
                                painter->drawLine(QPoint(leftX, vSpace*2 - 5), QPoint(leftX, vSpace*2 + 5));

                                painter->drawLine(QPoint(rightX - 8, vSpace), QPoint(rightX + 8, vSpace));
                                painter->drawLine(QPoint(rightX, vSpace - 5), QPoint(rightX, vSpace + 5));
                                painter->drawLine(QPoint(rightX - 8, vSpace*2), QPoint(rightX + 8, vSpace*2));
                                painter->drawLine(QPoint(rightX, vSpace*2 - 5), QPoint(rightX, vSpace*2 + 5));

                                painter->setPen(k->greenThinPen);

                                left = k->drawingRect.topLeft() + QPointF(innerBorder, innerBorder);
                                right = k->drawingRect.bottomRight() - QPointF(innerBorder, innerBorder);
                                QRectF innerRect(left, right);

                                painter->drawRect(innerRect); 
                            }
                        }
                    } 
                }
            } else {
                drawPadLock(painter, rect, tr("No Frames!"));
            } 
        } else {
            drawPadLock(painter, rect, tr("No Layers!"));
        }
    }
}

void TupPaintAreaBase::drawPadLock(QPainter *painter, const QRectF &rect, QString text)
{
    QFont kfont(QFont("Arial", 30));
    QFontMetricsF fm(kfont);

    painter->setFont(kfont);
    painter->fillRect(rect, QColor(201, 201, 201, 200));

    QRectF shore = fm.boundingRect(text);

    int middleX = k->scene->sceneRect().topRight().x() - k->scene->sceneRect().topLeft().x();
    int middleY = k->scene->sceneRect().bottomLeft().y() - k->scene->sceneRect().topLeft().y();

    int x = (middleX - shore.width()) / 2;
    int y = (middleY - shore.height()) / 2;
    painter->drawText(x, y, text);

    x = (middleX - 20) / 2;
    y = (middleY - 20) / 2;
    painter->setPen(QPen(QColor(100, 100, 100), 4, Qt::SolidLine));
    painter->drawRoundedRect(x, y + 18, 20, 20, 1, 1, Qt::AbsoluteSize);

    x = (middleX - 30) / 2;
    painter->fillRect(x, y + 30, 30, 20, QColor(100, 100, 100));
}

bool TupPaintAreaBase::canPaint() const
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->scene) {
        TupFrame *frame = k->scene->currentFrame();
        if (frame) {
            return !frame->isLocked();
        }
    } else {
        #ifdef K_DEBUG
               tWarning() << "TupPaintAreaBase::canPaint() - Warning: Scene is NULL!";
        #endif
    }

    return false;
}

void TupPaintAreaBase::centerDrawingArea()
{
    centerOn(k->drawingRect.center());
}

QPointF TupPaintAreaBase::centerPoint() const
{
    return k->drawingRect.center();
}

void TupPaintAreaBase::wheelEvent(QWheelEvent *event)
{
    scaleView(pow((double)2, event->delta() / 520.0));

    /*
    if (event->modifiers() == Qt::ControlModifier)
        scaleView(pow((double)2, -event->delta() / 240.0));
    else
        QGraphicsView::wheelEvent(event);
    */
}

bool TupPaintAreaBase::viewportEvent(QEvent *event)
{
    bool flag = QGraphicsView::viewportEvent(event);

    if (event->type() == QEvent::Show) {
        if (k->scene->items().isEmpty())
            k->scene->drawCurrentPhotogram();
    }

    return flag;
}

void TupPaintAreaBase::scaleView(qreal scaleFactor)
{
    // SQA: Check if this method is called for some class

    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();

    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);

    emit scaled(scaleFactor);
}

void TupPaintAreaBase::setRotationAngle(int angle)
{
    int degrees = angle - k->angle;
    rotate(degrees);
    k->angle = angle;
}

void TupPaintAreaBase::setZoom(qreal scaleFactor)
{
    scale(scaleFactor, scaleFactor);
}

TupBrushManager *TupPaintAreaBase::brushManager() const
{
    return k->scene->brushManager();
}

QRectF TupPaintAreaBase::drawingRect() const
{
    return k->drawingRect;
}

TupGraphicsScene *TupPaintAreaBase::graphicsScene() const
{
    return k->scene;
}

QPointF TupPaintAreaBase::viewPosition()
{
    return k->position;
}

void TupPaintAreaBase::updateDimension(const QSize dimension)
{
    k->drawingRect = QRectF(QPointF(0, 0), dimension);
    k->scene->setSceneRect(k->drawingRect);

    update();
}
