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

#include "tupexportwidget.h"

#include "tuppluginselector.h"
#include "tupsceneselector.h"
#include "tupexportmodule.h"
#include "tupvideoproperties.h"

/**
 * This class handles the whole process to export a project into a movie format.
 * The export widget uses a wizard to guide the process: format, escenes, target file
 * @author David Cuadrado
*/

struct TupExportWidget::Private
{
    TupPluginSelector *pluginPage;
    TupSceneSelector *scenesPage;
    TupExportModule *animationExport;
    TupExportModule *imagesArrayExport;
    TupExportModule *animatedImageExport;
    TupVideoProperties *videoProperties;
    const TupProject *project;
    QHash<QString, TupExportInterface *> plugins;
};

TupExportWidget::TupExportWidget(TupProject *project, QWidget *parent, bool isLocal) : TupExportWizard(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExportWidget()]";
        #else
            TINIT;
        #endif
    #endif

    k->project = project;

    if (isLocal) {
        setWindowTitle(tr("Export to Video"));
        setWindowIcon(QIcon(THEME_DIR + "icons/export.png"));

        k->pluginPage = new TupPluginSelector();
        addPage(k->pluginPage);

        k->scenesPage = new TupSceneSelector(this);
        k->scenesPage->setScenes(project->scenes());
        addPage(k->scenesPage);

        k->animationExport = new TupExportModule(project, TupExportWidget::Animation, tr("Export to Video File"), this);
        addPage(k->animationExport);

        k->imagesArrayExport = new TupExportModule(project, TupExportWidget::ImagesArray, tr("Export to Image Sequence"), this);
        addPage(k->imagesArrayExport);

        k->animatedImageExport = new TupExportModule(project, TupExportWidget::AnimatedImage, tr("Export to Animated Image"), this);
        addPage(k->animatedImageExport);

        connect(k->pluginPage, SIGNAL(selectedPlugin(const QString &)), this, SLOT(setExporter(const QString &)));
        connect(k->pluginPage, SIGNAL(animationFormatSelected(int, const QString &)), k->animationExport, SLOT(setCurrentFormat(int, const QString &)));
        connect(k->pluginPage, SIGNAL(imagesArrayFormatSelected(int, const QString &)), k->imagesArrayExport, SLOT(setCurrentFormat(int, const QString &)));
        connect(k->pluginPage, SIGNAL(animatedImageFormatSelected(int, const QString &)), k->animatedImageExport, SLOT(setCurrentFormat(int, const QString &)));

        connect(k->scenesPage, SIGNAL(selectedScenes(const QList<int> &)), k->animationExport, SLOT(setScenesIndexes(const QList<int> &)));
        connect(k->scenesPage, SIGNAL(selectedScenes(const QList<int> &)), k->imagesArrayExport, SLOT(setScenesIndexes(const QList<int> &)));
        connect(k->scenesPage, SIGNAL(selectedScenes(const QList<int> &)), k->animatedImageExport, SLOT(setScenesIndexes(const QList<int> &)));

        loadPlugins();
        k->pluginPage->selectFirstItem();
    } else {
        setWindowTitle(tr("Post Animation in Tupitube"));
        setWindowIcon(QIcon(THEME_DIR + "icons/net_document.png"));

        k->scenesPage = new TupSceneSelector(this);
        k->scenesPage->setScenes(project->scenes());
        addPage(k->scenesPage);

        k->videoProperties = new TupVideoProperties(this);
        addPage(k->videoProperties);

        connect(k->scenesPage, SIGNAL(selectedScenes(const QList<int> &)), k->videoProperties, SLOT(setScenesIndexes(const QList<int> &)));
    }
}

TupExportWidget::~TupExportWidget()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupExportWidget()]";
        #else
            TEND;
        #endif
    #endif
}

void TupExportWidget::loadPlugins()
{
    QList<TupExportInterface *> pluginList;
    foreach (QObject *plugin, TupPluginManager::instance()->formats()) {
             if (plugin) {
                 TupExportInterface *exporter = qobject_cast<TupExportInterface *>(plugin);
                 if (exporter) {
                     int index = -1;
                     if (exporter->key().compare(tr("Video Formats")) == 0)
                         index = 0;
                     if (exporter->key().compare(tr("Open Video Format")) == 0)
                         index = 1;
                     if (exporter->key().compare(tr("Image Sequence")) == 0)
                         index = 2;
                     if (exporter->key().compare(tr("Animated Image")) == 0)
                         index = 3;

                     #if defined(Q_OS_LINUX) || defined(Q_OS_WIN)
                         pluginList.insert(index, exporter);
                     #else
                         if (exporter->key().compare(tr("Open Video Format")) != 0)
                             pluginList.insert(index, exporter);
                     #endif
                 } else {
                     #ifdef K_DEBUG
                         QString msg = "TupExportWidget::loadPlugins() - [ Fatal Error ] - Can't load export plugin";
                         #ifdef Q_OS_WIN
                            qDebug() << msg;
                         #else
                            tError() << msg;
                         #endif
                     #endif
                 }
             }
    }

    for (int i=0; i<pluginList.size(); i++) {
         TupExportInterface *exporter = pluginList.at(i);
         k->pluginPage->addPlugin(exporter->key());
         k->plugins.insert(exporter->key(), exporter);
    }
}

void TupExportWidget::setExporter(const QString &plugin)
{
    if (k->plugins.contains(plugin)) {
        TupExportInterface* currentExporter = k->plugins[plugin];
        k->pluginPage->setFormats(currentExporter->availableFormats());

        if (currentExporter)
            k->animationExport->setCurrentExporter(currentExporter);

        k->imagesArrayExport->setCurrentExporter(currentExporter);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExportWidget::setExporter() - [ Fatal Error ] - Can't load export plugin -> " + plugin;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

QString TupExportWidget::videoTitle() const
{
    return k->videoProperties->title();
}

QString TupExportWidget::videoTopics() const
{
    return k->videoProperties->topics();
}

QString TupExportWidget::videoDescription() const
{
    return k->videoProperties->description();
}

QList<int> TupExportWidget::videoScenes() const
{
    return k->videoProperties->scenesList();
}

bool TupExportWidget::isComplete()
{
    return k->videoProperties->isComplete();
}
