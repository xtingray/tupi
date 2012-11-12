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

#include "tuplibrarywidget.h"
#include "tglobal.h"
#include "tapplication.h"
#include "toptionaldialog.h"
#include "tconfig.h"
#include "tuplibrary.h"
#include "tupproject.h"
#include "tuplibraryobject.h"
#include "tupsymboleditor.h"
#include "tuprequestbuilder.h"
#include "tosd.h"
// #include "taudioplayer.h"
#include "tdebug.h"

#include <QApplication>
#include <QGroupBox>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QBuffer>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QComboBox>
#include <QTreeWidgetItemIterator>

#include <cstdlib>
#include <ctime>

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
    TupItemPreview *display;
    TupGCTable *libraryTree;
    int childCount;
    QDir libraryDir;
    QComboBox *itemType;
    int currentPlayerId;
    QString oldId;
    bool renaming;
    bool mkdir;
    bool isNetworked;

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
           TINIT;
    #endif

    k->childCount = 0;
    k->renaming = false;
    k->mkdir = false;

    setWindowIcon(QPixmap(THEME_DIR + "icons/library.png"));
    setWindowTitle(tr("Library"));

    k->libraryDir = QDir(CONFIG_DIR + "libraries");
    k->display = new TupItemPreview(this);
    k->libraryTree = new TupGCTable(this);

    connect(k->libraryTree, SIGNAL(itemSelected(QTreeWidgetItem *)), this,
                                   SLOT(previewItem(QTreeWidgetItem *)));

    connect(k->libraryTree, SIGNAL(itemRemoved()), this,
                                   SLOT(removeCurrentGraphic()));

    connect(k->libraryTree, SIGNAL(itemRenamed(QTreeWidgetItem*)), this, 
                                   SLOT(renameObject(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
                                   SLOT(refreshItem(QTreeWidgetItem*)));

    connect(k->libraryTree, SIGNAL(itemMoved(QString, QString)), this,
                                   SLOT(updateLibrary(QString, QString)));

    connect(k->libraryTree, SIGNAL(itemCreated(QTreeWidgetItem*)), this,
                                   SLOT(activeRefresh(QTreeWidgetItem*)));

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
    k->itemType->addItem(QIcon(THEME_DIR + "icons/bitmap_array.png"), tr("Image Array"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg.png"), tr("Svg File"));
    k->itemType->addItem(QIcon(THEME_DIR + "icons/svg_array.png"), tr("Svg Array"));
    comboLayout->addWidget(k->itemType);

    connect(k->itemType, SIGNAL(currentIndexChanged(int)), this, SLOT(importGraphicObject()));

    TImageButton *addGC = new TImageButton(QPixmap(THEME_DIR + "icons/plus_sign.png"), 22, buttons);
    addGC->setToolTip(tr("Add an object to library"));
    connect(addGC, SIGNAL(clicked()), this, SLOT(importGraphicObject()));
    comboLayout->addWidget(addGC);

    buttonLayout->addLayout(comboLayout);

    TImageButton *addFolderGC = new TImageButton(QPixmap(THEME_DIR + "icons/create_folder.png"), 22, buttons);
    connect(addFolderGC, SIGNAL(clicked()), this, SLOT(addFolder()));
    addFolderGC->setToolTip(tr("Adds a folder to the object's list"));
    buttonLayout->addWidget(addFolderGC);
    // SQA: Temporary code
    // addFolderGC->setEnabled(false);

    TImageButton *gctoDrawingArea = new TImageButton(QPixmap(THEME_DIR + "icons/library_to_ws.png"), 22, buttons);
    connect(gctoDrawingArea, SIGNAL(clicked()), this, SLOT(insertObjectInWorkspace()));
    gctoDrawingArea->setToolTip(tr("Inserts the selected object into the drawing area"));
    buttonLayout->addWidget(gctoDrawingArea);

    buttons->setLayout(buttonLayout);

    addChild(k->display);
    addChild(buttons);
    addChild(k->libraryTree);
}


TupLibraryWidget::~TupLibraryWidget()
{
    #ifdef K_DEBUG
           TEND;
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

void TupLibraryWidget::addFolder()
{
    k->libraryTree->createFolder();
    k->mkdir = true;
}

void TupLibraryWidget::activeRefresh(QTreeWidgetItem *item)
{
    k->mkdir = true;
    refreshItem(item);
}

void TupLibraryWidget::previewItem(QTreeWidgetItem *item)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    RETURN_IF_NOT_LIBRARY;

    if (item) {

        if (item->text(2).length() == 0) {
            QGraphicsTextItem *msg = new QGraphicsTextItem(tr("Directory"));
            k->display->render(static_cast<QGraphicsItem *>(msg));
            return;
        }

        TupLibraryObject *object = k->library->findObject(item->text(1) + "." + item->text(2).toLower());

        if (!object) {
            #ifdef K_DEBUG
                   tError() << "TupLibraryWidget::previewItem() - Cannot find the object: " << item->text(1) + "." + item->text(2).toLower();
            #endif
            QGraphicsTextItem *msg = new QGraphicsTextItem(tr("No preview available"));
            k->display->render(static_cast<QGraphicsItem *>(msg));

            return;
        }

        switch (object->type()) {
                case TupLibraryObject::Svg:
                   {
                     QGraphicsSvgItem *svg = new QGraphicsSvgItem(object->dataPath()); 
                     k->display->render(static_cast<QGraphicsItem *>(svg)); 
                   }
                   break;
                case TupLibraryObject::Image:
                case TupLibraryObject::Item:
                   {
                     if (object->data().canConvert<QGraphicsItem *>()) {

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
                     /*
                     TAudioPlayer::instance()->setCurrentPlayer(k->currentPlayerId);
                     TAudioPlayer::instance()->stop();

                     k->currentPlayerId = TAudioPlayer::instance()->load(object->dataPath());
                     TAudioPlayer::instance()->play(0);
                     */
                   }
                   break;
                default:
                   {
                     #ifdef K_DEBUG
                            tDebug("library") << "Unknown symbol id: " << object->type();
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
           T_FUNCINFO;
    #endif

    if (!k->libraryTree->currentItem()) { 
        #ifdef K_DEBUG
               tFatal() << "TupLibraryWidget::insertObjectInWorkspace() - There's no current selection!";
        #endif
        return;
    } else if (k->libraryTree->currentItem()->text(2).length() == 0) {
               #ifdef K_DEBUG
                      tFatal() << "TupLibraryWidget::insertObjectInWorkspace() - It's a directory!";
               #endif
               return;
    }

    QString objectKey = k->libraryTree->currentItem()->text(1) + "." + k->libraryTree->currentItem()->text(2).toLower();

    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::InsertSymbolIntoFrame, objectKey,
                                TupLibraryObject::Type(k->libraryTree->currentItem()->data(1, 3216).toInt()), k->project->spaceContext(), 
                                0, QString(), k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);

    emit requestTriggered(&request);
}

void TupLibraryWidget::removeCurrentGraphic()
{
    if (!k->libraryTree->currentItem()) 
        return;

    QString extension = k->libraryTree->currentItem()->text(2);

    QString objectKey = k->libraryTree->currentItem()->text(1);
    TupLibraryObject::Type type = TupLibraryObject::Folder;

    // If it's NOT a directory
    if (extension.length() > 0) {
        objectKey = k->libraryTree->currentItem()->text(3);
        if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0)
            type = TupLibraryObject::Image;
        if (extension.compare("SVG")==0)
            type = TupLibraryObject::Svg;
        if (extension.compare("OBJ")==0)
            type = TupLibraryObject::Item;
    } 

    TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::RemoveSymbolFromFrame, 
                                                   objectKey, type, k->project->spaceContext(), 0, QString(),
                                                   k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
    emit requestTriggered(&request);
}

void TupLibraryWidget::renameObject(QTreeWidgetItem* item)
{
    if (item) {
        k->renaming = true;
        k->oldId = item->text(1);
        k->libraryTree->editItem(item, 1);
    }
}

void TupLibraryWidget::importBitmap()
{
    QString image = QFileDialog::getOpenFileName (this, tr("Import an image..."), QDir::homePath(),  
                                                  tr("Images") + " (*.png *.xpm *.jpg *.jpeg *.gif)");
    if (image.isEmpty()) 
        return;

    QFile f(image);
    QFileInfo fileInfo(f);

    QString symName = fileInfo.fileName();


    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        QPixmap *pixmap = new QPixmap(image);
        int picWidth = pixmap->width();
        int picHeight = pixmap->height();
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();

        #ifdef K_DEBUG
               tFatal() << "TupLibraryWidget::importBitmap() - Image filename: " << symName << " | Raw Size: " << data.size();
               tFatal() << "TupLibraryWidget::importBitmap() - Image Size: " << "[" << picWidth << ", " << picHeight << "]" << " | Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";
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
                QByteArray ba = extension.toAscii();
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
        QString tag = symName;
        TupLibraryObject *object = k->library->findObject(tag);
        while (object) {
               i++;
               int index = symName.lastIndexOf(".");
               QString name = symName.mid(0, index);
               QString extension = symName.mid(index, symName.length() - index);
               tag = name + "-" + QString::number(i) + extension;
               object = k->library->findObject(tag);
        }

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                                                          TupLibraryObject::Image, k->project->spaceContext(), data, QString(), 
                                                                          k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);

        data.clear();
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(image), TOsd::Error);
    }
}

void TupLibraryWidget::importSvg()
{
    QString svgPath = QFileDialog::getOpenFileName (this, tr("Import a SVG file..."), QDir::homePath(),
                                                    tr("Vectorial") + " (*.svg)");
    if (svgPath.isEmpty())
        return;

    QFile f(svgPath);
    QFileInfo fileInfo(f);

    QString symName = fileInfo.fileName();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        // SQA: This block is just for debugging!   
        tFatal() << "TupLibraryWidget::importSvg() - Inserting SVG into project: " << k->project->projectName();
        int projectWidth = k->project->dimension().width();
        int projectHeight = k->project->dimension().height();
        tFatal() << "TupLibraryWidget::importSvg() - Project Size: " << "[" << projectWidth << ", " << projectHeight << "]";

        int i = 0;
        QString tag = symName;
        TupLibraryObject *object = k->library->findObject(tag);
        while (object) {
               i++;
               int index = symName.lastIndexOf(".");
               QString name = symName.mid(0, index);
               QString extension = symName.mid(index, symName.length() - index);
               tag = name + "-" + QString::number(i) + extension;
               object = k->library->findObject(tag);
        }

        tFatal() << "TupLibraryWidget::importSvg() - calling createLibraryRequest()...";

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, tag,
                                                       TupLibraryObject::Svg, k->project->spaceContext(), data, QString(), 
                                                       k->currentFrame.scene, k->currentFrame.layer, k->currentFrame.frame);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(svgPath), TOsd::Error);
    }
}

void TupLibraryWidget::importBitmapArray()
{
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the images directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir source(path); 
    QFileInfoList photograms = source.entryInfoList(QDir::Files, QDir::Name);
    int size = photograms.size();

    // Ensuring to get only graphic files here. Check extensions! (PNG, JPG, GIF, XPM) 

    int imagesCounter = 0; 
    for (int i = 0; i < size; ++i) {
         if (photograms.at(i).isFile()) {
             QString extension = photograms.at(i).suffix().toUpper();
             if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 || 
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
            progressDialog.setRange(1, size);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            for (int i = 0; i < size; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("JPG")==0 || extension.compare("PNG")==0 || extension.compare("GIF")==0 ||
                         extension.compare("XPM")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName();
                         QFile f(path);
                         QFileInfo fileInfo(f);

                         if (f.open(QIODevice::ReadOnly)) {

                             QByteArray data = f.readAll();
                             f.close();

                             if (resize) {
                                 pixmap = new QPixmap();
                                 QString extension = fileInfo.suffix().toUpper();
                                 QByteArray ba = extension.toAscii();
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

                             if (i < photograms.size()-1 && imagesCounter > 1) {

                                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, 
                                                                              k->currentFrame.frame + 1, TupProjectRequest::Add, tr("Frame %1").arg(k->currentFrame.frame + 2));
                                 emit requestTriggered(&request);

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

             QApplication::restoreOverrideCursor();

        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No image files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSvgArray() 
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
    int size = photograms.size();

    // Ensuring to get only SVG files here. Check extension! (SVG)
    int svgCounter = 0;
    for (int i = 0; i < size; ++i) {
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

            QString directory = source.dirName();
            k->libraryTree->createFolder(directory);

            QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

            QFont font = this->font();
            font.setPointSize(8);

            QProgressDialog progressDialog(this, Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Dialog);
            progressDialog.setFont(font);
            progressDialog.setLabelText(tr("Loading SVG files..."));
            progressDialog.setCancelButton(0);
            progressDialog.setRange(1, size);
            progressDialog.show();
            int index = 1;

            progressDialog.move((int) (desktop.screenGeometry().width() - progressDialog.width())/2 , 
                                (int) (desktop.screenGeometry().height() - progressDialog.height())/2);

            TupLibraryFolder *folder = new TupLibraryFolder(directory, k->project);
            k->library->addFolder(folder);

            for (int i = 0; i < size; ++i) {
                 if (photograms.at(i).isFile()) {
                     QString extension = photograms.at(i).suffix().toUpper();
                     if (extension.compare("SVG")==0) {
                         QString path = photograms.at(i).absoluteFilePath(); 
                         QString symName = photograms.at(i).fileName();
                         QFile f(path);

                         if (f.open(QIODevice::ReadOnly)) {

                             QByteArray data = f.readAll();
                             f.close();

                             TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                                                       TupLibraryObject::Svg, k->project->spaceContext(), data, directory);
                             emit requestTriggered(&request);

                             if (i < photograms.size()-1 && svgCounter > 1) {

                                 TupProjectRequest request = TupRequestBuilder::createFrameRequest(k->currentFrame.scene, k->currentFrame.layer, 
                                                                              k->currentFrame.frame + 1, TupProjectRequest::Add, tr("Frame %1").arg(k->currentFrame.frame + 2));
                                 emit requestTriggered(&request);

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

             QApplication::restoreOverrideCursor();

        }
    } else {
        TOsd::self()->display(tr("Error"), tr("No SVG files were found.<br/>Please, try another directory"), TOsd::Error);
    }
}

void TupLibraryWidget::importSound()
{
    QString sound = QFileDialog::getOpenFileName(this, tr("Import audio file..."), QDir::homePath(),
                                                 tr("Sound file") + " (*.ogg *.wav *.mp3)");

    if (sound.isEmpty()) 
        return;

    QFile f(sound);
    QFileInfo fileInfo(f);
    QString symName = fileInfo.baseName();

    if (f.open(QIODevice::ReadOnly)) {
        QByteArray data = f.readAll();
        f.close();

        TupProjectRequest request = TupRequestBuilder::createLibraryRequest(TupProjectRequest::Add, symName,
                                                     TupLibraryObject::Sound, k->project->spaceContext(), data);
        emit requestTriggered(&request);
    } else {
        TOsd::self()->display(tr("Error"), tr("Cannot open file: %1").arg(sound), TOsd::Error);
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
                 QString key = response->arg().toString();

                 int index = key.lastIndexOf(".");
                 QString name = key.mid(0, index);
                 QString extension = key.mid(index + 1, key.length() - index).toUpper();
                 TupLibraryObject *obj = k->library->findObject(key);

                 if (index < 0)
                     extension = "OBJ"; 

                 QTreeWidgetItem *item;
                 if (folderName.length() > 0 && folderName.compare("library")!=0)
                     item = new QTreeWidgetItem(k->libraryTree->getFolder(folderName));
                 else
                     item = new QTreeWidgetItem(k->libraryTree);

                 item->setText(1, name);
                 item->setText(2, extension);
                 item->setText(3, key);
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
                                 if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE)
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Svg:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/svg.png"));
                                 k->libraryTree->setCurrentItem(item);
                                 previewItem(item);
                                 if (!k->isNetworked && k->project->spaceContext() != TupProject::NONE)
                                     insertObjectInWorkspace();
                               }
                            break;
                            case TupLibraryObject::Sound:
                               {
                                 item->setIcon(0, QIcon(THEME_DIR + "icons/sound_object.png"));
                               }
                            break;
                     }

                 } else {
                     #ifdef K_DEBUG
                            tFatal() << "TupLibraryWidget::libraryResponse() - No object found: " << key;
                     #endif
                 }
              }
            break;

            case TupProjectRequest::InsertSymbolIntoFrame:
              {
                 #ifdef K_DEBUG
                        tDebug() << "*** TupLibraryWidget::libraryResponse -> InsertSymbolIntoFrame : No action taken";
                 #endif
              }
            break;

            case TupProjectRequest::RemoveSymbolFromFrame:
              {
                 QString key = response->arg().toString();

                 QTreeWidgetItemIterator it(k->libraryTree);
                 while ((*it)) {
                        // If target is not a folder 
                        if ((*it)->text(2).length() > 0) {
                            if (key == (*it)->text(3)) {
                                delete (*it);
                                // tError() << "*** Tracing deletion... flag 2"; 
                                // k->library->removeObject(key, true);
                                break;
                            } 
                        } else {
                            // If target is a folder
                            if (key == (*it)->text(1)) {
                                delete (*it);
                                k->library->removeFolder(key);
                                break;
                            } 
                        }
                        ++it;
                 }

                 previewItem(k->libraryTree->currentItem());
              }
            break;

            case TupProjectRequest::Remove:
              {
                 #ifdef K_DEBUG
                        tDebug() << "*** TupLibraryWidget::libraryResponse -> Remove : No action taken";
                 #endif
              }
            break;
  
            default:
              {
                 #ifdef K_DEBUG
                        tDebug() << "*** Project Code: " << response->action();
                        qWarning("ktlibrarywidget.cpp IMPLEMENT ME");
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

void TupLibraryWidget::importGraphicObject()
{
    QString option = k->itemType->currentText();

    if (option.compare(tr("Image")) == 0) {
        importBitmap();
        return;
    }

    if (option.compare(tr("Image Array")) == 0) {
        importBitmapArray();
        return;
    }

    if (option.compare(tr("Svg File")) == 0) {
        importSvg();
        return;
    }

    if (option.compare(tr("Svg Array")) == 0) {
        importSvgArray();
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
                    k->library->renameFolder(k->oldId, tag);
                    item->setText(1, tag);
                    k->library->renameFolder(k->oldId, tag);
                }
            } 
        } else {
            // Renaming item
            if (k->oldId.length() == 0)
                return;

            // SQA: Make the renaming action for real here!

            QString newId = item->text(1);
            QString extension = item->text(2);
            if (k->oldId.compare(newId) != 0) {
                QList<QTreeWidgetItem *> list = k->libraryTree->findItems(newId, Qt::MatchExactly, 1);
                if (list.size() > 1) {
                    int total = 0;
                    for (int i=0; i<list.size(); i++) {
                         QTreeWidgetItem *node = list.at(i);
                         if (node->text(2).compare(extension) == 0) {
                             total++;
                         }
                    }

                    if (total > 1) {
                             int index = newId.lastIndexOf("-");
                             if (index < 0) {
                                 item->setText(1, newId + "-1");
                             } else {
                                 QString first = newId.mid(0, index);
                                 QString last = newId.mid(index+1, newId.length() - index);
                                 bool *ok = false;
                                 int newIndex = last.toInt(ok);          
                                 newIndex++;
                                 newId = first + "-" + QString::number(newIndex);
                                 item->setText(1, newId);       
                             }

                    } 
                }

                k->oldId = k->oldId + "." + extension.toLower();
                newId = newId + "." + extension.toLower();

                QTreeWidgetItem *parent = item->parent();

                if (parent) 
                    k->library->renameObject(parent->text(1), k->oldId, newId);
                else
                    k->library->renameObject("", k->oldId, newId);

                TupLibraryObject::Type type = TupLibraryObject::Image;
                if (extension.compare("SVG")==0)
                    type = TupLibraryObject::Svg;
                if (extension.compare("OBJ")==0)
                    type = TupLibraryObject::Item;

                k->project->updateSymbolId(type, k->oldId, newId);
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
