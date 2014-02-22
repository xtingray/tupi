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

#include "tupdocumentview.h"
#include "tupapplication.h"

// Tupi Framework 
#include "tdebug.h"
#include "tconfig.h"

#include "tuprequestbuilder.h"
#include "tuppaintareaproperties.h"
#include "tuppluginmanager.h"
#include "tupexportinterface.h"
#include "tuppaintarea.h"
#include "tupprojectresponse.h"
#include "tuppaintareaevent.h"
#include "tuppaintareacommand.h"
#include "tupgraphicsscene.h"

#include "tupbrushmanager.h"
#include "tupproject.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tuppaintareastatus.h"
#include "tupcanvas.h"
#include "polyline.h"
#include "tupimagedialog.h"
#include "tupstoryboarddialog.h"
#include "tupruler.h"
#include "tupcamerainterface.h"

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
#include <QDesktopWidget>
#include <QMessageBox>

#include <QCamera>
#include <QCameraImageCapture>

/**
 * This class defines all the environment for the Ilustration interface.
 * Here is where all the tools for Ilustration are defined.
 * @author David Cuadrado
*/

struct TupDocumentView::Private
{
    QSize wsDimension;
    QActionGroup *gridGroup; 
    QActionGroup *editGroup; 
    QActionGroup *viewNextGroup; 
    QActionGroup *viewZoomGroup; 
    QActionGroup *viewPreviousGroup;
    QMenu *brushesMenu;
    QMenu *selectionMenu;
    QMenu *fillMenu;
    QMenu *viewToolMenu;
    QMenu *motionMenu;

    QMenu *filterMenu;
    QMenu *toolsMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *orderMenu;

    QToolBar *barGrid;
    QToolBar *toolbar;
    QToolBar *propertiesBar;

    QDoubleSpinBox *zoomFactorSpin;
    QDoubleSpinBox *onionFactorSpin;
    QSpinBox *prevOnionSkinSpin;
    QSpinBox *nextOnionSkinSpin;

    QComboBox *dirCombo;
    QSpinBox *shiftSpin;

    bool onionEnabled;
    int prevOnionValue;
    int nextOnionValue;
    double opacityFactor;
    int viewAngle;
    int autoSaveTime;
    TAction *fullScreenAction;
    bool fullScreenOn;
    bool isNetworked;
    QStringList onLineUsers;

    TupPaintArea *paintArea;
    TupCanvas *fullScreen;

    TupRuler *verticalRuler;
    TupRuler *horizontalRuler;

    TActionManager *actionManager;
    TupConfigurationArea *configurationArea;
    TupToolPlugin *currentTool;
    TupPaintAreaStatus *status;
    QComboBox *spaceMode;
    bool dynamicFlag;

    TupProject *project;
    QTimer *timer;

    TupExportInterface *imagePlugin;
};

TupDocumentView::TupDocumentView(TupProject *project, QWidget *parent, bool isNetworked, const QStringList &users) : QMainWindow(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    setWindowIcon(QPixmap(THEME_DIR + "icons/animation_mode.png"));

    k->project = project;
    k->currentTool = 0;
    k->onionEnabled = true;
    k->fullScreenOn = false;
    k->viewAngle = 0;
    k->isNetworked = isNetworked;
    k->onLineUsers = users;
    k->dynamicFlag = false;

    k->actionManager = new TActionManager(this);

    QFrame *frame = new QFrame(this, Qt::FramelessWindowHint);
    QGridLayout *layout = new QGridLayout(frame);

    k->paintArea = new TupPaintArea(project, frame);

    TCONFIG->beginGroup("OnionParameters");
    k->opacityFactor = TCONFIG->value("OnionFactor", -1).toDouble();
    if (k->opacityFactor < 0)
        k->opacityFactor = 0.5;
    k->paintArea->setOnionFactor(k->opacityFactor);

    setCentralWidget(frame);

    layout->addWidget(k->paintArea, 1, 1);

    k->horizontalRuler = new TupRuler(Qt::Horizontal, this);
    k->verticalRuler = new TupRuler(Qt::Vertical, this);
    layout->addWidget(k->horizontalRuler, 0, 1);
    layout->addWidget(k->verticalRuler, 1, 0);

    connect(k->paintArea, SIGNAL(scaled(double)), this, SLOT(updateScaleVars(double)));
    connect(k->paintArea, SIGNAL(rotated(int)), this, SLOT(updateRotationVars(int)));
    connect(k->paintArea, SIGNAL(updateStatusBgColor(QColor)), this, SLOT(updateStatusBgColor(QColor)));

    Tupi::RenderType renderType = Tupi::RenderType(TCONFIG->value("RenderType").toInt()); 

    switch (renderType) {
            case Tupi::OpenGL:
                 k->paintArea->setUseOpenGL(true);
                 break;

            case Tupi::Native:
                 k->paintArea->setUseOpenGL(false);
                 break;

            default:
                 #ifdef K_DEBUG
                        tWarning() << "Unsopported render, switching to native!";
                 #endif
                 k->paintArea->setUseOpenGL(false);
            break;
    }
    
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), this, SLOT(showPos(const QPointF &)));

    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->verticalRuler, SLOT(movePointers(const QPointF&)));
    connect(k->paintArea, SIGNAL(cursorPosition(const QPointF &)), k->horizontalRuler, SLOT(movePointers(const QPointF&)));

    connect(k->paintArea, SIGNAL(changedZero(const QPointF&)), this, SLOT(changeRulerOrigin(const QPointF&)));

    connect(k->paintArea, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
    connect(k->paintArea, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));

    setupDrawActions();

    //k->configurationArea = new TupConfigurationArea(this);
    //addDockWidget(Qt::RightDockWidgetArea, k->configurationArea);
    //k->configurationArea->close();
   
    createTools(); 
    createToolBar();
    
    k->status = new TupPaintAreaStatus(this);
    setStatusBar(k->status);
    connect(k->status, SIGNAL(colorRequested()), this, SIGNAL(expandColorPanel()));
    connect(k->status, SIGNAL(colorUpdated(const QColor)), this, SLOT(updateBgColor(const QColor)));
    connect(k->status, SIGNAL(newFramePointer(int)), k->paintArea, SLOT(goToFrame(int)));
    connect(k->paintArea, SIGNAL(frameChanged(int)), k->status, SLOT(updateFrameIndex(int)));

    // SQA: Verify if this code is doing something / SLOT setBrush() doesn't exist
    // connect(k->paintArea->brushManager(), SIGNAL(brushChanged(const QBrush&)), k->status, 
    //         SLOT(setBrush(const QBrush &)));

    connect(k->paintArea->brushManager(), SIGNAL(penChanged(const QPen&)), k->status, SLOT(setPen(const QPen &)));

    QTimer::singleShot(1000, this, SLOT(loadPlugins()));

    // SQA: Temporarily disabled  
    // if (!k->isNetworked)
    //     saveTimer();
}

TupDocumentView::~TupDocumentView()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    TCONFIG->beginGroup("General");
    TCONFIG->setValue("AutoSave", k->autoSaveTime);

    if (k->currentTool)
        k->currentTool->saveConfig();

    delete k->configurationArea;
    delete k;
}

void TupDocumentView::setWorkSpaceSize(int width, int height)
{
    k->wsDimension = QSize(width, height);
}

void TupDocumentView::setAntialiasing(bool useIt)
{
    k->paintArea->setAntialiasing(useIt);
}

void TupDocumentView::setOpenGL(bool useIt)
{
    k->paintArea->setUseOpenGL(useIt);
}

void TupDocumentView::setDrawGrid(bool draw)
{
    k->paintArea->setDrawGrid(draw);
}

QPainter::RenderHints TupDocumentView::renderHints() const
{
    return k->paintArea->renderHints();
}

void TupDocumentView::updateRotationAngleFromRulers(int angle)
{
    k->viewAngle = angle;

    TupRuler::Transformation flag = TupRuler::None;
    if (angle != 0 && angle != 90 && angle != 180  && angle != 270)
        flag = TupRuler::Rotation;
    else
        flag = TupRuler::None;

    k->verticalRuler->updateCurrentTransformation(flag);
    k->horizontalRuler->updateCurrentTransformation(flag);
}

void TupDocumentView::setRotationAngle(int angle)
{
    updateRotationAngleFromRulers(angle);

    k->paintArea->setRotationAngle(angle);
}

void TupDocumentView::updateRotationVars(int angle)
{
    updateRotationAngleFromRulers(angle);

    k->status->updateRotationAngle(angle);
}

void TupDocumentView::setZoom(qreal factor)
{
    k->paintArea->setZoom(factor);
    k->verticalRuler->setRulerZoom(factor);
    k->horizontalRuler->setRulerZoom(factor);

    if (k->currentTool) {
        if (k->currentTool->name().compare(tr("Object Selection")) == 0)
            k->currentTool->resizeNodes(factor);
    }
}

void TupDocumentView::setZoomView(const QString &percent)
{
    k->status->setZoomFactor(percent);
}

void TupDocumentView::showPos(const QPointF &point)
{
    QString message =  "X: " +  QString::number(point.x()) + " Y: " + QString::number(point.y());
    emit sendToStatus(message);
}

void TupDocumentView::setupDrawActions()
{
    TAction *showGrid = new TAction(QPixmap(THEME_DIR + "icons/subgrid.png"), 
                                    tr("Show grid"), QKeySequence(tr("#")),
                                    this, SLOT(toggleShowGrid()), k->actionManager, "show_grid");
    showGrid->setStatusTip(tr("Show a grid over the canvas"));
    showGrid->setCheckable(true);

    k->fullScreenAction = new TAction(QPixmap(THEME_DIR + "icons/full_screen.png"),
                                    tr("Full screen"), QKeySequence(tr("F11")),
                                    this, SLOT(showFullScreen()), k->actionManager, "full_screen");
    k->fullScreenAction->setStatusTip(tr("Open a full screen view of canvas"));

    TAction *copy = new TAction(QPixmap(THEME_DIR + "icons/copy.png"), 
                                tr("Copy"), QKeySequence(tr("Ctrl+C")),
                                k->paintArea, SLOT(copyItems()), k->actionManager, "copy");
    copy->setStatusTip(tr("Copies the selection and puts it onto the clipboard"));

    TAction *paste = new TAction(QPixmap(THEME_DIR + "icons/paste.png"), 
                                 tr("Paste"), QKeySequence(tr("Ctrl+V")),
                                 k->paintArea, SLOT(pasteItems()), k->actionManager, "paste");
    paste->setStatusTip(tr("Pastes the clipboard into the current document"));

    TAction *cut = new TAction(QPixmap(THEME_DIR + "icons/cut.png"), 
                               tr("Cut"), QKeySequence(tr("Ctrl+X")),
                               k->paintArea, SLOT(cutItems()),k->actionManager, "cut");
    cut->setStatusTip(tr("Cuts the selected items"));

    TAction *del = new TAction(QPixmap(THEME_DIR + "icons/delete.png"), tr("Delete"), 
                               QKeySequence(Qt::Key_Delete), k->paintArea, SLOT(deleteItems()), 
                               k->actionManager, "delete");
    
    del->setStatusTip(tr("Deletes the selected object"));

    TAction *group = new TAction(QPixmap(THEME_DIR + "icons/group.png"), tr("&Group"),   
                                 QKeySequence(tr("Ctrl+G")), k->paintArea, SLOT(groupItems()), 
                                 k->actionManager, "group");

    group->setStatusTip(tr("Group the selected objects into a single one"));
    group->setDisabled(true);

    TAction *ungroup = new TAction(QPixmap(THEME_DIR + "icons/ungroup.png"), tr("&Ungroup"), 
                                    QKeySequence(tr("Ctrl+Shift+G")) , k->paintArea, SLOT(ungroupItems()), 
                                    k->actionManager, "ungroup");
    ungroup->setDisabled(true);

    ungroup->setStatusTip(tr("Ungroups the selected object"));

    TAction *onion = new TAction(QPixmap(THEME_DIR + "icons/layer.png"), tr("Onion Skin"),
                               QKeySequence(tr("Ctrl+Shift+O")), this, SLOT(enableOnionFeature()),
                               k->actionManager, "onion");

    onion->setStatusTip(tr("Enable/Disable onion skin"));

    TAction *onionFactor = new TAction(QPixmap(THEME_DIR + "icons/onion.png"), tr("Onion Skin Factor"),
                               QKeySequence(tr("Ctrl+Shift+S")), this, SLOT(setDefaultOnionFactor()),
                               k->actionManager, "onionfactor");

    onionFactor->setStatusTip(tr("Set onion skin factor default value"));

    TAction *exportImage = new TAction(QPixmap(THEME_DIR + "icons/export_frame.png"),
                                     tr("Export Current Frame As Image"), QKeySequence(tr("@")),
                                     this, SLOT(exportImage()), k->actionManager, "export_image");
    exportImage->setStatusTip("Export the current frame as image");

    TCONFIG->beginGroup("Network");
    QString server = TCONFIG->value("Server").toString();

    if (k->isNetworked && server.compare("tupitu.be") == 0) {
        TAction *postImage = new TAction(QPixmap(THEME_DIR + "icons/import_project.png"),
                                         tr("Export Current Frame To Gallery"), QKeySequence(tr("@")),
                                         this, SLOT(postImage()), k->actionManager, "post_image");
        postImage->setStatusTip("Export the current frame to gallery");
    }

    TAction *storyboard = new TAction(QPixmap(THEME_DIR + "icons/storyboard.png"),
                                     tr("Storyboard Settings"), QKeySequence(tr("Ctrl+Shift+S")),
                                     this, SLOT(storyboardSettings()), k->actionManager, "storyboard");
    storyboard->setStatusTip("Storyboard settings");

    TAction *camera = new TAction(QPixmap(THEME_DIR + "icons/camera.png"),
                                     tr("Camera Interface"), QKeySequence(tr("Ctrl+Shift+C")),
                                     this, SLOT(cameraInterface()), k->actionManager, "camera");
    camera->setStatusTip("Camera Interface");
}

void TupDocumentView::createTools()
{
    k->toolbar = new QToolBar(tr("Draw tools"), this);
    k->toolbar->setIconSize(QSize(16, 16));
    addToolBar(Qt::LeftToolBarArea, k->toolbar);

    connect(k->toolbar, SIGNAL(actionTriggered(QAction *)), this, SLOT(selectToolFromMenu(QAction *)));

    // Brushes menu
    k->brushesMenu = new QMenu(tr("Brushes"), k->toolbar);
    k->brushesMenu->setIcon(QPixmap(THEME_DIR + "icons/brush.png"));
    connect(k->brushesMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->brushesMenu->menuAction());

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
    k->viewToolMenu->setIcon(QPixmap(THEME_DIR + "icons/zoom.png"));
    connect(k->fillMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->viewToolMenu->menuAction());

    // Motion Tween menu
    k->motionMenu = new QMenu(tr("Tweening"), k->toolbar);
    k->motionMenu->setIcon(QPixmap(THEME_DIR + "icons/tweening.png"));
    connect(k->motionMenu, SIGNAL(triggered(QAction *)), this, SLOT(selectToolFromMenu(QAction*)));

    k->toolbar->addAction(k->motionMenu->menuAction());
}

void TupDocumentView::loadPlugins()
{
    bool imagePluginLoaded = false; 
    foreach (QObject *plugin, TupPluginManager::instance()->formats()) {
             if (plugin) {
                 TupExportInterface *exporter = qobject_cast<TupExportInterface *>(plugin);
                 if (exporter) {
                     if (exporter->key().compare(tr("Image Array")) == 0) {
                         k->imagePlugin = exporter;
                         imagePluginLoaded = true;
                         break;
                     }
                 }
             }
    }

    if (!imagePluginLoaded) {
        #ifdef K_DEBUG
               tError() << "TupDocumentView::loadPlugins() - Fatal Error: Couldn't found the \"Image Array\" plugin!";
        #endif
    }

    QVector<TAction*> brushTools(8);
    QVector<TAction*> tweenTools(7);

    TAction *pencil = 0;

    foreach (QObject *plugin, TupPluginManager::instance()->tools()) {

             TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(plugin);

             if (tool->toolType() != TupToolInterface::Tweener) {
                 connect(tool, SIGNAL(closeHugeCanvas()), this, SLOT(closeFullScreen()));
                 connect(tool, SIGNAL(callForPlugin(int, int)), this, SLOT(loadPlugin(int, int)));
             }

             QStringList::iterator it;
             QStringList keys = tool->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                         tWarning() << "TupDocumentView::loadPlugins() - Tool Loaded: " << *it;
                  #endif

                  TAction *action = tool->actions()[*it];
                  action->setIconVisibleInMenu(true);

                  if (action) {
                      connect(action, SIGNAL(triggered()), this, SLOT(selectTool()));
                      action->setParent(plugin);
                      QString toolName = action->text();

                      switch (tool->toolType()) {
                              case TupToolInterface::Brush:
                                 {
                                   // SQA: Experimental plugin (disabled)
                                   /*
                                   if (toolName.compare(tr("Scheme")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[1] = action;
                                   }
                                   */

                                   if (toolName.compare(tr("Pencil")) == 0) {
                                       brushTools[0] = action;
                                       pencil = action;
                                       k->brushesMenu->setDefaultAction(action);
                                   }

                                   if (toolName.compare(tr("Ink")) == 0) {
                                       brushTools[1] = action;
                                   }

                                   if (toolName.compare(tr("Eraser")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[2] = action;
                                   }

                                   if (toolName.compare(tr("PolyLine")) == 0) {
                                       brushTools[3] = action;
                                       TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
                                       connect(k->paintArea, SIGNAL(closePolyLine()), tool, SLOT(initEnv()));
                                       connect(this, SIGNAL(closePolyLine()), tool, SLOT(initEnv()));
                                   }

                                   if (toolName.compare(tr("Line")) == 0)
                                       brushTools[4] = action;

                                   if (toolName.compare(tr("Rectangle")) == 0)
                                       brushTools[5] = action;

                                   if (toolName.compare(tr("Ellipse")) == 0)
                                       brushTools[6] = action;

                                   if (toolName.compare(tr("Text")) == 0) {
                                       action->setDisabled(true);
                                       brushTools[7] = action;
                                   }
                                 }
                                 break;
                              case TupToolInterface::Tweener:
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
                              case TupToolInterface::Selection:
                                 {
                                   k->selectionMenu->addAction(action);
                                   if (toolName.compare(tr("Object Selection")) == 0)
                                       k->selectionMenu->setDefaultAction(action);
                                 }
                                 break;
                              case TupToolInterface::Fill:
                                 {
                                   k->fillMenu->addAction(action);
                                   if (toolName.compare(tr("Internal fill")) == 0)
                                       k->fillMenu->setDefaultAction(action);
                                 }
                                 break;
                               case TupToolInterface::View:
                                 {
                                   k->viewToolMenu->addAction(action);
                                   if (toolName.compare(tr("Zoom In")) == 0)
                                       k->viewToolMenu->setDefaultAction(action);

                                 }
                                 break;
                               default:
                                 break;
                      }
                  }
             }
    } // end foreach

    for (int i = 0; i < brushTools.size(); ++i) {
         k->brushesMenu->addAction(brushTools.at(i));
    }

    for (int i = 0; i < tweenTools.size(); ++i)
         k->motionMenu->addAction(tweenTools.at(i));

    foreach (QObject *plugin, TupPluginManager::instance()->filters()) {
             AFilterInterface *filter = qobject_cast<AFilterInterface *>(plugin);
             QStringList::iterator it;
             QStringList keys = filter->keys();

             for (it = keys.begin(); it != keys.end(); ++it) {
                  #ifdef K_DEBUG
                         tDebug("plugins") << "*** Filter Loaded: " << *it;
                  #endif

                  TAction *act = filter->actions()[*it];
                  if (act) {
                      connect(act, SIGNAL(triggered()), this, SLOT(applyFilter()));
                      k->filterMenu->addAction(act);
                  }
             }
    }

    brushTools.clear();
    tweenTools.clear();

    pencil->trigger();
}

void TupDocumentView::loadPlugin(int menu, int index)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TAction *action = 0;

    switch (menu) {
            case TupToolPlugin::Arrows:
                 {
                     if (index == TupToolPlugin::FrameBack) {
                         k->paintArea->goOneFrameBack();
                     } else if (index == TupToolPlugin::FrameForward) {
                                k->paintArea->goOneFrameForward();
                     } else if (index == TupToolPlugin::QuickCopy) {
                                k->paintArea->quickCopy();
                     }
                     return;
                 }
            break;
            case TupToolPlugin::ColorMenu:
                 {
                     if (index == TupToolPlugin::ColorTool) {
                         if (k->fullScreenOn) {
                             QColor currentColor = brushManager()->penColor();
                             emit openColorDialog(currentColor);
                         }
                         return;
                     }
                 }
            break;
            case TupToolPlugin::BrushesMenu:
                 {
                     QList<QAction*> brushActions = k->brushesMenu->actions();
                     if (index < brushActions.size()) {
                         action = (TAction *) brushActions[index];
                     } else {
                         #ifdef K_DEBUG
                                tError() << "TupDocumentView::loadPlugin() - Error: Invalid Brush Index / No plugin loaded";
                         #endif
                         return;
                     }
                 }
            break;
            case TupToolPlugin::SelectionMenu:
                 {
                     if (index == TupToolPlugin::Delete) {
                         k->paintArea->deleteItems();
                     } else {
                         QList<QAction*> selectionActions = k->selectionMenu->actions();
                         if (index < selectionActions.size()) {
                             action = (TAction *) selectionActions[index];
                         } else {
                             #ifdef K_DEBUG
                                    tError() << "TupDocumentView::loadPlugin() - Error: Invalid Selection Index / No plugin loaded";
                             #endif
                             return;
                         }
                     }
                 }
            break;
            case TupToolPlugin::FillMenu:
                 {
                     QList<QAction*> fillActions = k->fillMenu->actions();
                     if (index < fillActions.size()) {
                         action = (TAction *) fillActions[index];
                     } else {
                         #ifdef K_DEBUG
                                tError() << "TupDocumentView::loadPlugin() - Error: Invalid Fill Index / No plugin loaded";
                         #endif
                         return;
                     }
                 }
            break;
            case TupToolPlugin::ZoomMenu:
                 {
                     QList<QAction*> viewActions = k->viewToolMenu->actions();
                     if (index < viewActions.size()) {
                         action = (TAction *) viewActions[index];
                     } else {
                         #ifdef K_DEBUG
                                tError() << "TupDocumentView::loadPlugin() - Error: Invalid Zoom Index (" << index << ") / No plugin loaded";
                         #endif
                         return;
                     }
                 }
            break;
            default:
                 {
                     #ifdef K_DEBUG
                            tError() << "TupDocumentView::loadPlugin() - Error: Invalid Menu Index / No plugin loaded";
                     #endif
                     return;
                 }
            break;
    }

    if (action) {
        QString toolName = tr("%1").arg(action->text());

        if (index == TupToolPlugin::ZoomInTool || index == TupToolPlugin::ZoomOutTool) {
            if (k->fullScreenOn) {
                action->trigger();
                k->fullScreen->updateCursor(action->cursor());
                TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
                tool->autoZoom();
            }
        } else if (toolName.compare(k->currentTool->name()) != 0) {
            if (k->fullScreenOn) {
                action->trigger();
                k->fullScreen->updateCursor(action->cursor());
            }
        }
    } else {
        #ifdef K_DEBUG
               tError() << "TupDocumentView::loadPlugin() - Error: Action pointer is NULL!";
        #endif
        return;
    }
}

void TupDocumentView::selectTool()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    TAction *action = qobject_cast<TAction *>(sender());

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

        TupToolPlugin *tool = qobject_cast<TupToolPlugin *>(action->parent());
        k->currentTool = tool; 
        tool->setName(toolName);
        k->paintArea->setCurrentTool(toolName);

        if (!action->icon().isNull())
            k->status->updateTool(toolName, action->icon().pixmap(15, 15));

        int minWidth = 0;

        switch (tool->toolType()) {
                case TupToolInterface::Brush: 
                     k->fullScreenAction->setDisabled(false);
                     if (toolName.compare(tr("Pencil"))==0) {
                         minWidth = 130;
                     } else if (toolName.compare(tr("Text"))==0) {
                                minWidth = 350;
                     } else if (toolName.compare(tr("PolyLine"))==0 
                                || toolName.compare(tr("Rectangle"))==0 
                                || toolName.compare(tr("Ellipse"))==0) {
                                minWidth = 130;
                     }

                     k->brushesMenu->setDefaultAction(action);
                     k->brushesMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->brushesMenu->menuAction()->setIcon(action->icon());
                     break;
                case TupToolInterface::Tweener:
                     k->fullScreenAction->setDisabled(true);
                     minWidth = 220;
                     k->motionMenu->setDefaultAction(action);
                     k->motionMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->motionMenu->menuAction()->setIcon(action->icon());
                     break;
                case TupToolInterface::Fill:
                     k->fullScreenAction->setDisabled(false);
                     k->fillMenu->setDefaultAction(action);
                     k->fillMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->fillMenu->menuAction()->setIcon(action->icon());
                     break;
                case TupToolInterface::Selection:
                     k->fullScreenAction->setDisabled(false);
                     k->selectionMenu->setDefaultAction(action);
                     k->selectionMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->selectionMenu->menuAction()->setIcon(action->icon());
                     if (toolName.compare(tr("Object Selection"))==0) {
                         minWidth = 130;
                         connect(k->paintArea, SIGNAL(itemAddedOnSelection(TupGraphicsScene *)), 
                                 tool, SLOT(updateItems(TupGraphicsScene *)));
                     } 
                     break;
                case TupToolInterface::View:
                     k->fullScreenAction->setDisabled(false);
                     k->viewToolMenu->setDefaultAction(action);
                     k->viewToolMenu->setActiveAction(action);
                     if (!action->icon().isNull())
                         k->viewToolMenu->menuAction()->setIcon(action->icon());
                     if (toolName.compare(tr("Zoom In"))==0 || toolName.compare(tr("Zoom Out"))==0)
                         minWidth = 130;

                     if (toolName.compare(tr("Hand"))==0) {
                         tool->setProjectSize(k->project->dimension());
                     }
                     break;
        }

        QWidget *toolConfigurator = tool->configurator();

        if (toolConfigurator) {
            k->configurationArea = new TupConfigurationArea(this);
            k->configurationArea->setConfigurator(toolConfigurator, minWidth);
            addDockWidget(Qt::RightDockWidgetArea, k->configurationArea);
            toolConfigurator->show();
            if (!k->configurationArea->isVisible())
                k->configurationArea->show();
        } else {
            if (k->configurationArea->isVisible())
                k->configurationArea->close();
        }

        k->paintArea->setTool(tool);
        k->paintArea->viewport()->setCursor(action->cursor());

        if (toolName.compare(tr("Object Selection"))==0) {
            qreal globalFactor = k->status->currentZoomFactor();
            qreal factor = globalFactor*0.01;
            tool->updateZoomFactor(factor);
        }

    } else {
        #ifdef K_DEBUG
               tError() << "TupDocumentView::selectTool() - Fatal Error: Action from sender() is NULL";
        #endif
    }
}

void TupDocumentView::selectToolFromMenu(QAction *action)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    QMenu *menu = qobject_cast<QMenu *>(action->parent());

    if (menu) {
        TAction *tool = qobject_cast<TAction *>(menu->activeAction());

        if (tool) {
            if (tool->text().compare(k->currentTool->name()) == 0)
                return;
            tool->trigger(); // this line calls selectTool()
        } else {
            tool = qobject_cast<TAction *>(menu->defaultAction());
            if (tool) {
                tool->trigger();
            } else {
                #ifdef K_DEBUG
                       tError() << "TupDocumentView::selectToolFromMenu() - Default action is NULL";
                #endif
            }
        }
    } else {
        #ifdef K_DEBUG
               tFatal() << "TupDocumentView::selectToolFromMenu() - Error: Action with NO parent! Aborting...";
        #endif
    } 
}

bool TupDocumentView::handleProjectResponse(TupProjectResponse *event)
{
    return k->paintArea->handleResponse(event);
}

void TupDocumentView::applyFilter()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        // SQA: Check this code to understand how filters work 
        /*
        SQA issue: Pending to check this code  
        AFilterInterface *aFilter = qobject_cast<AFilterInterface *>(action->parent());
        QString filter = action->text();
        
        TupFrame *frame = k->paintArea->currentFrame();
        if (frame) {
            aFilter->filter(action->text(), frame->components());
            k->paintArea->redrawAll();
        }
        */
    }
}

void TupDocumentView::updateZoomFactor(double factor)
{
    k->zoomFactorSpin->blockSignals(true);
    k->zoomFactorSpin->setValue(factor*100);
    k->zoomFactorSpin->blockSignals(false);
}

void TupDocumentView::createToolBar()
{
    k->barGrid = new QToolBar(tr("Paint area actions"), this);
    k->barGrid->setIconSize(QSize(16, 16));

    k->propertiesBar = new QToolBar(tr("Dynamic Background Properties"), this);

    addToolBar(k->barGrid);

    k->spaceMode = new QComboBox();
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons/frames_mode.png"), tr("Frames Mode"));
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons/static_background_mode.png"), tr("Static BG Mode"));
    k->spaceMode->addItem(QIcon(THEME_DIR + "icons/dynamic_background_mode.png"), tr("Dynamic BG Mode"));

    connect(k->spaceMode, SIGNAL(currentIndexChanged(int)), this, SLOT(setSpaceContext()));
    setSpaceContext();
    k->barGrid->addWidget(k->spaceMode);

    k->barGrid->addAction(kApp->findGlobalAction("undo"));
    k->barGrid->addAction(kApp->findGlobalAction("redo"));

    k->barGrid->addAction(k->actionManager->find("copy"));
    k->barGrid->addAction(k->actionManager->find("paste"));
    k->barGrid->addAction(k->actionManager->find("cut"));
    k->barGrid->addAction(k->actionManager->find("delete"));

    k->barGrid->addAction(k->actionManager->find("show_grid"));
    k->barGrid->addAction(k->actionManager->find("full_screen"));

    k->barGrid->addAction(k->actionManager->find("group"));
    k->barGrid->addAction(k->actionManager->find("ungroup"));

    TCONFIG->beginGroup("OnionParameters");
    int preview = TCONFIG->value("PreviousFrames", -1).toInt();
    int next = TCONFIG->value("NextFrames", -1).toInt();

    k->prevOnionSkinSpin = new QSpinBox(this);
    k->prevOnionSkinSpin->setToolTip(tr("Previous Frames"));
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

    k->barGrid->addAction(k->actionManager->find("export_image"));

    TCONFIG->beginGroup("Network");
    QString server = TCONFIG->value("Server").toString();

    if (k->isNetworked && server.compare("tupitu.be") == 0)
        k->barGrid->addAction(k->actionManager->find("post_image"));

    k->barGrid->addAction(k->actionManager->find("storyboard"));

    k->barGrid->addAction(k->actionManager->find("camera"));

    addToolBarBreak();

    QLabel *dirLabel = new QLabel(tr("Direction") + ": ");

    k->dirCombo = new QComboBox;
    k->dirCombo->addItem(tr("Left to Right"));
    k->dirCombo->addItem(tr("Right to Left"));
    k->dirCombo->addItem(tr("Top to Bottom"));
    k->dirCombo->addItem(tr("Bottom to Top"));
    connect(k->dirCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setBackgroundDirection(int)));

    QLabel *shiftLabel = new QLabel(tr("Shift Length") + ": ");

    k->shiftSpin = new QSpinBox(this);
    k->shiftSpin->setSingleStep(1);
    k->shiftSpin->setRange(1, 1000);
    connect(k->shiftSpin, SIGNAL(valueChanged(int)), this, SLOT(updateBackgroundShiftProperty(int)));

    QWidget *empty1 = new QWidget();
    empty1->setFixedWidth(5);
    QWidget *empty2 = new QWidget();
    empty2->setFixedWidth(5);

    k->propertiesBar->addWidget(dirLabel);
    k->propertiesBar->addWidget(k->dirCombo);
    k->propertiesBar->addWidget(empty1);
    k->propertiesBar->addSeparator();
    k->propertiesBar->addWidget(empty2);
    k->propertiesBar->addWidget(shiftLabel);
    k->propertiesBar->addWidget(k->shiftSpin);

    k->propertiesBar->setVisible(false);

    addToolBar(k->propertiesBar);
}

void TupDocumentView::closeArea()
{
    if (k->configurationArea->isVisible())
        k->configurationArea->close();

    k->paintArea->setScene(0);
    close();
}

void TupDocumentView::undo()
{
    puts("Adding undo support");
}

void TupDocumentView::redo()
{
    puts("Adding redo support");
}

void TupDocumentView::setCursor(const QCursor &cursor)
{
    Q_UNUSED(cursor);
 /*
    k->paintArea->setCursor(c);
 */
}

void TupDocumentView::setPreviousOnionSkin(int level)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("PreviousFrames", level);

    k->paintArea->setPreviousFramesOnionSkinCount(level);
}

void TupDocumentView::setNextOnionSkin(int level)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("NextFrames", level);

    k->paintArea->setNextFramesOnionSkinCount(level);
}

void TupDocumentView::toggleShowGrid()
{
    k->paintArea->setDrawGrid(!k->paintArea->drawGrid());
}

void TupDocumentView::updateScaleVars(double factor)
{
    k->status->updateZoomFactor(factor);
    k->verticalRuler->setRulerZoom(factor);
    k->horizontalRuler->setRulerZoom(factor);

    if (k->currentTool) {
        if (k->currentTool->name().compare(tr("Object Selection")) == 0)
            k->currentTool->resizeNodes(factor);
    }
}

void TupDocumentView::changeRulerOrigin(const QPointF &zero)
{
    k->verticalRuler->setOrigin(zero.y());
    k->horizontalRuler->setOrigin(zero.x());
}

QSize TupDocumentView::sizeHint() const
{
    QSize size(parentWidget()->size());

    return size.expandedTo(QApplication::globalStrut());
}

QSize TupDocumentView::workSpaceSize() const
{
    return k->paintArea->size();
}

TupBrushManager *TupDocumentView::brushManager() const
{
    return k->paintArea->brushManager();
}

TupPaintAreaCommand *TupDocumentView::createCommand(const TupPaintAreaEvent *event)
{
    TupPaintAreaCommand *command = new TupPaintAreaCommand(k->paintArea, event);

    return command;
}

void TupDocumentView::updatePaintArea()
{
    k->paintArea->updatePaintArea(); 
}

void TupDocumentView::callAutoSave()
{
    emit autoSave();
}

void TupDocumentView::saveTimer()
{
    TCONFIG->beginGroup("General");
    k->autoSaveTime = TCONFIG->value("AutoSave", 10).toInt();

    k->timer = new QTimer(this);

    if (k->autoSaveTime != 0) {
        if (k->autoSaveTime < 0 || k->autoSaveTime > 60) 
            k->autoSaveTime = 5;

        int saveTime = k->autoSaveTime*60000;
        connect(k->timer, SIGNAL(timeout()), this, SLOT(callAutoSave()));
        k->timer->start(saveTime);
    }
}

void TupDocumentView::setSpaceContext()
{
    TupProject::Mode mode = TupProject::Mode(k->spaceMode->currentIndex());

    if (mode == TupProject::FRAMES_EDITION) {
        if (k->dynamicFlag) {
            k->dynamicFlag = false;
            renderDynamicBackground();
        }
        k->project->updateSpaceContext(TupProject::FRAMES_EDITION);
        k->propertiesBar->setVisible(false);
        k->motionMenu->setEnabled(true);
    } else if (mode == TupProject::STATIC_BACKGROUND_EDITION) {
               if (k->dynamicFlag) {
                   k->dynamicFlag = false;
                   renderDynamicBackground();
               }
               k->project->updateSpaceContext(TupProject::STATIC_BACKGROUND_EDITION);
               k->propertiesBar->setVisible(false);
               k->motionMenu->setEnabled(false);
    } else if (mode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
               k->dynamicFlag = true;
               k->project->updateSpaceContext(TupProject::DYNAMIC_BACKGROUND_EDITION);

               int sceneIndex = k->paintArea->currentSceneIndex();
               TupScene *scene = k->project->scene(sceneIndex);
               if (scene) {
                   TupBackground *bg = scene->background();
                   if (bg) {
                       int direction = bg->dyanmicDirection();
                       k->dirCombo->setCurrentIndex(direction);
                       int shift = bg->dyanmicShift();
                       k->shiftSpin->setValue(shift);
                   }
               }
               k->propertiesBar->setVisible(true);
               k->motionMenu->setEnabled(false);
    }

    k->paintArea->updateSpaceContext();
    k->paintArea->updatePaintArea();

   if (k->currentTool) {
       k->currentTool->init(k->paintArea->graphicsScene()); 
       if ((k->currentTool->toolType() == TupToolInterface::Tweener) && (mode != TupProject::FRAMES_EDITION)) {
           QAction *pencil = k->brushesMenu->actions().at(0);
           pencil->trigger();
       }
   }

   emit modeHasChanged(mode);
}

TupProject::Mode TupDocumentView::spaceContext()
{
   return TupProject::Mode(k->spaceMode->currentIndex());
}

TupProject *TupDocumentView::project()
{
   return k->project;
}

int TupDocumentView::currentFramesTotal()
{
   int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
   int layerIndex = k->paintArea->graphicsScene()->currentLayerIndex();

   TupScene *scene = k->project->scene(sceneIndex);

   if (scene) {
       TupLayer *layer = scene->layer(layerIndex);
       if (layer)
           return layer->framesTotal();
    }

    return -1;
}

int TupDocumentView::currentSceneIndex()
{
    if (k->paintArea)
        return k->paintArea->graphicsScene()->currentSceneIndex();
   
    return -1; 
}

void TupDocumentView::updateBgColor(const QColor color)
{
   if (!k->isNetworked) {
       k->project->setBgColor(color);
       k->paintArea->setBgColor(color);
   } else {
       TupProjectRequest event = TupRequestBuilder::createSceneRequest(currentSceneIndex(), TupProjectRequest::BgColor, color.name());
       emit requestTriggered(&event);
   }
}

void TupDocumentView::enableOnionFeature()
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

void TupDocumentView::setDefaultOnionFactor()
{
    k->onionFactorSpin->setValue(0.5);
    setOnionFactor(0.5);
}

void TupDocumentView::setOnionFactor(double opacity)
{
    TCONFIG->beginGroup("OnionParameters");
    TCONFIG->setValue("OnionFactor", opacity);

    k->paintArea->setOnionFactor(opacity);
}

void TupDocumentView::showFullScreen()
{
    if (k->fullScreenOn || k->currentTool->toolType() == TupToolInterface::Tweener)
        return;

    k->fullScreenOn = true;

    QDesktopWidget desktop;
    int screenW = desktop.screenGeometry().width();
    int screenH = desktop.screenGeometry().height();

    double scale = 1;

    QSize projectSize = k->project->dimension();
    if (projectSize.width() < projectSize.height())
        scale = (double) (screenW - 50) / (double) projectSize.width();
    else
        scale = (double) (screenH - 50) / (double) projectSize.height();

    k->fullScreen = new TupCanvas(this, Qt::Window|Qt::FramelessWindowHint, k->paintArea->graphicsScene(), 
                                 k->paintArea->centerPoint(), QSize(screenW, screenH), k->project, scale,
                                 k->viewAngle, brushManager(), k->isNetworked, k->onLineUsers); 

    k->fullScreen->updateCursor(k->currentTool->cursor());
    k->fullScreen->showFullScreen();

    connect(this, SIGNAL(openColorDialog(const QColor &)), k->fullScreen, SLOT(colorDialog(const QColor &)));
    connect(k->fullScreen, SIGNAL(updateColorFromFullScreen(const QColor &)), this, SIGNAL(updateColorFromFullScreen(const QColor &)));
    connect(k->fullScreen, SIGNAL(updatePenThicknessFromFullScreen(int)), this, SLOT(updatePenThickness(int)));
    connect(k->fullScreen, SIGNAL(updateOnionOpacityFromFullScreen(double)), this, SLOT(updateOnionOpacity(double)));
    connect(k->fullScreen, SIGNAL(callAction(int, int)), this, SLOT(loadPlugin(int, int)));
    connect(k->fullScreen, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
    connect(k->fullScreen, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));
    connect(k->fullScreen, SIGNAL(rightClick()), this, SLOT(fullScreenRightClick()));

    connect(k->fullScreen, SIGNAL(goToFrame(int, int, int)), this, SLOT(selectFrame(int, int, int)));
    connect(k->fullScreen, SIGNAL(goToScene(int)), this, SLOT(selectScene(int)));
}

void TupDocumentView::updatePenThickness(int size) 
{
    QPen pen = brushManager()->pen();
    pen.setWidth(size);
    emit updatePenFromFullScreen(pen);
}

void TupDocumentView::updateOnionOpacity(double opacity)
{
    k->paintArea->setOnionFactor(opacity);
    k->onionFactorSpin->setValue(opacity);
}

void TupDocumentView::closeFullScreen()
{
    if (k->fullScreenOn) {
        disconnect(this, SIGNAL(openColorDialog(const QColor &)), k->fullScreen, SLOT(colorDialog(const QColor &)));
        disconnect(k->fullScreen, SIGNAL(updateColorFromFullScreen(const QColor &)), this, SIGNAL(updateColorFromFullScreen(const QColor &)));
        disconnect(k->fullScreen, SIGNAL(updatePenThicknessFromFullScreen(int)), this, SLOT(updatePenThickness(int))); 
        disconnect(k->fullScreen, SIGNAL(updateOnionOpacityFromFullScreen(double)), this, SLOT(updateOnionOpacity(double)));
        disconnect(k->fullScreen, SIGNAL(callAction(int, int)), this, SLOT(loadPlugin(int, int)));
        disconnect(k->fullScreen, SIGNAL(requestTriggered(const TupProjectRequest *)), this, SIGNAL(requestTriggered(const TupProjectRequest *)));
        disconnect(k->fullScreen, SIGNAL(localRequestTriggered(const TupProjectRequest *)), this, SIGNAL(localRequestTriggered(const TupProjectRequest *)));

        disconnect(k->fullScreen, SIGNAL(goToFrame(int, int, int)), this, SLOT(selectFrame(int, int, int)));
        disconnect(k->fullScreen, SIGNAL(goToScene(int)), this, SLOT(selectScene(int)));

        k->fullScreen->close();
        k->fullScreenOn = false;
        k->currentTool->init(k->paintArea->graphicsScene());
        k->fullScreen = 0;
    }
}

void TupDocumentView::selectFrame(int frame, int layer, int scene)
{
    k->paintArea->goToFrame(frame, layer, scene);
}

void TupDocumentView::selectScene(int scene)
{
    k->paintArea->goToScene(scene);
}

void TupDocumentView::exportImage()
{
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
    int frameIndex = k->paintArea->graphicsScene()->currentFrameIndex();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Frame As"), QDir::homePath(),
                                                        tr("Images") + " (*.png *.jpg)");
    if (!fileName.isNull()) {
        bool isOk = k->imagePlugin->exportFrame(frameIndex, k->project->bgColor(), fileName, k->project->scene(sceneIndex), k->project->dimension()); 
        updatePaintArea();
        if (isOk)
            TOsd::self()->display(tr("Information"), tr("Frame has been exported successfully"));
        else
            TOsd::self()->display(tr("Error"), tr("Can't export frame as image"), TOsd::Error);
    }
}

void TupDocumentView::postImage()
{
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();
    int frameIndex = k->paintArea->graphicsScene()->currentFrameIndex();

    TupImageDialog *dialog = new TupImageDialog(this);
    dialog->show();
    QDesktopWidget desktop;
    dialog->move((int) (desktop.screenGeometry().width() - dialog->width())/2 ,
                 (int) (desktop.screenGeometry().height() - dialog->height())/2);

    if (dialog->exec() != QDialog::Rejected) {
        QString title = dialog->imageTitle();
        QString topics = dialog->imageTopics();
        QString description = dialog->imageDescription();
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        emit requestExportImageToServer(frameIndex, sceneIndex, title, topics, description);
    }
}

void TupDocumentView::updateStatusBgColor(const QColor color)
{
    k->status->setBgColor(color);
}

void TupDocumentView::storyboardSettings()
{
    QDesktopWidget desktop;
    int sceneIndex = k->paintArea->graphicsScene()->currentSceneIndex();

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    TupStoryBoardDialog *storySettings = new TupStoryBoardDialog(k->isNetworked, k->imagePlugin, k->project->bgColor(), k->project->dimension(), 
                                                                 k->project->scene(sceneIndex), currentSceneIndex(), this);
    connect(storySettings, SIGNAL(updateStoryboard(TupStoryboard *, int)), this, SLOT(sendStoryboard(TupStoryboard *, int)));

    if (k->isNetworked)
        connect(storySettings, SIGNAL(postStoryboard(int)), this, SIGNAL(postStoryboard(int)));

    QApplication::restoreOverrideCursor();

    storySettings->show();
    storySettings->move((int) (desktop.screenGeometry().width() - storySettings->width())/2 ,
                        (int) (desktop.screenGeometry().height() - storySettings->height())/2);
}

void TupDocumentView::sendStoryboard(TupStoryboard *storyboard, int sceneIndex)
{
    if (k->isNetworked) {
        #ifdef K_DEBUG
               tWarning() << "TupDocumentView::sendStoryboard() - Sending storyboard...";
        #endif
        emit updateStoryboard(storyboard, sceneIndex);
    } else {
        k->project->scene(sceneIndex)->setStoryboard(storyboard);    
    }
}

void TupDocumentView::updateUsersOnLine(const QString &login, int state)
{
    if (state == 1) {
        k->onLineUsers << login; 
    } else {
        int index = k->onLineUsers.indexOf(login);
        k->onLineUsers.removeAt(index); 
    }

    if (k->fullScreenOn)
        k->fullScreen->updateOnLineUsers(k->onLineUsers);
}

void TupDocumentView::setBackgroundDirection(int direction)
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex);
   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->setDyanmicDirection(direction);
           emit projectHasChanged();
       }
   }
}

void TupDocumentView::updateBackgroundShiftProperty(int shift)
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex);
   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->setDyanmicShift(shift);
           emit projectHasChanged();
       }
   }
}

void TupDocumentView::renderDynamicBackground()
{
   int sceneIndex = k->paintArea->currentSceneIndex();
   TupScene *scene = k->project->scene(sceneIndex); 

   if (scene) {
       TupBackground *bg = scene->background();
       if (bg) {
           bg->renderDynamicView();
       }
   }
}

void TupDocumentView::fullScreenRightClick()
{
   if (k->currentTool->name().compare(tr("PolyLine")) == 0)
       emit closePolyLine();
}

void TupDocumentView::cameraInterface()
{
    int camerasTotal = QCamera::availableDevices().count();
    if (camerasTotal > 0) {
        QByteArray cameraDevice = QCamera::availableDevices()[0];
        QCamera *camera = new QCamera(cameraDevice);

        QCameraImageCapture *imageCapture = new QCameraImageCapture(camera);
        QList<QSize> resolutions = imageCapture->supportedResolutions();
        QSize cameraSize = QSize(0, 0);
        for (int i=0; i < resolutions.size(); i++) {
             QSize resolution = resolutions.at(i);
             if (resolution.width() > cameraSize.width()) {
                 cameraSize.setWidth(resolution.width());
                 cameraSize.setHeight(resolution.height());
             }
        }

        if (k->project->dimension() != cameraSize) {
            QDesktopWidget desktop;
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Question"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("The camera resolution is different than your project size."));
            msgBox.setInformativeText(tr("Do you want to adjust your project size?"));

            msgBox.addButton(QString(tr("Adjust it")), QMessageBox::AcceptRole);
            msgBox.addButton(QString(tr("Cancel")), QMessageBox::DestructiveRole);

            msgBox.show();
            msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2 ,
                        (int) (desktop.screenGeometry().height() - msgBox.height())/2);

            int ret = msgBox.exec();

            switch (ret) {
                    case QMessageBox::AcceptRole:
                         resizeProjectDimension(cameraSize);
                    break;
            }
        }

        TupCameraInterface *dialog = new TupCameraInterface(k->project->dimension(), cameraSize, this);
        // connect(dialog, SIGNAL(projectSizeHasChanged(int, int)), this, SLOT(resizeProjectDimension(int, int)));
        dialog->show();
    } else {
        // No devices connected!
        TOsd::self()->display(tr("Error"), tr("No cameras detected"), TOsd::Error);
    }
}

void TupDocumentView::resizeProjectDimension(const QSize dimension)
{
    k->paintArea->updateDimension(dimension);

    int width = k->wsDimension.width();
    int height = k->wsDimension.height();
    int pWidth = dimension.width();
    int pHeight = dimension.height();

    double proportion = 1;

    if (pWidth > pHeight)
        proportion = (double) width / (double) pWidth;
    else
        proportion = (double) height / (double) pHeight;

    if (proportion <= 0.5) {
        setZoomView("20");
    } else if (proportion > 0.5 && proportion <= 0.75) {
               setZoomView("25");
    } else if (proportion > 0.75 && proportion <= 1.5) {
               setZoomView("50");
    } else if (proportion > 1.5 && proportion < 2) {
               setZoomView("75");
    }

    emit projectSizeHasChanged(dimension);
}
