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
 *   the Free Software Foundation; either version 3 of the License, or     *
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

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsRectItem>
#include <QPolygon>
#include <QApplication>
#include <QTimer>
#include <QStyleOptionGraphicsItem>
#include <QClipboard>

#include "ktpaintareabase.h"
#include "ktbrushmanager.h"
#include "ktinputdeviceinformation.h"
#include "kttextitem.h"
#include "ktpaintarearotator.h"

#include "ktgraphicsscene.h"
#include "ktgraphicalgorithm.h"

// Tupi Framework 
#include <ktscene.h>
#include "tconfig.h"
#include "tdebug.h"
#include "tapplication.h"
#include "tosd.h"

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

struct KTPaintAreaBase::Private
{
    QGraphicsRectItem *grid;
    QRectF drawingRect;
    QPointF position;
    QColor bgcolor;

    bool drawGrid;
    double angle;

    KTPaintAreaRotator *rotator;
    QStringList copiesXml;
    KTGraphicsScene *scene;
};

KTPaintAreaBase::KTPaintAreaBase(QWidget * parent) : QGraphicsView(parent), k(new Private)
{
    k->scene = new KTGraphicsScene();
    k->grid = 0;

    k->drawGrid = false;
    k->angle = 0;

    k->rotator = new KTPaintAreaRotator(this, this);
    k->drawingRect = QRectF(QPointF(0,0), QSizeF(520, 380)); // FIXME: configurable

    k->scene->setSceneRect(k->drawingRect);
    setScene(k->scene);
    centerDrawingArea();
    setUseOpenGL(false);
    setInteractive(true);
    setMouseTracking(true); 

    restoreState();
}

void KTPaintAreaBase::setBgColor(const QColor color)
{
    k->bgcolor = color;
}

void KTPaintAreaBase::saveState()
{
    TConfig *config = kApp->config("PaintArea");
    config->setValue("RenderHints", int(renderHints()));
}

void KTPaintAreaBase::restoreState()
{
    TConfig *config = kApp->config("PaintArea");

    int renderHints = config->value("RenderHints", int(this->renderHints())).toInt();
    setRenderHints(QPainter::RenderHints(renderHints));
}

KTPaintAreaBase::~KTPaintAreaBase()
{
    saveState();
    delete k;
}

void KTPaintAreaBase::setAntialiasing(bool use)
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

void KTPaintAreaBase::setUseOpenGL(bool opengl)
{
    T_FUNCINFO << opengl;

    QCursor cursor(Qt::ArrowCursor);
    if (viewport())
        cursor = viewport()->cursor();
	
#ifdef QT_OPENGL_LIB
        if (opengl) {
            setViewport(new GLDevice());
        } else {
            // setViewport(new KTImageDevice());
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

void KTPaintAreaBase::setDrawGrid(bool draw)
{
    k->drawGrid = draw;
    // resetCachedContent();
    viewport()->update();
}

void KTPaintAreaBase::setTool(KTToolPlugin *tool)
{
    if (!scene()) 
        return;

    if (tool)
        disconnect(tool,SIGNAL(requested(const KTProjectRequest *)), this, 
                   SIGNAL(requestTriggered(const KTProjectRequest *)));

    k->scene->setTool(tool);
    connect(tool,SIGNAL(requested(const KTProjectRequest *)), this, 
            SIGNAL(requestTriggered(const KTProjectRequest*)));
}

bool KTPaintAreaBase::drawGrid() const
{
    return k->drawGrid;
}

void KTPaintAreaBase::mousePressEvent(QMouseEvent * event)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (!canPaint()) { 
        #ifdef K_DEBUG
               tDebug() << "KTPaintAreaBase::mousePressEvent -> I can't paint right now!";
        #endif
        return;
    }

    k->scene->aboutToMousePress();
    QGraphicsView::mousePressEvent(event);
}

void KTPaintAreaBase::mouseMoveEvent(QMouseEvent * event)
{
    if (!canPaint()) 
        return;

    // Rotate
    if (!k->scene->isDrawing() && event->buttons() == Qt::LeftButton 
        && (event->modifiers () == (Qt::ShiftModifier | Qt::ControlModifier))) {
        setUpdatesEnabled(false);
        setDragMode(QGraphicsView::NoDrag);
	QPointF p1 = event->pos();
        QPointF p2 = k->drawingRect.center();
        k->rotator->rotateTo((int)(-(180*KTGraphicalAlgorithm::angleForPos(p1,p2))/M_PI));
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

void KTPaintAreaBase::mouseReleaseEvent(QMouseEvent *event)
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

void KTPaintAreaBase::tabletEvent(QTabletEvent *event)
{
    QGraphicsView::tabletEvent(event);
}

void KTPaintAreaBase::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QGraphicsView::enterEvent(event);
}

void KTPaintAreaBase::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QGraphicsView::leaveEvent(event);
}

void KTPaintAreaBase::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsView::drawBackground(painter, rect);
    painter->save();

    bool hasAntialiasing = painter->renderHints() & QPainter::Antialiasing;

    //painter->setRenderHint(QPainter::Antialiasing, false);

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(QColor(0,0,0,180), 2));
    // painter->fillRect(k->drawingRect, Qt::white);
    painter->fillRect(k->drawingRect, k->bgcolor);
    painter->drawRect(k->drawingRect);

    //emit changedZero(painter->matrix().map(QPointF(0,0)));
    emit changedZero(painter->worldTransform().map(QPointF(0,0)));

    // if enabled draw grid
    if (k->drawGrid) {
        // SQA: This code is not useful anymore, but is interesting
        /*
        int sx = (int)painter->matrix().m11();
        int sy = (int)painter->matrix().m22();
        painter->resetMatrix();
        painter->scale(sx, sy);
        */

        // SQA: This procedure is very heavy. Must be optimized
        //      Some kind of previously loaded buffer is required
        painter->setPen(QPen(QColor(0,0,180, 50), 1));
        int maxX = k->drawingRect.width() + 100;
        int maxY = k->drawingRect.height() + 100; 
        for (int i = -100; i <= maxX; i += 10)
             painter->drawLine(i, -100, i, maxY);
        for (int i = -100; i <= maxY; i += 10)
             painter->drawLine(-100, i, maxX, i);
    } 

    painter->setRenderHint(QPainter::Antialiasing, hasAntialiasing);
    painter->restore();
}

void KTPaintAreaBase::drawForeground(QPainter *painter, const QRectF &rect)
{
    KTScene *currentScene = k->scene->scene();

    if (!currentScene) {
        drawPadLock(painter, rect, tr("No Scene!"));
    } else {
        if (currentScene->layersTotal() > 0) {
            if (currentScene->framesTotal() > 0) {
                if (KTFrame *frame = k->scene->currentFrame()) {
                    if (frame) {
                        if (frame->isLocked())
                            drawPadLock(painter, rect, tr("Locked!"));
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

void KTPaintAreaBase::drawPadLock(QPainter *painter, const QRectF &rect, QString text)
{
    QFont kfont(QFont("Arial", 30));
    QFontMetricsF fm(kfont);

    painter->setFont(kfont);
    painter->fillRect(rect, QColor(201,201,201, 200));

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

bool KTPaintAreaBase::canPaint() const
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    if (k->scene) {
        KTFrame *frame = k->scene->currentFrame();

        if (frame) {
            #ifdef K_DEBUG
                   tFatal() << "KTPaintAreaBase::canPaint() : SORRY, THE PROBLEM IS THE FRAME";
            #endif
            return !frame->isLocked();
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "KTPaintAreaBase::canPaint() : SORRY, THERE'S NO SCENE";
        #endif
    }

    return false;
}

void KTPaintAreaBase::centerDrawingArea()
{
    centerOn(k->drawingRect.center());
}

void KTPaintAreaBase::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier)
        scaleView(pow((double)2, -event->delta() / 240.0));
    else
        QGraphicsView::wheelEvent(event);
}

bool KTPaintAreaBase::viewportEvent(QEvent *e)
{
    bool ret = QGraphicsView::viewportEvent(e);

    if (e->type() == QEvent::Show) {
        if (k->scene->items().isEmpty()) {
            k->scene->drawCurrentPhotogram();
        }
    }

    return ret;
}

void KTPaintAreaBase::scaleView(qreal scaleFactor)
{
    // SQA: Check if this method is called for some class

    qreal factor = matrix().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();

    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);

    emit scaled(scaleFactor);
}

void KTPaintAreaBase::setRotationAngle(int angle)
{
    rotate(angle - k->angle);
    k->angle = angle;
}

void KTPaintAreaBase::setZoom(qreal scaleFactor)
{
    scale(scaleFactor, scaleFactor);

    // emit scaled(scaleFactor);
}

KTBrushManager *KTPaintAreaBase::brushManager() const
{
    return k->scene->brushManager();
}

QRectF KTPaintAreaBase::drawingRect() const
{
    return k->drawingRect;
}

KTGraphicsScene *KTPaintAreaBase::graphicsScene() const
{
    return k->scene;
}

QPointF KTPaintAreaBase::viewPosition()
{
    return k->position;
}

