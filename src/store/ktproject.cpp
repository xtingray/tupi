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

#include "ktproject.h"
#include "ktscene.h"
#include "ktlayer.h"
#include "ktsoundlayer.h"
#include "ktframe.h"
#include "ktbackground.h"
#include "ktlibrary.h"
#include "ktlibraryobject.h"
#include "ktgraphiclibraryitem.h"
#include "ktitemfactory.h"
#include "ktprojectresponse.h"
#include "ktprojectloader.h"

#include "tdebug.h"

#include <QDir>
#include <QGraphicsView>

struct KTProject::Private
{
    QString name;
    QString author;
    QColor bgColor;
    QString description;
    QSize dimension;
    int fps;
    QString cachePath; 

    Scenes scenes;
    int sceneCounter;
    KTLibrary *library;
    bool isOpen;
    KTProject::Mode spaceMode;
};

/**
 * Constructor
 */
KTProject::KTProject(QObject *parent) : QObject(parent), k(new Private)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    k->spaceMode = KTProject::NONE;
    k->bgColor = QColor("#fff");
    k->sceneCounter = 0;
    k->isOpen = false;
    k->library = new KTLibrary("library", this);
    k->cachePath = "";
}

/**
 * Destructor
 */
KTProject::~KTProject()
{
    #ifdef K_DEBUG
           TEND;
    #endif

    deleteDataDir();
    k->scenes.clear(true);
    delete k;
}

/**
 * This function loads a local library
 */
void KTProject::loadLibrary(const QString &filename)
{
    QFile lfile(filename);

    if (lfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        k->library->fromXml(QString::fromLocal8Bit(lfile.readAll()));
        lfile.close();
    } else {
        #ifdef K_DEBUG
               tFatal("library") << "Cannot open library from: " << filename;
        #endif
    }
}

/**
 * This function closes project 
 */
void KTProject::clear()
{
    k->scenes.clear(true);
    k->sceneCounter = 0;

    deleteDataDir();
}

/**
 * This function sets project name 
 */
void KTProject::setProjectName(const QString &name)
{
    tError() << "KTProject::setProjectName() - k->name: " << name;
    k->name = name;
}

/**
 * This function sets author name 
 */
void KTProject::setAuthor(const QString &author)
{
    k->author = author;
}

void KTProject::setBgColor(const QColor color)
{
    k->bgColor = color;
}

/**
 * This function sets project description
 */
void KTProject::setDescription(const QString &description)
{
    k->description = description;
}

/**
 * This function sets project dimension
 */
void KTProject::setDimension(const QSize dimension)
{
    k->dimension = dimension;
}

/**
 * This function sets FPS dimension
 */
void KTProject::setFPS(const int fps)
{
    k->fps = fps;
}

void KTProject::setDataDir(const QString &path)
{
    k->cachePath = path;
}

/**
 * Returns project name
 */
QString KTProject::projectName() const
{
    return k->name;
}

/**
 * Returns project name
 */
QString KTProject::author() const
{
    return k->author;
}

QColor KTProject::bgColor() const
{
    return k->bgColor;
}

/**
 * Returns description project
 */
QString KTProject::description() const
{
    return k->description;
}

/**
 * Returns dimension project
 */
QSize KTProject::dimension() const
{
    return k->dimension;
}

/**
 * Returns fps project
 */
int KTProject::fps() const
{
    return k->fps;
}

QString KTProject::dataDir() const
{
    return k->cachePath;
}

KTScene *KTProject::createScene(QString name, int position, bool loaded)
{
    if (position < 0 || position > k->scenes.count())
        return 0;

    KTScene *scene = new KTScene(this);

    k->scenes.insert(position, scene);
    k->sceneCounter++;

    // scene->setSceneName(tr("Scene %1").arg(k->sceneCounter));
    scene->setSceneName(name);
    
    if (loaded)
        KTProjectLoader::createScene(scene->sceneName(), position, this);

    return scene;
}

void KTProject::updateScene(int position, KTScene *scene)
{
    k->scenes.insert(position, scene);
}

bool KTProject::removeScene(int position)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    KTScene *toRemove = scene(position);

    if (toRemove) {
        k->scenes.removeObject(position);
        delete toRemove;
        toRemove = 0;
        k->sceneCounter--;

        #ifdef K_DEBUG
               QList<int> list = k->scenes.indexes();
               QString test = "";
               for (int i = 0; i < list.size(); ++i) {
                    test += QString::number(list.at(i));
                    if (i < (list.size()-1))
                        test += " ";
               }
               tDebug() << "KTProject::removeScene() - Current scene indexes: " << test;
        #endif

        return true;
    } 

    return false;
}

bool KTProject::moveScene(int position, int newPosition)
{
    // if (position < 0 || position >= k->scenes.count() || newPosition < 0 || newPosition >= k->scenes.count()) {

    if (position < 0 || newPosition < 0) {
        #ifdef K_DEBUG
               tError() << "KTProject::moveScene() - Failed moving scene from " << position << " to " << newPosition; 
        #endif
        return false;
    }

    KTScene *scene = k->scenes.takeObject(position);
    k->scenes.insert(newPosition, scene);

    return true;
}

KTScene *KTProject::scene(int position) const
{
    #ifdef K_DEBUG
           T_FUNCINFOX("project")<< position;
    #endif

    // if (position < 0 || position >= k->scenes.count()) {

    if (position < 0) {
        #ifdef K_DEBUG
               tError() << "KTProject::scene() - FATAL ERROR: index out of bound (" << position << ")";
        #endif
        return 0;
    }

    return k->scenes.value(position);
}

int KTProject::visualIndexOf(KTScene *scene) const
{
    return k->scenes.objectIndex(scene);
}

/*
int KTProject::logicalIndexOf(KTScene *scene) const
{
    return k->scenes.logicalIndex(scene);
}
*/

void KTProject::fromXml(const QString &xml)
{
    QDomDocument document;

    if (!document.setContent(xml))
        return;

    QDomElement root = document.documentElement();
    QDomNode n = root.firstChild();

    int i = 0;
    while (!n.isNull()) {
           QDomElement e = n.toElement();

           if (!e.isNull()) {
               if (e.tagName() == "project") {
                   setProjectName(e.attribute("name", projectName()));
                   QDomNode n1 = e.firstChild();
                   e = n1.toElement();

                   if (e.tagName() == "meta") {

                       QDomNode n1 = e.firstChild();

                       while (!n1.isNull()) {

                              QDomElement e1 = n1.toElement();

                              if (e1.tagName() == "author") {

                                  if (e1.firstChild().isText()) 
                                      setAuthor(e1.text());

                              } else if (e1.tagName() == "bgcolor") {
                                         if (e1.text().isEmpty())
                                             setBgColor(QColor("#ffffff"));
                                         else
                                             setBgColor(QColor(e1.text()));

                              } else if (e1.tagName() == "description") {

                                         if (e1.firstChild().isText())
                                             setDescription(e1.text());

                                } else if (e1.tagName() == "dimension") {

                                           if (e1.firstChild().isText()) {
                                               QStringList list = e1.text().split(",");
                                               int x = list.at(0).toInt();
                                               int y = list.at(1).toInt();
                                               QSize size(x,y);
                                               setDimension(size);
                                           }

                                } else if (e1.tagName() == "fps") {

                                           if (e1.firstChild().isText())
                                               setFPS(e1.text().toInt());

                                }

                                n1 = n1.nextSibling();
                          }
                   }
               }
           } 
           n = n.nextSibling();
           i++;
    }
}

QDomElement KTProject::toXml(QDomDocument &doc) const
{
    QDomElement tupi = doc.createElement("Tupi");
    tupi.setAttribute("version", "1");

    QDomElement project = doc.createElement("project");
    project.setAttribute("name", k->name);
    //project.setAttribute("name", projectName);

    QDomElement meta = doc.createElement("meta");

    QDomElement author = doc.createElement("author");
    author.appendChild(doc.createTextNode(k->author));

    QDomElement description = doc.createElement("description");
    description.appendChild(doc.createTextNode(k->description));

    QDomElement color = doc.createElement("bgcolor");
    color.appendChild(doc.createTextNode(k->bgColor.name()));

    QDomElement size = doc.createElement("dimension");
    QString xy = QString::number(k->dimension.width()) + "," + QString::number(k->dimension.height());
    size.appendChild(doc.createTextNode(xy));

    QDomElement fps = doc.createElement("fps");
    QString frames = QString::number(k->fps);
    fps.appendChild(doc.createTextNode(frames));

    meta.appendChild(author);
    meta.appendChild(color);
    meta.appendChild(description);
    meta.appendChild(size);
    meta.appendChild(fps);

    project.appendChild(meta);
    tupi.appendChild(project);
    
    return tupi;
}

Scenes KTProject::scenes() const
{
    return k->scenes;
}

bool KTProject::createSymbol(int type, const QString &name, const QByteArray &data, const QString &folder)
{
    if (!k->isOpen) {
        #ifdef K_DEBUG
               tError() << "KTProject::createSymbol() - Fatal error: project is NOT open!";
        #endif
        return false;
    }

    if (k->library->createSymbol(KTLibraryObject::Type(type), name, data, folder) == 0) {
        #ifdef K_DEBUG
               tError() << "KTProject::createSymbol() - Fatal error: object can't be created. Data is NULL!";
        #endif
    } else {
        #ifdef K_DEBUG
               tWarning() << "KTProject::createSymbol() - Object added successfully -> " << name;
        #endif
    }

    return true;
}

bool KTProject::removeSymbol(const QString &name, KTLibraryObject::Type symbolType, KTProject::Mode spaceMode, 
                             int sceneIndex, int layerIndex, int frameIndex)
{
    Q_UNUSED(name);

    KTFrame *frame = 0;
    KTScene *scene = this->scene(sceneIndex);

    if (scene) {

        if (spaceMode == KTProject::FRAMES_EDITION) {

            KTLayer *layer = scene->layer(layerIndex);
            if (layer) {
                frame = layer->frame(frameIndex);
                if (frame) {
                    if (symbolType == KTLibraryObject::Svg) {
                        QList<int> indexes = frame->svgIndexes();
                        int lastIndex = indexes.at(indexes.size()-1);
                        if (frame->removeSvgAt(lastIndex))
                            return true;
                    } else {
                        QList<int> indexes = frame->itemIndexes();
                        int lastIndex = indexes.at(indexes.size()-1);
                        if (frame->removeGraphicAt(lastIndex))
                            return true;
                    }
                }
            }
        } else if (spaceMode == KTProject::BACKGROUND_EDITION) {

            KTBackground *bg = scene->background();

            if (bg) {
                KTFrame *frame = bg->frame();
                if (frame) {
                    if (symbolType == KTLibraryObject::Svg) {
                        QList<int> indexes = frame->svgIndexes();
                        int lastIndex = indexes.at(indexes.size()-1);
                        if (frame->removeSvgAt(lastIndex))
                            return true;
                    } else {
                        QList<int> indexes = frame->itemIndexes();
                        int lastIndex = indexes.at(indexes.size()-1);
                        if (frame->removeGraphicAt(lastIndex))
                            return true;
                    }
                }
            }
        } else {
            #ifdef K_DEBUG
                   tError() << "KTProject::removeSymbol() - spaceMode invalid!";
            #endif
        }
    }

    return false;
}

bool KTProject::removeSymbol(const QString &name)
{
    return k->library->removeObject(name, true);
}

bool KTProject::addSymbolToProject(KTProject::Mode spaceMode, const QString &name, int sceneIndex, 
                                   int layerIndex, int frameIndex)
{
    KTFrame *frame = 0;
    KTScene *scene = this->scene(sceneIndex);

    if (scene) {

        if (spaceMode == KTProject::FRAMES_EDITION) {
            KTLayer *layer = scene->layer(layerIndex);

            if (layer)
                frame = layer->frame(frameIndex);
            else
                return false;
        } else if (spaceMode == KTProject::BACKGROUND_EDITION) { 
            KTBackground *bg = scene->background();

            if (bg)
                frame = bg->frame();
            else
                return false;
        } else {
            #ifdef K_DEBUG
                   tError() << "KTProject::addSymbolToProject() - spaceMode invalid!";
            #endif
            
            return false;
        }

        if (frame) {

            KTLibraryObject *object = k->library->findObject(name);

            if (object) {

                switch (object->type()) {
                        case KTLibraryObject::Image:
                        {
                             KTGraphicLibraryItem *libraryItem = new KTGraphicLibraryItem(object);
                             int imageW = libraryItem->boundingRect().width();
                             int imageH = libraryItem->boundingRect().height();

                             if (k->dimension.width() > imageW && k->dimension.height() > imageH)
                                 libraryItem->moveBy((k->dimension.width() - imageW)/2, (k->dimension.height() - imageH)/2);
                             else
                                 libraryItem->moveBy(0, 0);
           
                             tError() << "KTProject::addSymbolToProject() - Adding image item: " << name;
                             frame->addItem(name, libraryItem);
                        }
                        break;
                        case KTLibraryObject::Text:
                        {
                             // SQA: Just out of curiosity, check if this case really happens!
                             // tFatal() << "KTProject::addSymbolToProject() - Just tracing text!";
                             KTGraphicLibraryItem *libraryItem = new KTGraphicLibraryItem(object);
                             frame->addItem(name, libraryItem);
                        }
                        break;
                        case KTLibraryObject::Svg:
                        {
                             QString path(object->dataPath());
                             KTSvgItem *svgItem = new KTSvgItem(path, frame);
                             svgItem->setSymbolName(name);

                             int svgW = svgItem->boundingRect().width();
                             int svgH = svgItem->boundingRect().height();

                             if (k->dimension.width() > svgW && k->dimension.height() > svgH) {
                                 svgItem->moveBy((k->dimension.width() - svgW)/2, (k->dimension.height() - svgH)/2);
                             } else {
                                 qreal factorW = ((qreal)k->dimension.width())/((qreal)svgW);
                                 qreal factorH = ((qreal)k->dimension.height())/((qreal)svgH);

                                 if (factorW < factorH)                           
                                     svgItem->setScale(factorW);
                                 else
                                     svgItem->setScale(factorH);

                                 svgItem->moveBy(0, 0);
                             }

                             frame->addSvgItem(name, svgItem);
                        }
                        break;
                        case KTLibraryObject::Item:
                        {
                             KTGraphicLibraryItem *libraryItem = new KTGraphicLibraryItem(object);
                             frame->addItem(name, libraryItem);
                        }
                        break;
                        case KTLibraryObject::Sound:
                        {
                             KTSoundLayer *sound = scene->createSoundLayer(scene->soundLayers().count());
                             sound->fromSymbol(object->symbolName());
                        }
                        break;
                        default:
                             #ifdef K_DEBUG
                                    tFatal() << "KTProject::addSymbolToProject() -> Unknown Object Type"; 
                             #endif
                        break;
                }

                return true;

            } else {
                #ifdef K_DEBUG 
                       tError() << "KTProject::addSymbolToProject() - Object NOT found at library! " << name;
                #endif
                return false;
            }
        } else {
                #ifdef K_DEBUG
                       tError() << "KTProject::addSymbolToProject() - Invalid frame!";
                #endif
        }

    } else {
        #ifdef K_DEBUG
               tError() << "KTProject::addSymbolToProject() - Invalid scene!";
        #endif
    }

    return false;
}

bool KTProject::removeSymbolFromProject(const QString &name, KTLibraryObject::Type type)
{
    if (type == KTLibraryObject::Folder)
        return true;

    foreach (KTScene *scene, k->scenes.values()) {
             foreach (KTLayer *layer, scene->layers().values()) {
                      foreach (KTFrame *frame, layer->frames().values()) {
                               if (type != KTLibraryObject::Svg)
                                   frame->removeItemFromFrame(name);
                               else
                                   frame->removeSvgItemFromFrame(name);
                      }
             }

             KTBackground *bg = scene->background();
             if (bg) {
                 KTFrame *frame = bg->frame();
                 if (frame) {
                     if (type != KTLibraryObject::Svg)
                         frame->removeItemFromFrame(name);
                     else
                         frame->removeSvgItemFromFrame(name);
                 }
             }
    }

    return true;
}

bool KTProject::updateSymbolId(KTLibraryObject::Type type, const QString &oldId, const QString &newId)
{
    foreach (KTScene *scene, k->scenes.values()) {

             foreach (KTLayer *layer, scene->layers().values()) {
                      foreach (KTFrame *frame, layer->frames().values()) {
                               if (type != KTLibraryObject::Svg)
                                   frame->updateIdFromFrame(oldId, newId);
                               else 
                                   frame->updateSvgIdFromFrame(oldId, newId);
                      }
             }

             KTBackground *bg = scene->background();
             if (bg) {
                 KTFrame *frame = bg->frame();
                 if (frame) {
                     if (type != KTLibraryObject::Svg)
                         frame->updateIdFromFrame(oldId, newId);
                     else
                         frame->updateSvgIdFromFrame(oldId, newId);
                 }
             }
    }

    return true;
}

KTLibrary *KTProject::library()
{
    return k->library;
}

void KTProject::emitResponse(KTProjectResponse *response)
{
    emit responsed(response);
}

void KTProject::setOpen(bool open)
{
    k->isOpen = open;
}

bool KTProject::isOpen()
{
    return k->isOpen;
}

bool KTProject::deleteDataDir()
{
    if (QFile::exists(dataDir()) && !k->name.isEmpty()) {
        QDir dir(dataDir());

        if (dir.exists("project.tpp")) {
            dir.remove("project.tpp");
            dir.remove("library.tpl");
            foreach (QString scene, dir.entryList(QStringList() << "scene*.tps", QDir::Files))
                     dir.remove(scene);
        }

        if (dir.exists("audio") || dir.exists("video") || dir.exists("images") || dir.exists("svg")) {

            #ifdef K_DEBUG
                   tWarning("project") << "KTProject::deleteDataDir() - Removing directory -> " << dir.absolutePath(); 
            #endif

            foreach (QString subdir, QStringList() << "audio" << "video" << "images" << "svg") {
                     if (dir.exists(subdir)) {
                         dir.cd(subdir);
                         foreach (QString file, dir.entryList()) {
                                  QString absolute = dir.absolutePath() + QDir::separator() + file;

                                  if (!file.startsWith(".")) {
                                      QFileInfo finfo(absolute);
                                      if (finfo.isFile())
                                          QFile::remove(absolute);
                                  }
                          }
                          dir.cdUp();
                          dir.rmdir(subdir);
                     }
            }
        }

        if (! dir.rmdir(dir.absolutePath())) {
            #ifdef K_DEBUG
                   tError("project") << "Cannot remove project data directory!";
            #endif
        }

        return true;
    }

    return false;
}

int KTProject::scenesTotal() const
{
    return k->scenes.count();
}

void KTProject::updateSpaceContext(KTProject::Mode mode)
{
    k->spaceMode = mode; 
}

KTProject::Mode KTProject::spaceContext()
{
    return k->spaceMode;
}
