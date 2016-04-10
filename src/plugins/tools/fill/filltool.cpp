/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustav Gonzalez / xtingray                                           *
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

#include "filltool.h"

struct FillTool::Private
{
    QMap<QString, TAction *> actions;
    TupGraphicsScene *scene;
    QCursor insideCursor;
    QCursor contourCursor;
};

FillTool::FillTool() : k(new Private)
{
    setupActions();
}

FillTool::~FillTool()
{
}

void FillTool::init(TupGraphicsScene *scene)
{
    k->scene = scene;
}

QStringList FillTool::keys() const
{
    return QStringList() << tr("Internal Fill") << tr("Line Fill");
}

void FillTool::setupActions()
{
    TAction *action1 = new TAction(QIcon(kAppProp->themeDir() + "icons/internal_fill.png"), tr("Internal Fill"), this);
    action1->setShortcut(QKeySequence(tr("I")));
    action1->setToolTip(tr("Internal Fill") + " - " + "I");
    k->insideCursor = QCursor(kAppProp->themeDir() + "cursors/internal_fill.png", 0, 11);
    action1->setCursor(k->insideCursor);
    k->actions.insert(tr("Internal Fill"), action1);
    
    TAction *action2 = new TAction(QIcon(kAppProp->themeDir() + "icons/line_fill.png"), tr("Line Fill"), this);
    action2->setShortcut(QKeySequence(tr("B")));
    action2->setToolTip(tr("Line Fill") + " - " + "B");
    k->contourCursor = QCursor(kAppProp->themeDir() + "cursors/line_fill.png", 0, 13);
    action2->setCursor(k->contourCursor);
    k->actions.insert(tr("Line Fill"), action2);
}

void FillTool::press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[FillTool::press()]";
        #else
            T_FUNCINFOX("tools");
        #endif
    #endif

    if (input->buttons() == Qt::LeftButton) {
        // SQA: Enhance this plugin to support several items with one click 
        // QList<QGraphicsItem *> list = scene->items(input->pos(), Qt::IntersectsItemShape, Qt::DescendingOrder, QTransform());

        QGraphicsItem *item = scene->itemAt(input->pos(), QTransform());
        if (item) {
            int itemIndex = -1;
            int currentLayer;
            int currentFrame;
            TupFrame *frame;

            if (scene->spaceContext() == TupProject::FRAMES_EDITION) {
                frame = scene->currentFrame();
                itemIndex = frame->indexOf(item);
                currentLayer = scene->currentLayerIndex();
                currentFrame = scene->currentFrameIndex();
            } else {
                currentLayer = -1;
                currentFrame = -1;
                TupBackground *bg = scene->scene()->background();
                if (scene->spaceContext() == TupProject::STATIC_BACKGROUND_EDITION) {
                    frame = bg->staticFrame();
                    itemIndex = frame->indexOf(item);
                } else if (scene->spaceContext() == TupProject::DYNAMIC_BACKGROUND_EDITION) {
                           frame = bg->dynamicFrame();
                           itemIndex = frame->indexOf(item);
                }
            }

            if (itemIndex >= 0) {
                if (TupGraphicLibraryItem *libraryItem = qgraphicsitem_cast<TupGraphicLibraryItem *>(item)) {
                    // This condition only applies for images
                    if (libraryItem->type() != TupLibraryObject::Item) {
                        TOsd::self()->display(tr("Error"), tr("Sorry, only native objects can be filled"), TOsd::Error);
                        #ifdef K_DEBUG
                            QString msg = "FillTool::press() - Warning: item is a RASTER object!";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                        return;
                    }
                }

                // Testing if object is a SVG file
                TupSvgItem *svg = qgraphicsitem_cast<TupSvgItem *>(item);
                if (svg) {
                    TOsd::self()->display(tr("Error"), tr("Sorry, only native objects can be filled"), TOsd::Error);
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Warning: item is a SVG object!";
                        #ifdef Q_OS_WIN
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                    return;
                }

                if (qgraphicsitem_cast<TupItemGroup *>(item)) {
                    TOsd::self()->display(tr("Error"), tr("Sorry, Groups can't be filled yet"), TOsd::Error);
                    return;
                }

                
                if (qgraphicsitem_cast<QAbstractGraphicsShapeItem *>(item)) {
                    QDomDocument doc;
                    TupProjectRequest::Action action;
                    if (name() == tr("Internal Fill")) {
                        if (frame->brushIsNotEdited(itemIndex))
                            frame->saveInitBrush(itemIndex); 

                        QBrush brush = brushManager->brush();
                        action = TupProjectRequest::Brush;
                        doc.appendChild(TupSerializer::brush(&brush, doc));
                    } else if (name() == tr("Line Fill")) {
                               if (frame->penIsNotEdited(itemIndex))
                                   frame->saveInitPen(itemIndex);

                               QPen pen = brushManager->pen();
                               pen.setBrush(brushManager->pen().brush());
                               action = TupProjectRequest::Pen;
                               doc.appendChild(TupSerializer::pen(&pen, doc));
                    }

                    tError() << "Tracing doc:";
                    tError() << doc.toString();

                    TupProjectRequest event = TupRequestBuilder::createItemRequest(
                                              scene->currentSceneIndex(), currentLayer,
                                              currentFrame, itemIndex, QPointF(),
                                              scene->spaceContext(), TupLibraryObject::Item,
                                              action, doc.toString());

                    emit requested(&event);
                } else {
                    #ifdef K_DEBUG
                        QString msg = "FillTool::press() - Fatal Error: QAbstractGraphicsShapeItem cast has failed!";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                }
            }
        } else {
            #ifdef K_DEBUG
                QString msg = "FillTool::press() - No item found";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        }
    }
}

void FillTool::move(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

void FillTool::release(const TupInputDeviceInformation *, TupBrushManager *, TupGraphicsScene *)
{
}

QMap<QString, TAction *> FillTool::actions() const
{
    return k->actions;
}

int FillTool::toolType() const
{
    return TupToolInterface::Fill;
}
        
QWidget *FillTool::configurator()
{
    return 0;
}

void FillTool::aboutToChangeScene(TupGraphicsScene *)
{
}

void FillTool::aboutToChangeTool() 
{
    foreach (QGraphicsItem *item, k->scene->items()) {
             item->setFlag(QGraphicsItem::ItemIsSelectable, false);
             item->setFlag(QGraphicsItem::ItemIsFocusable, false);
    }
}

QPainterPath FillTool::mapPath(const QPainterPath &path, const QPointF &pos)
{
    QMatrix transform;
    transform.translate(pos.x(), pos.y());
    
    QPainterPath painter = transform.map(path);
    painter.closeSubpath();
    
    return painter;
}

QPainterPath FillTool::mapPath(const QGraphicsPathItem *item)
{
    return mapPath(item->path(), item->pos());
}

void FillTool::saveConfig()
{
}

void FillTool::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F11 || event->key() == Qt::Key_Escape) {
        emit closeHugeCanvas();
    // } else if (event->modifiers() != Qt::ShiftModifier && event->modifiers() != Qt::ControlModifier) {
    } else {
        QPair<int, int> flags = TupToolPlugin::setKeyAction(event->key(), event->modifiers());
        if (flags.first != -1 && flags.second != -1)
            emit callForPlugin(flags.first, flags.second);
    }
}

QCursor FillTool::cursor() const
{
    if (name() == tr("Internal Fill")) {
        return k->insideCursor;
    } else if (name() == tr("Line Fill")) {
               return k->contourCursor;
    }

    return QCursor(Qt::ArrowCursor);
}
