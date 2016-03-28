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

#include "tupproject.h"
#include "tupscene.h"
#include "tuplayer.h"
#include "tupsoundlayer.h"
#include "tupframe.h"
#include "tupbackground.h"
#include "tuplibrary.h"
#include "tuplibraryobject.h"
#include "tupgraphiclibraryitem.h"
#include "tupitemfactory.h"
#include "tupprojectresponse.h"
#include "tupprojectloader.h"

struct TupProject::Private
{
    QString name;
    QString author;
    QColor bgColor;
    QString description;
    QSize dimension;
    int fps;
    QString cachePath; 

    Scenes scenes;
    Scenes undoScenes;
    int sceneCounter;
    TupLibrary *library;
    bool isOpen;
    TupProject::Mode spaceMode;
};

/**
 * Constructor
 */
TupProject::TupProject(QObject *parent) : QObject(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject()]";
        #else
            TINIT;
        #endif
    #endif    
    
    k->spaceMode = TupProject::NONE;
    k->bgColor = QColor("#fff");
    k->sceneCounter = 0;
    k->isOpen = false;
    k->library = new TupLibrary("library", this);
    k->cachePath = "";
}

/**
 * Destructor
 */
TupProject::~TupProject()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupProject()]";
        #else
            TEND;
        #endif
    #endif    
        
    deleteDataDir();
    k->scenes.clear();
    delete k;
}

/**
 * This function loads a local library
 */
void TupProject::loadLibrary(const QString &filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        k->library->fromXml(QString::fromLocal8Bit(file.readAll()));
        file.close();
    } else {               
        #ifdef K_DEBUG
            QString msg = "TupProject::loadLibrary() - Cannot open library from: " + filename;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal("library") << msg;
            #endif
        #endif
    }
}

/**
 * This function closes project 
 */
void TupProject::clear()
{
    k->scenes.clear();
    k->sceneCounter = 0;

    deleteDataDir();
}

/**
 * This function sets project name 
 */
void TupProject::setProjectName(const QString &name)
{
    k->name = name;
}

/**
 * This function sets author name 
 */
void TupProject::setAuthor(const QString &author)
{
    k->author = author;
}

void TupProject::setBgColor(const QColor color)
{
    k->bgColor = color;

    int totalScenes = k->scenes.size();
    for (int i = 0; i < totalScenes; i++) {
         TupScene *scene = k->scenes.at(i);
         scene->setBgColor(color);
    }
}

/**
 * This function sets project description
 */
void TupProject::setDescription(const QString &description)
{
    k->description = description;
}

/**
 * This function sets project dimension
 */
void TupProject::setDimension(const QSize dimension)
{
    k->dimension = dimension;
}

/**
 * This function sets FPS dimension
 */
void TupProject::setFPS(const int fps)
{
    k->fps = fps;
}

void TupProject::setDataDir(const QString &path)
{
    k->cachePath = path;
}

/**
 * Returns project name
 */
QString TupProject::projectName() const
{
    return k->name;
}

/**
 * Returns project name
 */
QString TupProject::author() const
{
    return k->author;
}

QColor TupProject::bgColor() const
{
    return k->bgColor;
}

/**
 * Returns description project
 */
QString TupProject::description() const
{
    return k->description;
}

/**
 * Returns dimension project
 */
QSize TupProject::dimension() const
{
    return k->dimension;
}

/**
 * Returns fps project
 */
int TupProject::fps() const
{
    return k->fps;
}

QString TupProject::dataDir() const
{
    return k->cachePath;
}

TupScene *TupProject::createScene(QString name, int position, bool loaded)
{
    if (position < 0 || position > k->scenes.count())
        return 0;

    TupScene *scene = new TupScene(this, k->dimension, k->bgColor);
    k->scenes.insert(position, scene);
    k->sceneCounter++;
    scene->setSceneName(name);
    
    if (loaded)
        TupProjectLoader::createScene(scene->sceneName(), position, this);

    return scene;
}

void TupProject::updateScene(int position, TupScene *scene)
{
    k->scenes.insert(position, scene);
}

bool TupProject::restoreScene(int position)
{
    if (k->undoScenes.count() > 0) {
        TupScene *scene = k->undoScenes.takeLast();
        if (scene) {
            k->scenes.insert(position, scene);
            k->sceneCounter++;
            return true;
        }
        return false;
    }

    return false;
}

bool TupProject::removeScene(int position)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::removeScene()]";
        #else
            T_FUNCINFO;
        #endif
    #endif        
    
    TupScene *toRemove = sceneAt(position);
    if (toRemove) {
        QString path = dataDir() + "/scene" + QString::number(position) + ".tps";
        if (QFile::exists(path)) {
            if (!QFile::remove(path)) {        
                #ifdef K_DEBUG
                    QString msg = "TupProject::removeScene() - Error removing file " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
            
                return false;
            }
        }

        int total = k->sceneCounter - 1;
        if (position < total) {
            for (int i=position + 1; i<=total; i++) {
                 QString oldName = dataDir() + "/scene" + QString::number(i) + ".tps";  
                 QString newName = dataDir() + "/scene" + QString::number(i-1) + ".tps";
                 QFile::rename(oldName, newName); 
            }
        }

        k->undoScenes << k->scenes.takeAt(position);
        // k->scenes.removeAt(position);
        k->sceneCounter--;

        return true;
    } 

    return false;
}

bool TupProject::moveScene(int position, int newPosition)
{
    if (position < 0 || newPosition < 0) {
        #ifdef K_DEBUG
            QString msg = "TupProject::moveScene() - Failed moving scene from " + QString::number(position) + " to " + QString::number(newPosition);
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return false;
    }

    TupScene *scene = k->scenes.takeAt(position);
    k->scenes.insert(newPosition, scene);

    return true;
}

TupScene *TupProject::sceneAt(int position) const
{    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::sceneAt()] - position: " << position;
        #else
            T_FUNCINFOX("project") << position;
        #endif
    #endif    

    if (position < 0) {
        #ifdef K_DEBUG
            QString msg = "TupProject::sceneAt() - FATAL ERROR: index out of bound (" + QString::number(position) + ")";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return 0;
    }

    return k->scenes.value(position);
}

int TupProject::visualIndexOf(TupScene *scene) const
{
    return k->scenes.indexOf(scene);
}

void TupProject::fromXml(const QString &xml)
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

QDomElement TupProject::toXml(QDomDocument &doc) const
{
    QDomElement tupi = doc.createElement("Tupi");
    tupi.setAttribute("version", "1");

    QDomElement project = doc.createElement("project");
    project.setAttribute("name", k->name);

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

Scenes TupProject::scenes() const
{
    return k->scenes;
}

bool TupProject::createSymbol(int type, const QString &name, const QByteArray &data, const QString &folder)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::createSymbol()]";
        #else
            T_FUNCINFOX("symbol");
        #endif
    #endif
   
    if (!k->isOpen) {        
        #ifdef K_DEBUG
            QString msg = "TupProject::createSymbol() - Fatal error: project is NOT open!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        
        return false;
    }

    if (k->library->createSymbol(TupLibraryObject::Type(type), name, data, folder) == 0) {
        #ifdef K_DEBUG
            QString msg = "TupProject::createSymbol() - Fatal error: object can't be created. Data is NULL!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif    

    } else {        
        #ifdef K_DEBUG
            QString msg = "TupProject::createSymbol() - Object added successfully -> " + name;
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif    
    }

    return true;
}

bool TupProject::removeSymbol(const QString &name, TupLibraryObject::Type type)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::removeSymbol()]";
        #else
            T_FUNCINFO;
        #endif
    #endif    
    
    int totalScenes = k->scenes.size();
    for (int i = 0; i < totalScenes; i++) {
         TupScene *scene = k->scenes.at(i);

         TupBackground *bg = scene->background();
         if (bg) {
             TupFrame *frame = bg->staticFrame();
             if (frame) {
                 if (type != TupLibraryObject::Svg)
                     frame->removeImageItemFromFrame(name);
                 else
                     frame->removeSvgItemFromFrame(name);
             } 

             frame = bg->dynamicFrame();
             if (frame) {
                 if (type != TupLibraryObject::Svg)
                     frame->removeImageItemFromFrame(name);
                 else
                     frame->removeSvgItemFromFrame(name);
             }
         }

         int totalLayers = scene->layers().size();
         for (int j = 0; j < totalLayers; j++) {
              TupLayer *layer = scene->layers().at(j);
              int totalFrames = layer->frames().size();
              for (int t = 0; t < totalFrames; t++) {
                   TupFrame *frame = layer->frames().at(t);
                   if (type != TupLibraryObject::Svg)
                       frame->removeImageItemFromFrame(name);
                   else
                       frame->removeSvgItemFromFrame(name);
              }
         }
    }

    return k->library->removeObject(name, true);
}

bool TupProject::addFolder(const QString &name)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::addFolder()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    TupLibraryFolder *folder = new TupLibraryFolder(name, this);

    return k->library->addFolder(folder);
}

bool TupProject::removeFolder(const QString &name)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::removeFolder()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    return k->library->removeFolder(name);
}

bool TupProject::removeSound(const QString &name)
{
    return k->library->removeObject(name, true);
}

bool TupProject::insertSymbolIntoFrame(TupProject::Mode spaceMode, const QString &name, int sceneIndex, 
                                       int layerIndex, int frameIndex)
{    
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupProject::insertSymbolIntoFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif        
    
    TupFrame *frame = 0;
    TupScene *scene = this->sceneAt(sceneIndex);

    if (scene) {
        if (spaceMode == TupProject::FRAMES_EDITION) {
            TupLayer *layer = scene->layerAt(layerIndex);

            if (layer)
                frame = layer->frameAt(frameIndex);
            else
                return false;
        } else if (spaceMode == TupProject::STATIC_BACKGROUND_EDITION) { 
            TupBackground *bg = scene->background();

            if (bg)
                frame = bg->staticFrame();
            else
                return false;
        } else if (spaceMode == TupProject::DYNAMIC_BACKGROUND_EDITION) {
            TupBackground *bg = scene->background();

            if (bg)
                frame = bg->dynamicFrame();
            else
                return false;
        } else {
            #ifdef K_DEBUG
                QString msg = "TupProject::insertSymbolIntoFrame() - Fatal Error: invalid spaceMode!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif        
            return false;
        }

        if (frame) {
            TupLibraryObject *object = k->library->getObject(name);
            if (object) {
                switch (object->type()) {
                        case TupLibraryObject::Item:
                        {
                             TupGraphicLibraryItem *libraryItem = new TupGraphicLibraryItem(object);
                             int zLevel = frame->getTopZLevel();
                             libraryItem->setZValue(zLevel);
                             frame->addLibraryItem(name, libraryItem);
                        }
                        break;
                        case TupLibraryObject::Image:
                        {
                             TupGraphicLibraryItem *libraryItem = new TupGraphicLibraryItem(object);
                             int imageW = libraryItem->boundingRect().width();
                             int imageH = libraryItem->boundingRect().height();

                             if (k->dimension.width() > imageW && k->dimension.height() > imageH)
                                 libraryItem->moveBy((k->dimension.width() - imageW)/2, (k->dimension.height() - imageH)/2);
                             else
                                 libraryItem->moveBy(0, 0);

                             int zLevel = frame->getTopZLevel();
                             libraryItem->setZValue(zLevel);
                             frame->addItem(name, libraryItem);
                        }
                        break;
                        case TupLibraryObject::Svg:
                        {
                             QString path(object->dataPath());
                             TupSvgItem *svgItem = new TupSvgItem(path, frame);
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

                             int zLevel = frame->getTopZLevel();
                             svgItem->setZValue(zLevel);
                             frame->addSvgItem(name, svgItem);
                        }
                        break;
                        case TupLibraryObject::Text:
                        {
                             // SQA: Just out of curiosity, check if this case really happens!
                             // tFatal() << "TupProject::insertSymbolIntoFrame() - Just tracing text!";
                             TupGraphicLibraryItem *libraryItem = new TupGraphicLibraryItem(object);

                             int zLevel = frame->getTopZLevel();
                             libraryItem->setZValue(zLevel);
                             frame->addItem(name, libraryItem);
                        }
                        break;
                        case TupLibraryObject::Sound:
                        {
                             TupSoundLayer *sound = scene->createSoundLayer(scene->soundLayers().count());
                             sound->fromSymbol(object->symbolName());
                        }
                        break;
                        default:
                             #ifdef K_DEBUG
                                 QString msg = "TupProject::insertSymbolIntoFrame() -> Unknown Object Type";
                                 #ifdef Q_OS_WIN
                                     qDebug() << msg;
                                 #else
                                     tFatal() << msg;
                                 #endif
                             #endif                         
                        break;
                }

                return true;
            } else {                
                #ifdef K_DEBUG
                    QString msg = "TupProject::insertSymbolIntoFrame() - Object NOT found at library! " + name;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                
                return false;
            }
        } else {
                #ifdef K_DEBUG
                    QString msg = "TupProject::insertSymbolIntoFrame() - Invalid frame!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
        }
    } else {        
        #ifdef K_DEBUG
            QString msg = "TupProject::insertSymbolIntoFrame() - Invalid scene!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif        
    }

    return false;
}

bool TupProject::removeSymbolFromFrame(const QString &name, TupLibraryObject::Type type)
{
    #ifdef K_DEBUG
        QString msg = "TupProject::removeSymbolFromFrame() - Removing symbol " + name + " from project...";
        #ifdef Q_OS_WIN
            qDebug() << msg;
        #else
            tFatal() << msg;
        #endif
    #endif    
    
    if (type == TupLibraryObject::Folder)
        return true;

    int totalScenes = k->scenes.size(); 
    for (int i = 0; i < totalScenes; i++) {
         TupScene *scene =  k->scenes.at(i);
         int totalLayers = scene->layers().size();
         for (int j = 0; j < totalLayers; j++) {
              TupLayer *layer = scene->layers().at(j);
              int totalFrames = layer->frames().size(); 
              for (int t = 0; t < totalFrames; t++) {
                   TupFrame *frame = layer->frames().at(t);
                   if (type != TupLibraryObject::Svg)
                       frame->removeImageItemFromFrame(name);
                   else
                       frame->removeSvgItemFromFrame(name);
              }
         }

         TupBackground *bg = scene->background();
         if (bg) {
             TupFrame *frame = bg->staticFrame();
             if (frame) {
                 if (type == TupLibraryObject::Svg)
                     frame->removeSvgItemFromFrame(name);
                 else
                     frame->removeImageItemFromFrame(name);
             }

             frame = bg->dynamicFrame();
             if (frame) {
                 if (type == TupLibraryObject::Svg)
                     frame->removeSvgItemFromFrame(name);
                 else
                     frame->removeImageItemFromFrame(name);
             }
         }
    }

    // k->library->removeObject(name, true);

    return true;
}

bool TupProject::updateSymbolId(TupLibraryObject::Type type, const QString &oldId, const QString &newId)
{
    int scenesCount = k->scenes.size();
    for (int i = 0; i < scenesCount; i++) {
         TupScene *scene = k->scenes.at(i);
         int layersCount = scene->layers().size();
         for (int j = 0; j < layersCount; j++) {
              TupLayer *layer = scene->layers().at(j);
              int framesCount = layer->frames().size();  
              for (int t = 0; t < framesCount; t++) {
                   TupFrame *frame = layer->frames().at(t); 
                   if (type != TupLibraryObject::Svg)
                       frame->updateIdFromFrame(oldId, newId);
                   else 
                       frame->updateSvgIdFromFrame(oldId, newId);
              }
         }

         TupBackground *bg = scene->background();
         if (bg) {
             TupFrame *frame = bg->staticFrame();
             if (frame) {
                 if (type != TupLibraryObject::Svg)
                     frame->updateIdFromFrame(oldId, newId);
                 else
                     frame->updateSvgIdFromFrame(oldId, newId);
             }

             frame = bg->dynamicFrame();
             if (frame) {
                 if (type != TupLibraryObject::Svg)
                     frame->updateIdFromFrame(oldId, newId);
                 else
                     frame->updateSvgIdFromFrame(oldId, newId);
             }
         }
    }

    return true;
}

void TupProject::reloadLibraryItem(TupLibraryObject::Type type, const QString &id, TupLibraryObject *object)
{
    int scenesCount = k->scenes.size();
    for (int i = 0; i < scenesCount; i++) {
         TupScene *scene = k->scenes.at(i);
         int layersCount = scene->layers().size();
         for (int j = 0; j < layersCount; j++) {
              TupLayer *layer = scene->layers().at(j);
              int framesCount = layer->frames().size();
              for (int t = 0; t < framesCount; t++) {
                   TupFrame *frame = layer->frames().at(t);
                   if (type == TupLibraryObject::Svg)
                       frame->reloadSVGItem(id, object);
                   else
                       frame->reloadGraphicItem(id, object->dataPath());
              }
         }

         TupBackground *bg = scene->background();
         if (bg) {
             TupFrame *frame = bg->staticFrame();
             if (frame) {
                 if (type == TupLibraryObject::Svg)
                     frame->reloadSVGItem(id, object);
                 else
                     frame->reloadGraphicItem(id, object->dataPath());
             }

             frame = bg->dynamicFrame();
             if (frame) {
                 if (type == TupLibraryObject::Svg)
                     frame->reloadSVGItem(id, object);
                 else
                     frame->reloadGraphicItem(id, object->dataPath());
             }
         }
    }
}

TupLibrary *TupProject::library()
{
    return k->library;
}

void TupProject::emitResponse(TupProjectResponse *response)
{
    emit responsed(response);
}

void TupProject::setOpen(bool open)
{
    k->isOpen = open;
}

bool TupProject::isOpen()
{
    return k->isOpen;
}

bool TupProject::deleteDataDir()
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
                QString msg = "TupProject::deleteDataDir() - Removing directory -> " + dir.absolutePath();
                #ifdef Q_OS_WIN
                    qWarning() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif

            foreach (QString subdir, QStringList() << "audio" << "video" << "images" << "svg") {
                     if (dir.exists(subdir)) {
                         dir.cd(subdir);
                         foreach (QString file, dir.entryList()) {
                                  QString absolute = dir.absolutePath() + "/" + file;

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
                QString msg = "TupProject::deleteDataDir() - Fatal Error: Can't remove project data directory! -> " + dataDir();
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }

        return true;
    }

    return false;
}

int TupProject::scenesCount() const
{
    return k->scenes.count();
}

void TupProject::updateSpaceContext(TupProject::Mode mode)
{
    k->spaceMode = mode; 
}

TupProject::Mode TupProject::spaceContext()
{
    return k->spaceMode;
}
