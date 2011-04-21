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

#include "ktapplication.h"
#include "ktviewdocument.h"

// Tupi Framework 
#include "kdebug.h"
#include "kconfig.h"

#include <QLayout>
#include <QStatusBar>
#include <QMenuBar>
#include <QPixmap>
#include <QActionGroup>
#include <QDockWidget>
#include <QTimer>
#include <QApplication>
#include <QCursor>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QMenu>
#include <QDir>
#include <QPluginLoader>
#include <QSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QComboBox>

#include "ktpaintareaproperties.h"
#include "ktpluginmanager.h"
#include "ktpaintarea.h"
#include "ktprojectresponse.h"
#include "ktpaintareaevent.h"
#include "ktpaintareacommand.h"
#include "ktgraphicsscene.h"

#include "ktbrushmanager.h"
#include "ktproject.h"
#include "ktpaintareastatus.h"

/**
 * This class defines all the environment for the Ilustration interface.
 * Here is where all the tools for Ilustration are defined.
 * @author David Cuadrado
*/

struct KTViewDocument::Private
{
    QActionGroup *gridGroup, *editGroup, *viewNextGroup, *viewZoomGroup, *viewPreviousGroup;
    QMenu *brushesMenu, *motionMenu, *selectionMenu, *fillMenu, *filterMenu, *viewToolMenu;
    QMenu *toolsMenu, *editMenu, *viewMenu, *orderMenu;
    QAction *aUndo, *aRedo, *aClose;
    QToolBar *barGrid, *toolbar;
    QDoubleSpinBox *zoomFactorSpin;
    QDoubleSpinBox *onionFactorSpin;

    QSpinBox *prevOnionSkinSpin;
    QSpinBox *nextOnionSkinSpin;
    bool onionEnabled;
    int prevOnionValue;
    int nextOnionValue;
    double opacityFactor;

    KTPaintArea *paintArea;

    KTDocumentRuler *verticalRuler, *horizontalRuler;
    KActionManager *actionManager;
    KTConfigurationArea *configurationArea;
    KTToolPlugin *currentTool;
    KTPaintAreaStatus *status;
    QComboBox *spaceMode;

    KTProject *project;
    QTimer *timer;
};

KTViewDocument::KTViewDocument(KTProject *project, QWidget *parent) : QMainWindow(parent), k(new Private)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    setWindowIcon(QPixmap(THEME_DIR + "icons/animation_mode.png"));

    k->project = project;
    k->currentTool = 0;
    k->onionEnabled = true;
    k->actionManager = new KActionManager(this);

    QFrame *frame = new QFrame(this, Qt::FramelessWindowHint);
    QGridLayout *layout = new QGridLayout(frame);

    k->paintArea = new KTPaintArea(project, frame);
    connect(k->paintArea, SIGNAL(scaled(double)), this, SLOT(scaleRuler(double)));

    KCONFIG->beginGroup("OnionParameters");
    k->opacityFactor = KCONFIG->value("OnionFactor", -1).toDouble();
    if (k->opacityFactor < 0)
        k->opacityFactor = 0.5;
    k->paintArea->setOnionFactor(k->opacityFactor);

    setCentralWidget(frame);

    layout->addWidget(k->paintArea, 1, 1);
    k->horizontalRuler = new KTDocumentRuler(Qt::Horizontal);
    k->verticalRuler = new KTDocumentRuler(Qt::Vertical);

    layout->addWidget(k->horizontalRuler, 0, 1);
    layout->addWidget(k->verticalRuler, 1, 0);

    Tupi::RenderType renderType = Tupi::RenderType(KCONFIG->value("RenderType").toInt()); 

    switch (renderType) {
            case Tupi::OpenGL:
                 k->paintArea->setUseOpenGL(true);
                 break;

            case Tupi::Native:
                 k->paintArea->setUseOpenGL(false);
                 break;

            default:
                 #ifdef K_DEBUG
                        kWarning() << "Unsopported render, switching to native!";
                 #endif
                 k->paintArea->setUseOpenGL(false);
            break;
    }
    
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), this, SLOT(showPos(const QPointF &)));
    
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->verticalRuler, 
                          SLOT(movePointers(const QPointF&)));
    
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->horizontalRuler, 
                          SLOT(movePointers(const QPointF&)));

    connect(k->paintArea, SIGNAL(changedZero(const QPointF&)), this, SLOT(changeRulerOrigin(const QPointF&)));

    connect(k->paintArea, SIGNAL(requestTriggered(const KTProjectRequest *)), this, 
                          SIGNAL(requestTriggered(const KTProjectRequest *)));

    setupDrawActions();

    //k->configurationArea = new KTConfigurationArea(this);
    //addDockWidget(Qt::RightDockWidgetArea, k->configurationArea);
    //k->configurationArea->close();
    
    createToolBar();
    createTools();
    
    k->status = new KTPaintAreaStatus(this);
    setStatusBar(k->status);
    connect(k->status, SIGNAL(colorRequested()), this, SIGNAL(expandColorPanel()));
    connect(k->status, SIGNAL(colorUpdated(const QColor)), this, SLOT(updateBgColor(const QColor)));
    connect(k->paintArea, SIGNAL(frameChanged(int)), k->status, SLOT(updateFrameIndex(int)));

    // SQA: Verify if this code is doing something
    //connect(k->paintArea->brushManager(), SIGNAL(brushChanged(const QBrush&)), k->status, 
    //        SLOT(setBrush(const QBrush &)));

    connect(k->paintArea->brushManager(), SIGNAL(penChanged(const QPen&)), k->status, 
            SLOT(setPen(const QPen &)));

    QTimer::singleShot(1000, this, SLOT(loadPlugins()));

    updateTimer();
}

KTViewDocument::~KTViewDocument()
{
    if (k->currentTool)
        k->currentTool->saveConfig();

    delete k->configurationArea;
    delete k->timer;
    delete k;
}

void KTViewDocument::setAntialiasing(bool useIt)
{
    k->paintArea->setAntialiasing(useIt);
}

void KTViewDocument::setOpenGL(bool useIt)
{
    k->paintArea->setUseOpenGL(useIt);
}

void KTViewDocument::setDrawGrid(bool draw)
{
    k->paintArea->setDrawGrid(draw);
}

QPainter::RenderHints KTViewDocument::renderHints() const
{
    return k->paintArea->renderHints();
}

void KTViewDocument::setRotationAngle(int angle)
{
    k->paintArea->setRotationAngle(angle);
}

void KTViewDocument::setZoom(qreal factor)
{
    k->paintArea->setZoom(factor);
}

void KTViewDocument::showPos(const QPointF &p)
{
    QString message =  "X: " +  QString::number(p.x()) + " Y: " + QString::number(p.y());
    emit sendToStatus(message) ;
}

void KTViewDocument::setupDrawActions()
{
    KAction *showGrid = new KAction(QPixmap(THEME_DIR + "icons/subgrid.png"), 
                                    tr("Show grid"), QKeySequence(tr("#")),
                                    this, SLOT(toggleShowGrid()), k->actionManager, "show_grid");
    showGrid->setCheckable(true);

    KAction *copy = new KAction(QPixmap(THEME_DIR + "icons/copy.png"), 
                                tr("Copy"), QKeySequence(tr("Ctrl+C")),
                                k->paintArea, SLOT(copyItems()), k->actionManager, "copy");
    copy->setStatusTip(tr("Copies the selection and puts it onto the clipboard"));

    KAction *paste = new KAction(QPixmap(THEME_DIR + "icons/paste.png"), 
                                 tr("Paste"), QKeySequence(tr("Ctrl+V")),
                                 k->paintArea, SLOT(pasteItems()), k->actionManager, "paste");
    paste->setStatusTip(tr("Pastes the clipboard into the current document"));

    KAction *cut = new KAction(QPixmap(THEME_DIR + "icons/cut.png"), 
                               tr("Cut"), QKeySequence(tr("Ctrl+X")),
                               k->paintArea, SLOT(cutItems()),k->actionManager, "cut");
    cut->setStatusTip(tr("Cuts the selected items"));

    KAction *del = new KAction(QPixmap(THEME_DIR + "icons/delete.png"), tr("Delete"), 
                               QKeySequence(Qt::Key_Delete), k->paintArea, SLOT(deleteItems()), 
                               k->actionManager, "delete");
    
    del->setStatusTip(tr("Deletes the selected object"));

    KAction *group = new KAction(QPixmap(THEME_DIR + "icons/group.png"), tr("&Group"),   
                                 QKeySequence(tr("Ctrl+G")), k->paintArea, SLOT(groupItems()), 
                                 k->actionManager, "group");

    group->setStatusTip(tr("Group the selected objects into a single one"));
    group->setDisabled(true);

    KAction *ungroup = new KAction(QPixmap(THEME_DIR + "icons/ungroup.png"), tr("&Ungroup"), 
                                    QKeySequence(tr("Ctrl+Shift+G")) , k->paintArea, SLOT(ungroupItems()), 
                                    k->actionManager, "ungroup");
    ungroup->setDisabled(true);

    ungroup->setStatusTip(tr("Ungroups the selected object"));

    KAction *onion = new KAction(QPixmap(THEME_DIR + "icons/layer.png"), tr("Onion Skin"),
                               QKeySequence(tr("Ctrl+Shift+O")), this, SLOT(enableOnionFeature()),
                               k->actionManager, "onion");

    onion->setStatusTip(tr("Enable/Disable onion skin"));

    KAction *onionFactor = new KAction(QPixmap(THEME_DIR + "icons/onion.png"), tr("Onion Skin Factor"),
                               QKeySequence(tr("Ctrl+Shift+S")), this, SLOT(setDefaultOnionFactor()),
                               k->actionManager, "onionfactor");

    onionFactor->setStatusTip(tr("Set onion skin factor default value"));
}

void KTViewDocument::createTools()
{
    k->toolbar = new QToolBar(tr("Draw tools"), this);
    k->toolbar->setIconSize(QSize(16,16));
    addToolBar(Qt::LeftToolBarArea, k->toolbar);

    connect(k->toolbar, SIGNAL(actionTriggered(QAction *)), this, SLOT(selectToolFromMenu(QAction *)));

    // Brushes menu
    k->brushesMenu = new QMenu(tr("Brushes"), k->toolbar);
    k->brushesMenu->setIcon(QPixmap(THEME_DIR + "icons/brush.png"));
    connect(k->brushesMenu, SIGNAL(triggered (QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->brushesMenu->menuAction());

    // Motion menu
    k->motionMenu = new QMenu(tr("Tweening"), k->toolbar);
    k->motionMenu->setIcon(QPixmap(THEME_DIR + "icons/tweening.png"));
    connect(k->motionMenu, SIGNAL(triggered (QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->motionMenu->menuAction());

    // Selection menu
    k->selectionMenu = new QMenu(tr("Selection"), k->toolbar);
    k->selectionMenu->setIcon(QPixmap(THEME_DIR + "icons/selection.png"));
    connect(k->selectionMenu, SIGNAL(triggered(QAction*)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->selectionMenu->menuAction());

    // Fill menu
    k->fillMenu = new QMenu(tr("Fill"), k->toolbar);
    k->fillMenu->setIcon(QPixmap(THEME_DIR + "icons/fillcolor.png"));
    connect(k->fillMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->fillMenu->menuAction());

    // View menu
    k->viewToolMenu = new QMenu(tr("View"), k->toolbar);
    k->viewToolMenu->setIcon(QPixmap(THEME_DIR + "icons/magnifying.png"));
    connect(k->fillMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->viewToolMenu->menuAction());
}

void KTViewDocument::loadPlugins()
{
    QVector<KAction*> brushTools(7);
    QVector<KAction*> tweenTools(7);

    foreach (QObject *plugin, KTPluginManager::instance()->tools()) {

             KTToolPlugin *tool = qobject_cast<KTToolPlugin *>(plugin);

             QStringList::iterator it;
             QStringList keys = tool->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                         kDebug("plugins") << "*** Tool Loaded: " << *it;
                  #endif

                  KAction *action = tool->actions()[*it];
                  action->setIconVisibleInMenu(true);

                  if (action) {
                      connect(action, SIGNAL(triggered()), this, SLOT(selectTool()));
                      action->setParent(plugin);
                      QString toolName = action->text();

                      switch (tool->toolType()) {
                              case KTToolInterface::Brush:
                                 {
                                   if (toolName.compare(tr("Pencil")) == 0) {
                                       brushTools[0] = action;
                                       k->brushesMenu->setDefaultAction(action);
                                   }

                                   if (toolName.compare(tr("Eraser")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[1] = action;
                                   }

                                   if (toolName.compare(tr("PolyLine")) == 0)
                                       brushTools[2] = action;

                                   if (toolName.compare(tr("Line")) == 0)
                                       brushTools[3] = action;

                                   if (toolName.compare(tr("Rectangle")) == 0)
                                       brushTools[4] = action;

                                   if (toolName.compare(tr("Ellipse")) == 0)
                                       brushTools[5] = action;

                                   if (toolName.compare(tr("Text")) == 0)
                                       brushTools[6] = action;
                                 }
                                 break;
                              case KTToolInterface::Tweener:
                                 {
                                   if (toolName.compare(tr("Position Tween")) == 0) {
                                       tweenTools[0] = action;
                                       k->motionMenu->setDefaultAction(action);
                                   }

                                   if (toolName.compare(tr("Rotation Tween")) == 0)
                                       // action->setDisabled(true);
                                       tweenTools[1] = action;

                                   if (toolName.compare(tr("Scale Tween")) == 0) {
                                       // action->setDisabled(true);
                                       tweenTools[2] = action;
                                   }

                                   if (toolName.compare(tr("Shear Tween")) == 0) {
                                       // action->setDisabled(true);
                                       tweenTools[3] = action;
                                   }

                                   if (toolName.compare(tr("Opacity Tween")) == 0) {
                                       // action->setDisabled(true);
                                       tweenTools[4] = action;
                                   }

                                   if (toolName.compare(tr("Coloring Tween")) == 0) {
                                       // action->setDisabled(true);
                                       tweenTools[5] = action;
                                   }

                                   if (toolName.compare(tr("Compound Tween")) == 0) {
                                       action->setDisabled(true);
                                       tweenTools[6] = action;
                                   }
                                 }
                                 break;
                              case KTToolInterface::Selection:
                                 {
                                   k->selectionMenu->addAction(action);
                                   if (toolName.compare(tr("Object Selection")) == 0)
                                       k->selectionMenu->setDefaultAction(action);
                                 }
                                 break;
                              case KTToolInterface::Fill:
                                 {
                                   k->fillMenu->addAction(action);
                                   if (toolName.compare(tr("Internal fill")) == 0)
                                      k->fillMenu->setDefaultAction(action);
                                 }
                                 break;
                               case KTToolInterface::View:
                                 {
                                   k->viewToolMenu->addAction(action);
                                   if (toolName.compare(tr("Zoom")) == 0)
                                       k->viewToolMenu->setDefaultAction(action);

                                 }
                                 break;
                               default:
                                 break;
                      }
                  }
             }
    } // end foreach

    for (int i = 0; i < brushTools.size(); ++i)
         k->brushesMenu->addAction(brushTools.at(i));

    for (int i = 0; i < tweenTools.size(); ++i)
         k->motionMenu->addAction(tweenTools.at(i));

    foreach (QObject *plugin, KTPluginManager::instance()->filters()) {
             AFilterInterface *filter = qobject_cast<AFilterInterface *>(plugin);
             QStringList::iterator it;
             QStringList keys = filter->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                         kDebug("plugins") << "*** Filter Loaded: " << *it;
                  #endif

                  KAction *act = filter->actions()[*it];
                  if (act) {
                      connect(act, SIGNAL(triggered()), this, SLOT(applyFilter()));
                      k->filterMenu->addAction(act);
                  }
             }
    }

    KAction *pencil = brushTools.at(0);
    pencil->trigger();
    brushTools.clear();
    tweenTools.clear();
}

void KTViewDocument::selectTool()
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    KAction *action = qobject_cast<KAction *>(sender());

    if (action) {

        QString toolName = tr("%1").arg(action->text());

        if (k->currentTool) {
            if (toolName.compare(k->currentTool->name()) == 0)
                return;

            k->currentTool->saveConfig();
            QWidget *toolConfigurator = k->currentTool->configurator();
            if (toolConfigurator)
                k->configurationArea->close();
        }

        KTToolPlugin *tool = qobject_cast<KTToolPlugin *>(action->parent());
        k->currentTool = tool; 
        k->paintArea->setCurrentTool(toolName);

        if (!action->icon().isNull())
            k->status->updateTool(toolName, action->icon().pixmap(15, 15));

        int minWidth = 0;

        switch (tool->toolType()) {

                case KTToolInterface::Brush: 
                     if (toolName.compare(tr("Pencil"))==0) {
                         minWidth = 130;
                     } else if (toolName.compare(tr("Text"))==0) {
                                minWidth = 350;
                     } else if (toolName.compare(tr("PolyLine"))==0) {
                                minWidth = 130;
                     } 

                     k->brushesMenu->setDefaultAction(action);
                     k->brushesMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->brushesMenu->menuAction()->setIcon(action->icon());
                     break;

                case KTToolInterface::Tweener:
                     minWidth = 220;
                     k->motionMenu->setDefaultAction(action);
                     k->motionMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->motionMenu->menuAction()->setIcon(action->icon());
                     break;

                case KTToolInterface::Fill:
                     k->fillMenu->setDefaultAction(action);
                     k->fillMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->fillMenu->menuAction()->setIcon(action->icon());
                     break;

                case KTToolInterface::Selection:
                     k->selectionMenu->setDefaultAction(action);
                     k->selectionMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->selectionMenu->menuAction()->setIcon(action->icon());
                     if (toolName.compare(tr("Object Selection"))==0) {
                         minWidth = 130;
                         connect(k->paintArea, SIGNAL(itemAddedOnSelection(KTGraphicsScene *)), 
                                 tool, SLOT(updateItems(KTGraphicsScene *)));
                     } 
                     break;

                case KTToolInterface::View:
                     k->viewToolMenu->setDefaultAction(action);
                     k->viewToolMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->viewToolMenu->menuAction()->setIcon(action->icon());
                     if (toolName.compare(tr("Zoom"))==0)
                         minWidth = 130;
                     break;
        }

        QWidget *toolConfigurator = tool->configurator();

        if (toolConfigurator) {
            k->configurationArea = new KTConfigurationArea(this);
            k->configurationArea->setConfigurator(toolConfigurator, minWidth);
            addDockWidget(Qt::RightDockWidgetArea, k->configurationArea);
            toolConfigurator->show();
            if (!k->configurationArea->isVisible())
                k->configurationArea->show();
        } else {
            if (k->configurationArea->isVisible())
                k->configurationArea->close();
        }

        tool->setName(toolName);
        k->paintArea->setTool(tool);

        k->paintArea->viewport()->setCursor(action->cursor());

        if ((tool->toolType() == KTToolInterface::Tweener) && (k->spaceMode->currentIndex() != 0))
            k->spaceMode->setCurrentIndex(0);
    }
}

void KTViewDocument::selectToolFromMenu(QAction *action)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    QMenu *menu = qobject_cast<QMenu *>(action->parent());

    if (menu) {
        KAction *tool = qobject_cast<KAction *>(menu->activeAction());

        if (tool) {
            tool->trigger(); // this call selectTool()
        } else {
            tool = qobject_cast<KAction *>(menu->defaultAction());
            if (tool)
                tool->trigger();
        }
    } 
}

bool KTViewDocument::handleProjectResponse(KTProjectResponse *event)
{
    return k->paintArea->handleResponse(event);
}

void KTViewDocument::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        /*
        SQA issue: Code to check
        AFilterInterface *aFilter = qobject_cast<AFilterInterface *>(action->parent());
        QString filter = action->text();
        
        KTFrame *frame = k->paintArea->currentFrame();
        if (frame) {
            aFilter->filter(action->text(), frame->components());
            k->paintArea->redrawAll();
        }
        */
    }
}

void KTViewDocument::updateZoomFactor(double f)
{
    k->zoomFactorSpin->blockSignals(true);
    k->zoomFactorSpin->setValue(f*100);
    k->zoomFactorSpin->blockSignals(false);
}

void KTViewDocument::createToolBar()
{
    k->barGrid = new QToolBar(tr("Paint area actions"), this);
    k->barGrid->setIconSize(QSize(16,16));

    addToolBar(k->barGrid);

    k->barGrid->addAction(kApp->findGlobalAction("undo"));
    k->barGrid->addAction(kApp->findGlobalAction("redo"));

    k->barGrid->addSeparator();

    k->barGrid->addAction(k->actionManager->find("copy"));
    k->barGrid->addAction(k->actionManager->find("paste"));
    k->barGrid->addAction(k->actionManager->find("cut"));
    k->barGrid->addAction(k->actionManager->find("delete"));
    k->barGrid->addSeparator();

    k->barGrid->addAction(k->actionManager->find("show_grid"));
    k->barGrid->addSeparator();

    k->barGrid->addAction(k->actionManager->find("group"));
    k->barGrid->addAction(k->actionManager->find("ungroup"));

    k->barGrid->addSeparator();

    KCONFIG->beginGroup("OnionParameters");
    int preview = KCONFIG->value("PreviewFrames", -1).toInt();
    int next = KCONFIG->value("NextFrames", -1).toInt();

    k->prevOnionSkinSpin = new QSpinBox(this);
    k->prevOnionSkinSpin->setToolTip(tr("Preview Frames"));
    connect(k->prevOnionSkinSpin, SIGNAL(valueChanged(int)), this, SLOT(setPreviousOnionSkin(int)));

    if (preview > 0)
        k->prevOnionSkinSpin->setValue(preview);
    else
        k->prevOnionSkinSpin->setValue(1);

    k->barGrid->addWidget(k->prevOnionSkinSpin);
    k->barGrid->addAction(k->actionManager->find("onion"));

    k->nextOnionSkinSpin = new QSpinBox(this);
    k->nextOnionSkinSpin->setToolTip(tr("Next Frames"));
    connect(k->nextOnionSkinSpin, SIGNAL(valueChanged (int)), this, SLOT(setNextOnionSkin(int)));

    if (next > 0)
        k->nextOnionSkinSpin->setValue(next);
    else
        k->nextOnionSkinSpin->setValue(1);

    k->barGrid->addWidget(k->nextOnionSkinSpin);

    k->barGrid->addAction(k->actionManager->find("onionfactor"));

    k->onionFactorSpin = new QDoubleSpinBox(this);
    k->onionFactorSpin->setRange(0, 1);
    k->onionFactorSpin->setSingleStep(0.01);
    k->onionFactorSpin->setValue(k->opacityFactor);
    k->onionFactorSpin->setToolTip(tr("Onion Skin Factor"));
    connect(k->onionFactorSpin, SIGNAL(valueChanged(double)), this, SLOT(setOnionFactor(double)));

    k->barGrid->addWidget(k->onionFactorSpin);
    k->barGrid->addSeparator();

    k->spaceMode = new QComboBox();
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons/frames_mode.png"), tr("Frames Mode"));
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons/background_mode.png"), tr("Background Mode"));

    connect(k->spaceMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSpaceContext()));
    setSpaceContext();

    k->barGrid->addWidget(k->spaceMode);
}

void KTViewDocument::closeArea()
{
    if (k->configurationArea->isVisible())
        k->configurationArea->close();

    k->paintArea->setScene(0);
    close();
}

void KTViewDocument::undo()
{
    puts("Adding undo support");
}

void KTViewDocument::redo()
{
    puts("Adding redo support");
}

void KTViewDocument::setCursor(const QCursor &)
{
 /*
    k->paintArea->setCursor(c);
 */
}

void KTViewDocument::setPreviousOnionSkin(int level)
{
    KCONFIG->beginGroup("OnionParameters");
    KCONFIG->setValue("PreviewFrames", level);

    k->paintArea->setPreviousFramesOnionSkinCount(level);
}

void KTViewDocument::setNextOnionSkin(int level)
{
    KCONFIG->beginGroup("OnionParameters");
    KCONFIG->setValue("NextFrames", level);

    k->paintArea->setNextFramesOnionSkinCount(level);
}

void KTViewDocument::toggleShowGrid()
{
    k->paintArea->setDrawGrid(!k->paintArea->drawGrid());
}

void KTViewDocument::setZoomFactor(int /*percent*/)
{
    k->zoomFactorSpin->blockSignals(true);
    k->zoomFactorSpin->blockSignals(false);
}

void KTViewDocument::scaleRuler(double factor)
{
    Q_UNUSED(factor);
    /*
    double sep = factor * k->verticalRuler->scaleFactor();
    k->verticalRuler->scale(sep);
    k->horizontalRuler->scale(sep);
    */
}

void KTViewDocument::changeRulerOrigin(const QPointF &zero)
{
    k->verticalRuler->setZeroAt(zero);
    k->horizontalRuler->setZeroAt(zero);
}

QSize KTViewDocument::sizeHint() const
{
    QSize size(parentWidget()->size());

    return size.expandedTo(QApplication::globalStrut());
}

KTBrushManager *KTViewDocument::brushManager() const
{
    return k->paintArea->brushManager();
}

KTPaintAreaCommand *KTViewDocument::createCommand(const KTPaintAreaEvent *event)
{
    KTPaintAreaCommand *command = new KTPaintAreaCommand(k->paintArea, event);

    return command;
}

void KTViewDocument::updatePaintArea()
{
    k->paintArea->updatePaintArea(); 
}

void KTViewDocument::callAutoSave()
{
    emit autoSave();
}

void KTViewDocument::updateTimer()
{
    KCONFIG->beginGroup("General");
    int autoSave = KCONFIG->value("AutoSave").toInt();

    k->timer = new QTimer(this);

    if (autoSave > 0) {
        autoSave = autoSave*60000;
        connect(k->timer, SIGNAL(timeout()), this, SLOT(callAutoSave()));
        k->timer->start(autoSave);
    }
}

void KTViewDocument::setSpaceContext()
{
    QString option = k->spaceMode->currentText();
    int index = k->spaceMode->currentIndex();
    k->project->updateSpaceContext(index);
    k->paintArea->updateSpaceContext();

    k->paintArea->updatePaintArea();

   if (k->currentTool) {
       k->currentTool->init(k->paintArea->graphicsScene()); 
       if ((k->currentTool->toolType() == KTToolInterface::Tweener) && (index != 0)) {
           QAction *pencil = k->brushesMenu->actions().at(0);
           pencil->trigger();
       }
   }

   emit modeHasChanged(index);
}

KTProject::Mode KTViewDocument::spaceContext()
{
   return static_cast<KTProject::Mode>(k->spaceMode->currentIndex());
}

KTProject *KTViewDocument::project()
{
   return k->project;
}

void KTViewDocument::updateBgColor(const QColor color)
{
   k->project->setBgColor(color);
   k->paintArea->setBgColor(color);
}

void KTViewDocument::enableOnionFeature()
{
    if (!k->onionEnabled) {

        if (k->prevOnionValue == 0)
            k->prevOnionSkinSpin->setValue(1);
        else
            k->prevOnionSkinSpin->setValue(k->prevOnionValue);

        if (k->nextOnionValue == 0)
            k->nextOnionSkinSpin->setValue(1);
        else
            k->nextOnionSkinSpin->setValue(k->nextOnionValue);

        k->onionEnabled = true;

    } else {

        k->prevOnionValue = k->prevOnionSkinSpin->value();
        k->nextOnionValue = k->nextOnionSkinSpin->value();
        k->prevOnionSkinSpin->setValue(0);
        k->nextOnionSkinSpin->setValue(0);
        k->onionEnabled = false;

    }

    k->paintArea->updatePaintArea();
}

void KTViewDocument::setDefaultOnionFactor()
{
    k->onionFactorSpin->setValue(0.5);
    setOnionFactor(0.5);
}

void KTViewDocument::setOnionFactor(double opacity)
{
    KCONFIG->beginGroup("OnionParameters");
    KCONFIG->setValue("OnionFactor", opacity);

    k->paintArea->setOnionFactor(opacity);
}
