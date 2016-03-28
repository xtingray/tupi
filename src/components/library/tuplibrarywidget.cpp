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

#include "tuplibrarywidget.h"
#include "tuplayer.h"

#define RETURN_IF_NOT_LIBRARY if (!k->library) return;

/**
 * This class defines the Library widget.
 * Here is where events around the Library widget are handled.
 * @author David Cuadrado
*/

struct TupLibraryWidget::Private
{
    Private() : library(0)
    {
       currentFrame.frame = 0;
       currentFrame.layer = 0;
       currentFrame.scene = 0;
    }

    TupLibrary *library;
    TupProject *project;
    TupLibraryDisplay *display;
    TupItemManager *libraryTree;
    int childCount;
    QDir libraryDir;
    QComboBox *itemType;
    int currentPlayerId;
    QString oldId;
    bool renaming;
    bool mkdir;
    bool isNetworked;
    QTreeWidgetItem *lastItemEdited;
    QTreeWidgetItem *currentItemDisplayed;
    QFileSystemWatcher *watcher;
    QList<QString> editorItems;

    struct Frame
    {
       int scene;
       int layer;
       int frame;
    } currentFrame;
};

TupLibraryWidget::TupLibraryWidget(QWidget *parent) : TupModuleWidgetBase(parent), k(new Private)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryWidget()]";
        #else
            TINIT;
        #endif
    #endif

    k->childCount = 0;
    k->renaming = false;
    k->mkdir = false;

    setWindowIcon(QPixmap(THEME_DIR + "icons/library.png"));
    setWindowTitle(tr("Library"));

    k->libraryDir = QDir(CONFIG_DIR + "libraries");
    k->display = new TupLibraryDisplay();
    k->libraryTree = new TupItemManager(this);

    connect(k->libraryTree, SIGNAL(itemSelected(QTreeWidgetItem *)), this,
                                   SLOT(previewItem(QTreeWidgetItem *)));

    connect(k->libraryTree, SIGNAL(itemRemoved()), this,
                                   SLOT(removeCurrentItem()));

    connect(k->libraryTree, SIGNAL(itemCloned(QTreeWidgetItem*)), this,
                                   SLOT(cloneObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemExported(QTreeWidgetItem*)), this,
                                   SLOT(exportObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemRenamed(QTreeWidgetItem*)), this, 
                                   SLOT(renameObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                                   SLOT(refreshItem(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(editorClosed()), this,
                                   SLOT(updateItemEditionState()));

    connect(k->libraryTree, SIGNAL(itemMoved(QString, QString)), this,
                                   SLOT(updateLibrary(QString, QString)));

    connect(k->libraryTree, SIGNAL(itemRequired()), this,
                                   SLOT(insertObjectInWorkspace()));

    connect(k->libraryTree, SIGNAL(itemCreated(QTreeWidgetItem*)), this,
                                   SLOT(activeRefresh(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(inkscapeEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openInkscapeToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(gimpEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openGimpToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(kritaEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openKritaToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(myPaintEditCall(QTreeWidgetItem*)), this,
                                   SLOT(openMyPaintToEdit(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(newRasterCall()), this,
                                   SLOT(createRasterObject()));

    connect(k->libraryTree, SIGNAL(newVectorCall()), this,
                                   SLOT(createVectorObject()));

    QGroupBox *buttons = new QGroupBox(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttons);
    buttonLayout->setMargin(0);
    buttonLayout->setSpacing(0);

    QHBoxLayout *comboLayout = new QHBoxLayout;
    comboLayout->setMargin(0);
    comboLayout->setSpacing(0);

    k->itemType = new QComboBox();
    k->itemType->setIconSize(QSize(15, 15));
    k->itemType->setMaximumWidth(120);
    
    k->itemType->addItem(QIcon(THEME_DIR + "icons/bitmap.png"), tr("Image"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg.png"), tr("Svg File"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/bitmap_array.png"), tr("Image Sequence"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg_array.png"), tr("Svg Sequence"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/sound_object.png"), tr("Sound File"));

    comboLayout->addWidget(k->itemType);

    connect(k->itemType, SIGNAL(currentIndexChanged(int)), this, SLOT(importLibraryObject()));

    TImageButton *addGC = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign.png"), 22, buttons);
    addGC->setToolTip(tr("Add an object to library"));
    connect(addGC, SIGNAL(clicked()), this, SLOT(importLibraryObject()));
    comboLayout->addWidget(addGC);

    buttonLayout->addLayout(comboLayout);

    TImageButton *addFolderGC = new TImageButton(QPixmap(THEME_DIR + "icons/create_folder.png"), 22, buttons);
    connect(addFolderGC, SIGNAL(clicked()), this, SLOT(addFolder()));
    addFolderGC->setToolTip(tr("Create new folder"));
    buttonLayout->addWidget(addFolderGC);
    // SQA: Temporary code
    // addFolderGC->setEnabled(false);

    TImageButton *gctoDrawingArea = new TImageButton(QPixmap(THEME_DIR + "icons/library_to_ws.png"), 22, buttons);
    connect(gctoDrawingArea, SIGNAL(clicked()), this, SLOT(insertObjectInWorkspace()));
    gctoDrawingArea->setToolTip(tr("Insert library item into frame"));
    buttonLayout->addWidget(gctoDrawingArea);

    buttons->setLayout(buttonLayout);

    addChild(k->display);
    addChild(buttons);
    addChild(k->libraryTree);

    k->watcher = new QFileSystemWatcher(this);
    connect(k->watcher, SIGNAL(fileChanged(QString)), this, SLOT(updateItemFromSaveAction()));
}


TupLibraryWidget::~TupLibraryWidget()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[~TupLibraryWidget()]";
        #else
            TEND;
        #endif
    #endif

    delete k;
}

void TupLibraryWidget::resetGUI()
{
    k->library->reset();
    k->display->reset();
    k->libraryTree->cleanUI();
}

void TupLibraryWidget::setLibrary(TupLibrary *library)
{
    k->library = library;
    k->project = library->project(); 
}

void TupLibraryWidget::setNetworking(bool isNetworked)
{
    k->isNetworked = isNetworked;
}

void TupLibraryWidget::addFolder(const QString &folderName)
{
    k->libraryTree->createFolder(folderName);
    k->mkdir = true;
}

void TupLibraryWidget::updateItemEditionState()
{
    if (k->editorItems.count() == 2) {
        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Remove, k->editorItems.at(0), TupLibraryObject::Folder); 
        emit requestTriggered(&request);
    }

    k->editorItems.clear();
}

void TupLibraryWidget::activeRefresh(QTreeWidgetItem *item)
{
    k->mkdir = true;
    refreshItem(item);
}

void TupLibraryWidget::previewItem(QTreeWidgetItem *item)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryWidget::previewItem()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    RETURN_IF_NOT_LIBRARY;

    if (item) {
        k->currentItemDisplayed = item;

        if (item->text(2).length() == 0) {
            k->display->showDisplay();
            QGraphicsTextItem *msg = new QGraphicsTextItem(tr("Directory"));
            k->display->render(static_cast<QGraphicsItem *>(msg));
            return;
        }

        TupLibraryObject *object = k->library->getObject(item->text(1) + "." + item->text(2).toLower());

        if (!object) {
            #ifdef K_DEBUG
                QString msg = "TupLibraryWidget::previewItem() - Fatal Error: Cannot find the object: " + item->text(1) + "." + item->text(2).toLower();
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif

            k->display->showDisplay();
            QGraphicsTextItem *text = new QGraphicsTextItem(tr("No preview available"));
            k->display->render(static_cast<QGraphicsItem *>(text));

            return;
        }

        switch (object->type()) {
                case TupLibraryObject::Svg:
                   {
                     k->display->showDisplay();
                     QGraphicsSvgItem *svg = new QGraphicsSvgItem(object->dataPath()); 
                     k->display->render(static_cast<QGraphicsItem *>(svg));
                   }
                   break;
                case TupLibraryObject::Image:
                case TupLibraryObject::Item:
                   {
                     if (object->data().canConvert<QGraphicsItem *>()) {
                         k->display->showDisplay();
                         k->display->render(qvariant_cast<QGraphicsItem *>(object->data()));

                         /* SQA: Just a test
                         TupSymbolEditor *editor = new TupSymbolEditor;
                         editor->setSymbol(object);
                         emit postPage(editor);
                         */    
                     } 
                   }
                   break;
                case TupLibraryObject::Sound:
                   {
                     k->display->setSoundObject(object->dataPath());
                     k->display->showSoundPlayer();
                   }
                   break;
                default:
                   {
                     #ifdef K_DEBUG
                         QString msg = "TupLibraryWidget::previewItem() - Unknown symbol id: " + QString::number(object->type());
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tError("library") << msg;
                         #endif
                     #endif
                   }
                   break;
        }
    } else {
        QGraphicsTextItem *msg = new QGraphicsTextItem(tr("No preview available"));
        k->display->render(static_cast<QGraphicsItem *>(msg));
    }
}

void TupLibraryWidget::insertObjectInWorkspace()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupLibraryWidget::insertObjectInWorkspace()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->libraryTree->topLevelItemCount() == 0) {
        TOsd::self()->display(tr("Error"), tr("Library is empty!"), TOsd::Error);
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::insertObjectInWorkspace() - Library is empty!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    if (!k->libraryTree->currentItem()) { 
        TOsd::self()->display(tr("Error"), tr("There's no current selection!"), TOsd::Error);
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::insertObjectInWorkspace() - There's no current selection!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    } 

    QString extension = k->libraryTree->currentItem()->text(2);
    if (extension.length() == 0) {
        TOsd::self()->display(tr("Error"), tr("It's a directory! Please, pick a graphic object"), TOsd::Error);
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::insertObjectInWorkspace() - It's a directory!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
        return;
    }

    if ((extension.compare("OGG") == 0) || (extension.compare("WAV") == 0) || (extension.compare("MP3") == 0)) {
        TOsd::self()->display(tr("Error"), tr("It's a sound file! Please, pick a graphic object"), TOsd::Error);
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::insertObjectInWorkspace() - It's a sound file!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tFatal() << msg;
            #endif
        #endif
        return;
    } 

    QString key = k->libraryTree->currentItem()->text(1) + "." + extension.toLower();
    int objectType = k->libraryTree->itemType();
    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::InsertSymbolIntoFrame, key,
                                TupLibraryObject::Type(objectType), k->project->spaceContext(), 
                                0, QString(), k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);

    emit requestTriggered(&request);
}

void TupLibraryWidget::removeCurrentItem()
{
    if (!k->libraryTree->currentItem()) 
        return;

    TCONFIG->beginGroup("General");
    bool ask = TCONFIG->value("ConfirmRemoveObject", true).toBool();

    if (ask) {
        TOptionalDialog dialog(tr("Do you want to remove this object from Library?"), tr("Confirmation"), this);
        dialog.setModal(true);
        QDesktopWidget desktop;
        dialog.move((int) (desktop.screenGeometry().width() - dialog.sizeHint().width())/2,
                    (int) (desktop.screenGeometry().height() - dialog.sizeHint().height())/2);

        if (dialog.exec() == QDialog::Rejected)
            return;

        TCONFIG->beginGroup("General");
        TCONFIG->setValue("ConfirmRemoveObject", dialog.shownAgain());
        TCONFIG->sync();
    }

    QString objectKey = k->libraryTree->currentItem()->text(1);
    QString extension = k->libraryTree->currentItem()->text(2);
    TupLibraryObject::Type type = TupLibraryObject::Folder;

    // If it's NOT a directory
    if (extension.length() > 0) {
        objectKey = k->libraryTree->currentItem()->text(3);
        if (extension.compare("JPEG") == 0 || extension.compare("JPG") == 0 || extension.compare("PNG") == 0 || extension.compare("GIF") == 0)
            type = TupLibraryObject::Image;
        if (extension.compare("SVG")==0)
            type = TupLibraryObject::Svg;
        if (extension.compare("OBJ")==0)
            type = TupLibraryObject::Item;
        if ((extension.compare("OGG") == 0) || (extension.compare("WAV") == 0) || (extension.compare("MP3") == 0))
            type = TupLibraryObject::Sound;
    } 

    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Remove, objectKey, type);
    emit requestTriggered(&request);
}

void TupLibraryWidget::cloneObject(QTreeWidgetItem* item)
{
    if (item) {
        QString id = item->text(3);
        TupLibraryObject *object = k->library->getObject(id);

        if (object) {
            QString smallId = object->smallId();
            QString extension = object->extension();
            TupLibraryObject::Type type = object->type();
            QString path = object->dataPath();
            int limit = path.lastIndexOf("/");
            QString newPath = path.left(limit + 1); 

            QString symbolName = "";

            if (itemNameEndsWithDigit(smallId)) {
                int index = getItemNameIndex(smallId);
                symbolName = nameForClonedItem(smallId, extension, index, newPath);
                newPath += symbolName;
            } else {
                symbolName = nameForClonedItem(smallId, extension, newPath);
                newPath += symbolName;
            }

            QString baseName = symbolName.section('.', 0, 0);
            baseName = verifyNameAvailability(baseName, extension, true);
            symbolName = baseName + "." + extension.toLower();

            bool isOk = QFile::copy(path, newPath);

            if (!isOk) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::cloneObject() - Fatal Error: Object file couldn't be cloned!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }

            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(type);
            newObject->setDataPath(newPath);
            isOk = newObject->loadData(newPath);

            if (!isOk) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::cloneObject() - Fatal Error: Object file couldn't be loaded!";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            } 

            k->library->addObject(newObject);

            QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
            item->setText(1, baseName);
            item->setText(2, extension);
            item->setText(3, symbolName);
            item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            switch (object->type()) {
                    case TupLibraryObject::Item:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/drawing_object.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Image:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Svg:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));
                             k->libraryTree->setCurrentItem(item);
                             previewItem(item);
                         }
                         break;
                    case TupLibraryObject::Sound:
                         {
                             item->setIcon(0, QIcon(THEME_DIR + "icons/sound_object.png"));
                             previewItem(item);
                         }
                         break;
                    default:
                         {
                         }
                         break;
            }

        } else {
            #ifdef K_DEBUG
                QString msg = "TupLibraryWidget::cloneObject() - Fatal Error: Object doesn't exist! [ " + id + " ]";
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

void TupLibraryWidget::exportObject(QTreeWidgetItem *item)
{
    QString id = item->text(3);
    TupLibraryObject *object = k->library->getObject(id);
    if (object) {
        QString path = object->dataPath();
        if (path.length() > 0) {
            QString fileExtension = object->extension();
            QString filter;

            if (object->type() == TupLibraryObject::Image) {
                filter = tr("Images") + " ";
                if (fileExtension.compare("PNG") == 0)
                    filter += "(*.png)"; 
                if ((fileExtension.compare("JPG") == 0) || (fileExtension.compare("JPEG") == 0))
                    filter += "(*.jpg *.jpeg)";
                if (fileExtension.compare("GIF") == 0)
                    filter += "(*.gif)";
                if (fileExtension.compare("XPM") == 0)
                    filter += "(*.xpm)";
                if (fileExtension.compare("SVG") == 0)
                    filter += "(*.svg)";
            } else if (object->type() == TupLibraryObject::Sound) {
                      filter = tr("Sounds") + " ";
                      if (fileExtension.compare("OGG") == 0)
                          filter += "(*.ogg)";
                      if (fileExtension.compare("MP3") == 0)
                          filter += "(*.mp3)";
                      if (fileExtension.compare("WAV") == 0)
                          filter += "(*.wav)";
            }

            QString target = QFileDialog::getSaveFileName(this, tr("Export object..."), QDir::homePath(), filter);
            if (target.isEmpty())
                return;

            if (QFile::exists(target)) {
                if (!QFile::remove(target)) {
                    #ifdef K_DEBUG
                        QString msg = "TupLibraryWidget::exportObject() - Fatal Error: destination path already exists! [ " + id + " ]";
                        #ifdef Q_OS_WIN
                            qDebug() << msg;
                        #else
                            tError() << msg;
                        #endif
                    #endif
                    return;
                }
            }

            if (!QFile::copy(path, target)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::exportObject() - Error: Object file couldn't be exported! [ " + id + " ]";
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            } else {
                TOsd::self()->display(tr("Info"), tr("Item exported successfully!"), TOsd::Info);
            }
        } else {
            #ifdef K_DEBUG
                QString msg = "TupLibraryWidget::exportObject() - Error: Object path is null! [ " + id + " ]";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
            return;
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::exportObject() - Error: Object doesn't exist! [ " + id + " ]";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }
}

void TupLibraryWidget::renameObject(QTreeWidgetItem *item)
{
    if (item) {
        k->renaming = true;
        k->oldId = item->text(1);
        k->libraryTree->editItem(item, 1);
    }
}

void TupLibraryWidget::createRasterObject()
{
    QString name = "object00";
    QString extension = "PNG";
    name = verifyNameAvailability(name, extension, true);

    QSize size = k->project->dimension();
    int w = QString::number(size.width()).length();
    int h = QString::number(size.height()).length();

    int width = 1;
    int height = 1; 
    for(int i=0; i<w; i++)
        width *= 10;
    for(int i=0; i<h; i++)
        height *= 10;

    size = QSize(width, height);

    TupNewItemDialog dialog(name, TupNewItemDialog::Raster, size);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.itemName();
        QSize size = dialog.itemSize();
        QColor background = dialog.background();
        QString extension = dialog.itemExtension();
        QString editor = dialog.software();

        QString imagesDir = k->project->dataDir() + "/images/";
        if (!QFile::exists(imagesDir)) {
            QDir dir;
            if (!dir.mkpath(imagesDir)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createRasterObject() - Fatal Error: Couldn't create directory " + imagesDir;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                TOsd::self()->display(tr("Error"), tr("Couldn't create images directory!"), TOsd::Error);
                return;
            }
        }
             
        QString path = imagesDir + name + "." + extension.toLower();
        QString symbolName = name; 
        if (QFile::exists(path)) {
            symbolName = nameForClonedItem(name, extension, imagesDir);
            path = imagesDir + symbolName + "." + extension.toLower();
        }

        symbolName += "." + extension.toLower();

        QImage::Format format = QImage::Format_RGB32;
        if (extension.compare("PNG")==0)
            format = QImage::Format_ARGB32;

        QImage *image = new QImage(size, format); 
        image->fill(background);

        bool isOk = image->save(path);

        if (isOk) {
            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(TupLibraryObject::Image);
            newObject->setDataPath(path);
            isOk = newObject->loadData(path);

            if (isOk) {
                k->library->addObject(newObject);

                QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
                item->setText(1, name);
                item->setText(2, extension);
                item->setText(3, symbolName);
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
                item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                k->libraryTree->setCurrentItem(item);

                previewItem(item);

                k->lastItemEdited = item;
                executeSoftware(editor, path);
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createRasterObject() - Fatal Error: Object file couldn't be loaded from -> " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
        } else {
            #ifdef K_DEBUG
                QString msg = "TupLibraryWidget::createRasterObject() - Fatal Error: Object file couldn't be saved at -> " + path;
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

void TupLibraryWidget::createVectorObject()
{
    QString name = "object00";
    QString extension = "SVG";
    name = verifyNameAvailability(name, extension, true);

    QSize size = k->project->dimension();
    int w = QString::number(size.width()).length();
    int h = QString::number(size.height()).length();

    int width = 1;
    int height = 1;
    for(int i=0; i<w; i++) 
        width *= 10;
    for(int i=0; i<h; i++) 
        height *= 10;

    size = QSize(width, height);

    TupNewItemDialog dialog(name, TupNewItemDialog::Vector, size);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.itemName();
        QSize size = dialog.itemSize();
        QString extension = dialog.itemExtension();
        QString editor = dialog.software();

        QString vectorDir = k->project->dataDir() + "/svg/";
        if (!QFile::exists(vectorDir)) {
            QDir dir;
            if (!dir.mkpath(vectorDir)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: Couldn't create directory " + vectorDir;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif

                TOsd::self()->display(tr("Error"), tr("Couldn't create vector directory!"), TOsd::Error);
                return;
            }
        }

        QString path = vectorDir + name + "." + extension.toLower();
        QString symbolName = name;
        if (QFile::exists(path)) {
            symbolName = nameForClonedItem(name, extension, vectorDir);
            path = vectorDir + symbolName + "." + extension.toLower();
        }

        symbolName += "." + extension.toLower();

        QSvgGenerator generator;
        generator.setFileName(path);
        generator.setSize(size);
        generator.setViewBox(QRect(0, 0, size.width(), size.height()));
        generator.setTitle(name);
        generator.setDescription(tr("Tupi library item"));
        QPainter painter;
        painter.begin(&generator);
        bool isOk = painter.end();

        if (isOk) {
            QDomDocument doc;
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: SVG file couldn't be opened -> " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
            if (!doc.setContent(&file)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: SVG file couldn't be parsed as XML -> " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
            file.close();

            QDomNodeList roots = doc.elementsByTagName("svg");
            QDomElement root = roots.at(0).toElement(); 
            root.setAttribute("width", size.width());
            root.setAttribute("height", size.height());
            if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: SVG file couldn't be updated -> " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            } 
            QByteArray xml = doc.toByteArray();
            file.write(xml);
            file.close();

            TupLibraryObject *newObject = new TupLibraryObject();
            newObject->setSymbolName(symbolName);
            newObject->setType(TupLibraryObject::Svg);
            newObject->setDataPath(path);
            isOk = newObject->loadData(path);

            if (isOk) {
                k->library->addObject(newObject);
                QTreeWidgetItem *item = new QTreeWidgetItem(k->libraryTree);
                item->setText(1, name);
                item->setText(2, extension);
                item->setText(3, symbolName);
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
                item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));

                k->libraryTree->setCurrentItem(item);
                previewItem(item);

                k->lastItemEdited = item;
                executeSoftware(editor, path);
            } else {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: Object file couldn't be loaded from -> " + path;
                    #ifdef Q_OS_WIN
                        qDebug() << msg;
                    #else
                        tError() << msg;
                    #endif
                #endif
                return;
            }
        } else {
                #ifdef K_DEBUG
                    QString msg = "TupLibraryWidget::createVectorObject() - Fatal Error: Object file couldn't be saved at -> " + path;
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

void TupLibraryWidget::importBitmapGroup()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Import images..."), QDir::homePath(),
                                                      tr("Images") + " (*.png *.xpm *.jpg *.jpeg *.gif)");
    for (int i = 0; i < files.size(); ++i)
         importBitmap(files.at(i));
}

void TupLibraryWidget::importBitmap(const QString &image)
{
    if (image.isEmpty())
        return;

    QFile f(image);
    QFileInfo fileInfo(f);

    QString key = fileInfo.fileName().toLower();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        QPixmap *pixmap = new QPixmap(image);
        int picWidth = pixmap->width();
        int picHeight = pixmap->height();
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();

        #ifdef K_DEBUG
            #ifdef Q_OS_WIN
               qDebug() << "TupLibraryWidget::importBitmap() - Image filename: " << key << " | Raw Size: " << data.size();
               qDebug() << "TupLibraryWidget::importBitmap() - Image Size: " << "[" << picWidth << ", " << picHeight << "]"
                        << " | Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";
            #else
               tFatal() << "TupLibraryWidget::importBitmap() - Image filename: " << key << " | Raw Size: " << data.size();
               tFatal() << "TupLibraryWidget::importBitmap() - Image Size: " << "[" << picWidth << ", " << picHeight << "]"
                        << " | Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";
            #endif
        #endif

        if (picWidth > projectWidth || picHeight > projectHeight) {
            QDesktopWidget desktop;
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Information"));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("Image is bigger than workspace."));
            msgBox.setInformativeText(tr("Do you want to resize it?"));
            msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.show();
            msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2,
                        (int) (desktop.screenGeometry().height() - msgBox.height())/2);

            int answer = msgBox.exec();

            if (answer == QMessageBox::Yes) {
                pixmap = new QPixmap();
                QString extension = fileInfo.suffix().toUpper();
                // QByteArray ba = extension.toAscii();
                QByteArray ba = extension.toLatin1();
                const char* ext = ba.data();
                if (pixmap->loadFromData(data, ext)) {
                    QPixmap newpix;
                    if (picWidth > projectWidth)
                        newpix = QPixmap(pixmap->scaledToWidth(projectWidth, Qt::SmoothTransformation));
                    else
                        newpix = QPixmap(pixmap->scaledToHeight(projectHeight, Qt::SmoothTransformation));
                    QBuffer buffer(&data);
                    buffer.open(QIODevice::WriteOnly);
                    newpix.save(&buffer, ext);
                }
            } 
        }

        int i = 0;
        int index = key.lastIndexOf(".");
        QString name = key.mid(0, index);
        QString extension = key.mid(index, key.length() - index);
        while (k->library->exists(key)) {
               i++;
               key = name + "-" + QString::number(i) + extension;
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, key,
                                                                            TupLibraryObject::Image, k->project->spaceContext(), data, QString(), 
                                                                            k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);

        data.clear();
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(image), TOsd::Error);
    }
}

void TupLibraryWidget::importSvgGroup()
{
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Import SVG files..."), QDir::homePath(),
                                                      tr("Vector") + " (*.svg)");
    for (int i = 0; i < files.size(); ++i)
         importSvg(files.at(i));
}

void TupLibraryWidget::importSvg(const QString &svgPath)
{
    if (svgPath.isEmpty())
        return;

    QFile file(svgPath);
    QFileInfo fileInfo(file);

    QString key = fileInfo.fileName().toLower();

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        #ifdef K_DEBUG
            QString msg1 = "TupLibraryWidget::importSvg() - Inserting SVG into project: " + k->project->projectName();
            int projectWidth = k->project->dimension().width();
            int projectHeight = k->project->dimension().height();
            QString msg2 = "TupLibraryWidget::importSvg() - Project Size: [" + QString::number(projectWidth) + QString(", ") + QString::number(projectHeight) + QString("]"); 

            #ifdef Q_OS_WIN
                qDebug() << msg1;
                qDebug() << msg2;
            #else
                tFatal() << msg1;
                tFatal() << msg2;
            #endif
        #endif


        int i = 0;
        int index = key.lastIndexOf(".");
        QString name = key.mid(0, index);
        QString extension = key.mid(index, key.length() - index);
        while (k->library->exists(key)) {
               i++;
               key = name + "-" + QString::number(i) + extension;
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, key,
                                                       TupLibraryObject::Svg, k->project->spaceContext(), data, QString(), 
                                                       k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(svgPath), TOsd::Error);
    }
}

void TupLibraryWidget::verifyFramesAvailability(int filesTotal)
{
    TupScene *scene = k->project->sceneAt(k->currentFrame.scene);
    TupLayer *layer = scene->layerAt(k->currentFrame.layer);
    int framesTotal = layer->framesCount();
    int initFrame = k->currentFrame.frame;
    int scope = initFrame + filesTotal;
    if (scope > framesTotal) {
        // int limit = scope - framesTotal;
        for (int i=framesTotal; i<scope; i++) {
             TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer,
                                                                               i, TupProjectRequest::Add, tr("Frame %1").arg(i+1));
             emit requestTriggered(&request);
        }
        TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, initFrame,
                                                                          TupProjectRequest::Select);
        emit requestTriggered(&request);
    }
}

void TupLibraryWidget::importBitmapSequence()
{
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the images directory..."), dir,
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir source(path); 
    QFileInfoList photograms = source.entryInfoList(QDir::Files, QDir::Name);
    int filesTotal = photograms.size();

    // Ensuring to get only graphic files here. Check extensions! (PNG, JPG, GIF, XPM) 
    int imagesCounter = 0; 
    for (int i = 0; i < filesTotal; ++i) {
         if (photograms.at(i).isFile()) {
             QString extension = photograms.at(i).suffix().toUpper();
             if (extension.compare("JPEG")==0 || extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 || 
                 extension.compare("XPM")==0)
                 imagesCounter++;
         }
    }

    if (imagesCounter > 0) {
        QString text = tr("Image files found: %1.").arg(imagesCounter);
        bool resize = false;

        QPixmap *pixmap = new QPixmap(photograms.at(0).absoluteFilePath());
        int picWidth = pixmap->width();
        int picHeight = pixmap->height(); 
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();

        if (picWidth > projectWidth || picHeight > projectHeight) {
            text = text + "\n" + tr("Files are too big, so they will be resized.") + "\n"
                   + tr("Note: This task can take a while.");
            resize = true;
        }

        QDesktopWidget desktop;
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Information"));  
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(text);
        msgBox.setInformativeText(tr("Do you want to continue?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2, 
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int answer = msgBox.exec();

        if (answer == QMessageBox::Ok) {
            verifyFramesAvailability(filesTotal);

            QString directory = source.dirName();
            k->libraryTree->createFolder(directory);

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QFont font = this->font();
            font.setPointSize(8);

            QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint 
                                                 | Qt::Dialog);
            progressDialog.setFont(font);
            progressDialog.setLabelText(tr("Loading images..."));
            progressDialog.setCancelButton(0);
            progressDialog.setRange(1, filesTotal);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            int initFrame = k->currentFrame.frame;
            for (int i = 0; i < filesTotal; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("JPEG")==0 || extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 ||
                         extension.compare("XPM")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName().toLower();
                         QFile f(path);
                         QFileInfo fileInfo(f);

                         if (f.open(QIODevice::ReadOnly)) {
                             QByteArray data = f.readAll();
                             f.close();

                             if (resize) {
                                 pixmap = new QPixmap();
                                 QString extension = fileInfo.suffix().toUpper();
                                 // QByteArray ba = extension.toAscii();
                                 QByteArray ba = extension.toLatin1();
                                 const char* ext = ba.data();
                                 if (pixmap->loadFromData(data, ext)) {
                                     int width = projectWidth;
                                     QPixmap newpix(pixmap->scaledToWidth(width, Qt::SmoothTransformation));
                                     QBuffer buffer(&data);
                                     buffer.open(QIODevice::WriteOnly);
                                     newpix.save(&buffer, ext);
                                 }
                             }
                           
                             TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                         TupLibraryObject::Image, k->project->spaceContext(), data, directory);
                             emit requestTriggered(&request);
                             if (i < filesTotal-1) {
                                 request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame + 1,
                                                                                 TupProjectRequest::Select);
                                 emit requestTriggered(&request);
                             }

                             progressDialog.setLabelText(tr("Loading image #%1").arg(index));
                             progressDialog.setValue(index);
                             index++;
                         } else {
                             QMessageBox::critical(this, tr("ERROR!"), tr("ERROR: Can't open file %1. Please, check file permissions and try again.").arg(symName), QMessageBox::Ok);
                             QApplication::restoreOverrideCursor();
                             return;
                         }
                     }
                 }
            }
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, initFrame,
                                                                              TupProjectRequest::Select);
            emit requestTriggered(&request);

            QApplication::restoreOverrideCursor();
        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No image files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSvgSequence() 
{
    QDesktopWidget desktop;
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the SVG files directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir source(path); 
    QFileInfoList photograms = source.entryInfoList(QDir::Files, QDir::Name);
    int filesTotal = photograms.size();

    // Ensuring to get only SVG files here. Check extension! (SVG)
    int svgCounter = 0;
    for (int i = 0; i < filesTotal; ++i) {
         if (photograms.at(i).isFile()) {
             QString extension = photograms.at(i).suffix().toUpper();
             if (extension.compare("SVG")==0)
                 svgCounter++;
         }
    }

    if (svgCounter > 0) {
        QString testFile = photograms.at(0).absoluteFilePath();
        QFile file(testFile);
        file.close();

        QString text = tr("%1 SVG files will be loaded.").arg(svgCounter);

        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Information"));  
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(text);
        msgBox.setInformativeText(tr("Do you want to continue?"));
        msgBox.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.show();
        msgBox.move((int) (desktop.screenGeometry().width() - msgBox.width())/2, 
                    (int) (desktop.screenGeometry().height() - msgBox.height())/2);

        int answer = msgBox.exec();

        if (answer == QMessageBox::Ok) {
            verifyFramesAvailability(filesTotal);
            QString directory = source.dirName();
            k->libraryTree->createFolder(directory);

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QFont font = this->font();
            font.setPointSize(8);

            QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog);
            progressDialog.setFont(font);
            progressDialog.setLabelText(tr("Loading SVG files..."));
            progressDialog.setCancelButton(0);
            progressDialog.setRange(1, filesTotal);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            int initFrame = k->currentFrame.frame;
            for (int i = 0; i < filesTotal; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("SVG")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName().toLower();
                         QFile f(path);

                         if (f.open(QIODevice::ReadOnly)) {
                             QByteArray data = f.readAll();
                             f.close();

                             TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                                            TupLibraryObject::Svg, k->project->spaceContext(), data, directory);
                             emit requestTriggered(&request);
                             if (i < filesTotal-1) {
                                 request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame + 1,
                                                                                 TupProjectRequest::Select);
                                 emit requestTriggered(&request);
                             }

                             progressDialog.setLabelText(tr("Loading SVG file #%1").arg(index));
                             progressDialog.setValue(index);
                             index++;

                         } else {
                             QMessageBox::critical(this, tr("ERROR!"), tr("ERROR: Can't open file %1. Please, check file permissions and try again.").arg(symName), QMessageBox::Ok);
                             QApplication::restoreOverrideCursor();
                             return;
                         }
                     }
                 }
            }
            TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, initFrame,
                                                                              TupProjectRequest::Select);
            emit requestTriggered(&request);
            QApplication::restoreOverrideCursor();
        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No SVG files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSound()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Import audio file..."), QDir::homePath(),
                                                 tr("Sound file") + " (*.ogg *.wav *.mp3)");

    if (path.isEmpty()) 
        return;

    QFile file(path);
    QFileInfo fileInfo(file);
    QString key = fileInfo.fileName().toLower();

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, key,
                                                     TupLibraryObject::Sound, k->project->spaceContext(), data);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Error while opening file: %1").arg(path), TOsd::Error);
    }
}

void TupLibraryWidget::sceneResponse(TupSceneResponse *response)
{
    switch (response->action()) {
            case TupProjectRequest::Select:
            {
                 k->currentFrame.frame = 0;
                 k->currentFrame.layer = 0;
                 k->currentFrame.scene = response->sceneIndex();
            }
            break;
    }
}

void TupLibraryWidget::libraryResponse(TupLibraryResponse *response)
{
    RETURN_IF_NOT_LIBRARY;

    switch (response->action()) {
            case TupProjectRequest::Add:
              {
                 if (response->symbolType() == TupLibraryObject::Folder) {
                     k->libraryTree->createFolder(response->arg().toString());
                     return;
                 }

                 QString folderName = response->parent(); 
                 QString id = response->arg().toString();

                 int index = id.lastIndexOf(".");
                 QString name = id.mid(0, index);
                 QString extension = id.mid(index + 1, id.length() - index).toUpper();
                 TupLibraryObject *obj = k->library->getObject(id);

                 if (index < 0)
                     extension = "OBJ"; 

                 QTreeWidgetItem *item;
                 if (folderName.length() > 0 && folderName.compare("library")!=0)
                     item = new QTreeWidgetItem(k->libraryTree->getFolder(folderName));
                 else
                     item = new QTreeWidgetItem(k->libraryTree);

                 item->setText(1, name);
                 item->setText(2, extension);
                 item->setText(3, id);
                 item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

                 if (obj) {
                     switch (obj->type()) {
                            case TupLibraryObject::Item:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/drawing_object.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                               }
                            break;
                            case TupLibraryObject::Image:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/bitmap.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                                 // if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE && !k->library->loadingProject())
                               
                                 if (!k->isNetworked && !folderName.endsWith(".pgo") && !k->library->loadingProject())
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Svg:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                                 // if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE && !k->library->loadingProject())
                                 if (!k->isNetworked && !k->library->loadingProject())
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Sound:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/sound_object.png"));
                               }
                            break;
                            default:
                               {
                               }
                            break;
                     }

                 } else {
                     #ifdef K_DEBUG
                         QString msg = "TupLibraryWidget::libraryResponse() - No object found: " + id;
                         #ifdef Q_OS_WIN
                             qDebug() << msg;
                         #else
                             tFatal() << msg;
                         #endif
                     #endif
                 }
              }
            break;
            case TupProjectRequest::InsertSymbolIntoFrame:
              {
                 #ifdef K_DEBUG
                     QString msg = "TupLibraryWidget::libraryResponse() -> InsertSymbolIntoFrame : No action taken";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
              }
            break;
            case TupProjectRequest::RemoveSymbolFromFrame:
              {
                 #ifdef K_DEBUG
                     QString msg = "TupLibraryWidget::libraryResponse() -> RemoveSymbolFromFrame : No action taken";
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
              }
            break;
            case TupProjectRequest::Remove:
              {
                 QString id = response->arg().toString();
                 QTreeWidgetItemIterator it(k->libraryTree);
                 while ((*it)) {
                        // If target is NOT a folder
                        if ((*it)->text(2).length() > 0) {
                            if (id == (*it)->text(3)) {
                                delete (*it);
                                break;
                            }
                        } else {
                            // If target is a folder
                            if (id == (*it)->text(1)) {
                                delete (*it);
                                break;
                            }
                        }
                        ++it;
                 }

                 if (k->libraryTree->topLevelItemCount() > 0) {
                     previewItem(k->libraryTree->currentItem());
                 } else  {
                     k->display->showDisplay();
                     k->display->reset();
                 }
              }
            break;
            default:
              {
                 #ifdef K_DEBUG
                     QString msg = "TupLibraryWidget::libraryResponse() - Unknown/Unhandled project action: " + QString::number(response->action());
                     #ifdef Q_OS_WIN
                         qDebug() << msg;
                     #else
                         tFatal() << msg;
                     #endif
                 #endif
              }
            break;
    }
}

void TupLibraryWidget::frameResponse(TupFrameResponse *response)
{
    if (response->action() == TupProjectRequest::Select) {
        k->currentFrame.frame = response->frameIndex();
        k->currentFrame.layer = response->layerIndex();
        k->currentFrame.scene = response->sceneIndex();
    }
}

void TupLibraryWidget::importLibraryObject()
{
    QString option = k->itemType->currentText();

    if (option.compare(tr("Image")) == 0) {
        importBitmapGroup();
        return;
    }

    if (option.compare(tr("Image Sequence")) == 0) {
        importBitmapSequence();
        return;
    }

    if (option.compare(tr("Svg File")) == 0) {
        importSvgGroup();
        return;
    }

    if (option.compare(tr("Svg Sequence")) == 0) {
        importSvgSequence();
        return;
    }

    if (option.compare(tr("Sound File")) == 0) {
        importSound();
        return;
    }
}

void TupLibraryWidget::refreshItem(QTreeWidgetItem *item)
{
    if (k->mkdir) {
        k->mkdir = false;

        QString base = item->text(1);
        if (base.length() == 0)
            return;

        QString tag = base;
        int i = 0;
        while (k->library->folderExists(tag)) {
               int index = tag.lastIndexOf("-");
               if (index < 0) {
                   tag = base + "-1";
               } else {
                   QString name = base.mid(0, index);
                   i++;
                   tag = name + "-" + QString::number(i);
               }
        }

        item->setText(1, tag);

        TupLibraryFolder *folder = new TupLibraryFolder(tag, k->project);
        k->library->addFolder(folder);

        QGraphicsTextItem *msg = new QGraphicsTextItem(tr("Directory"));
        k->display->render(static_cast<QGraphicsItem *>(msg));

        k->editorItems << tag;

        return;
    }

    if (k->renaming) {
        // Renaming directory
        if (k->libraryTree->isFolder(item)) {
            QString base = item->text(1);
            if (k->oldId.length() == 0 || base.length() == 0)
                return;

            if (k->oldId.compare(base) == 0)
                return;

            int i = 0;
            QString tag = base;
            while (k->library->folderExists(tag)) {
                   int index = tag.lastIndexOf("-");
                   if (index < 0) {
                       tag = base + "-1";
                   } else {
                       QString name = base.mid(0, index);
                       i++;
                       tag = name + "-" + QString::number(i);
                   }
            }

            if (!k->library->folderExists(tag)) {
                // rename directory here!
                if (k->library->folderExists(k->oldId)) {
                    bool renamed = k->library->renameFolder(k->oldId, tag);
                    if (renamed)
                        item->setText(1, tag);
                }
            } 
        } else {
            // Renaming item
            if (k->oldId.length() == 0)
                return;

            QString newId = item->text(1);
            QString extension = item->text(2);

            if (k->oldId.compare(newId) != 0) {
                newId = verifyNameAvailability(newId, extension, false);
                QString oldId = k->oldId + "." + extension.toLower();
                item->setText(1, newId);

                newId = newId + "." + extension.toLower();
                item->setText(3, newId);

                QTreeWidgetItem *parent = item->parent();
                if (parent) 
                    k->library->renameObject(parent->text(1), oldId, newId);
                else
                    k->library->renameObject("", oldId, newId);

                TupLibraryObject::Type type = TupLibraryObject::Image;
                if (extension.compare("SVG")==0)
                    type = TupLibraryObject::Svg;
                if (extension.compare("OBJ")==0)
                    type = TupLibraryObject::Item;

                k->project->updateSymbolId(type, oldId, newId);
            }
        }

        k->renaming = false;
    }
}

void TupLibraryWidget::updateLibrary(QString node, QString target) 
{
    if (target.length() > 0)
        k->library->moveObject(node, target);
    else
        k->library->moveObjectToRoot(node);
}

void TupLibraryWidget::openInkscapeToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, "Inkscape");
}

void TupLibraryWidget::openGimpToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, "Gimp");
}

void TupLibraryWidget::openKritaToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, "Krita");
}

void TupLibraryWidget::openMyPaintToEdit(QTreeWidgetItem *item)
{
    callExternalEditor(item, "MyPaint");
}

void TupLibraryWidget::callExternalEditor(QTreeWidgetItem *item, const QString &software)
{
    if (item) {
        k->lastItemEdited = item;
        QString id = item->text(1) + "." + item->text(2).toLower();
        TupLibraryObject *object = k->library->getObject(id);

        if (object) {
            QString path = object->dataPath();
            executeSoftware(software, path);
        } else {
            #ifdef K_DEBUG
                QString msg = "TupLibraryWidget::callExternalEditor() - Fatal Error: No object related to the current library item -" + id + "- was found!";
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
            #endif
        }
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::callExternalEditor() - Error: Current library item is invalid!";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupLibraryWidget::executeSoftware(const QString &software, QString &path)
{
    if (path.length() > 0 && QFile::exists(path)) {
        QString program = "/usr/bin/" + software.toLower(); 

        QStringList arguments;
        arguments << path;

        QProcess *editor = new QProcess(this);
        editor->start(program, arguments);

        // SQA: Check the path list and if it doesn't exist yet, then add it to 
        k->watcher->addPath(path);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupLibraryWidget::executeSoftware() - Fatal Error: Item path either doesn't exist or is empty";
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupLibraryWidget::updateItemFromSaveAction()
{
    LibraryObjects collection = k->library->objects();
    QMapIterator<QString, TupLibraryObject *> i(collection);
    while (i.hasNext()) {
           i.next();
           TupLibraryObject *object = i.value();
           if (object) {
               updateItem(object->smallId(), object->extension().toLower(), object);
           } else {
               #ifdef K_DEBUG
                   QString msg = "TupLibraryWidget::updateItemFromSaveAction() - Fatal Error: The library item modified was not found!";
                   #ifdef Q_OS_WIN
                       qDebug() << msg;
                   #else
                       tError() << msg;
                   #endif
               #endif
           }
    }

    TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame,
                                                                      TupProjectRequest::Select);
    emit requestTriggered(&request);
}

void TupLibraryWidget::updateItem(const QString &name, const QString &extension, TupLibraryObject *object)
{
    QString onEdition = name + "." + extension;
    QString onDisplay = k->currentItemDisplayed->text(1) + "." + k->currentItemDisplayed->text(2).toLower();

    TupLibraryObject::Type type = TupLibraryObject::Image;
    if (extension.compare("svg") == 0)
        type = TupLibraryObject::Svg;

    bool isOk = k->library->reloadObject(onEdition);
    if (isOk) 
        k->project->reloadLibraryItem(type, onEdition, object);

    if (onDisplay.compare(onEdition) == 0)
        previewItem(k->lastItemEdited);
}

bool TupLibraryWidget::itemNameEndsWithDigit(QString &name)
{
    QByteArray array = name.toLocal8Bit();

    QChar letter(array.at(array.size() - 1));
    if (letter.isDigit())
        return true;

    return false;
}

int TupLibraryWidget::getItemNameIndex(QString &name) const
{
    QByteArray array = name.toLocal8Bit();
    int index = 0;
    for (int i = array.size()-1; i >= 0; i--) {
         QChar letter(array.at(i));
         if (!letter.isDigit()) {
             index = i + 1;
             break;
         }
    }

    return index;
}

QString TupLibraryWidget::nameForClonedItem(QString &name, QString &extension, int index, QString &path) const
{
    QString symbolName = "";

    QString base = name.left(index); 
    int counter = name.right(index).toInt();

    while (true) {
           counter++;
           QString number = QString::number(counter);
           if (counter < 10)
               number = "0" + number; 
           symbolName = base + number + "." + extension.toLower();
           QString tester = path + symbolName;
           if (!QFile::exists(tester))
               break;
    }

    return symbolName;
}

QString TupLibraryWidget::nameForClonedItem(QString &smallId, QString &extension, QString &path) const
{
    QString symbolName = "";
    int index = 0;

    while(true) {
          QString number = QString::number(index);
          if (index < 10)
              number = "0" + number;

          QString base = smallId + number;
          symbolName = base + "." + extension.toLower();
          QString tester = path + symbolName;

          if (!QFile::exists(tester))
              break;

          index++;
    }

    return symbolName;
}

QString TupLibraryWidget::verifyNameAvailability(QString &name, QString &extension, bool isCloningAction) {

    int limit = 1;
    if (isCloningAction)
        limit = 0; 

    QList<QTreeWidgetItem *> list = k->libraryTree->findItems(name, Qt::MatchExactly, 1);
    if (list.size() > limit) {
        int total = 0;
        for (int i=0; i<list.size(); i++) {
             QTreeWidgetItem *node = list.at(i);
             if (node->text(2).compare(extension) == 0)
                 total++;
        }

        if (total > limit) {
            bool ok = false;
            if (itemNameEndsWithDigit(name)) {
                int index = getItemNameIndex(name);
                QString base = name.left(index);
                int counter = name.right(name.length() - index).toInt(&ok);
                if (ok) {
                    while (true) {
                           counter++;
                           QString number = QString::number(counter);
                           if (counter < 10)
                               number = "0" + number;
                           name = base + number;
                           QList<QTreeWidgetItem *> others = k->libraryTree->findItems(name, Qt::MatchExactly, 1);
                           if (others.size() == 0)
                               break;
                    }
                } else {
                    name = TAlgorithm::randomString(8);
                    #ifdef K_DEBUG
                        QString msg = "TupLibraryWidget::verifyNameAvailability() - Warning: error while processing item name!";
                        #ifdef Q_OS_WIN
                            qWarning() << msg;
                        #else
                            tWarning() << msg;
                        #endif
                    #endif
                }
            } else {
                int index = name.lastIndexOf("-");
                if (index < 0) {
                    name += "-1";
                } else {
                    QString first = name.mid(0, index);
                    QString last = name.mid(index+1, name.length() - index);
                    int newIndex = last.toInt(&ok);
                    if (ok) {
                        newIndex++;
                        name = first + "-" + QString::number(newIndex);
                    } else {
                        name = TAlgorithm::randomString(8);
                        #ifdef K_DEBUG
                            QString msg = "TupLibraryWidget::verifyNameAvailability() - Warning: error while processing item name!";
                            #ifdef Q_OS_WIN
                                qWarning() << msg;
                            #else
                                tWarning() << msg;
                            #endif
                        #endif
                    }
                }
            }
        }
    }

    return name;
}
