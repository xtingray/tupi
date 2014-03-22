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
#include "tuppluginmanager.h"
#include "tglobal.h"
#include "tdebug.h"
#include "titemselector.h"
#include "txyspinbox.h"

// Qt
#include <QApplication>
#include <QPluginLoader>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QIntValidator>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QtDebug>
#include <QLocale>

/**
 * This class handles the whole process to export a project into a movie format.
 * The export widget uses a wizard to guide the process: format, escenes, target file
 * @author David Cuadrado
*/

class SelectPlugin : public TupExportWizardPage
{
    Q_OBJECT

    public:
        SelectPlugin();
        ~SelectPlugin();

        bool isComplete() const;
        const char *extension;

        void reset();
        void addPlugin(const QString &plugin);
        void setFormats(TupExportInterface::Formats formats);

    public slots:
        void selectedPluginItem(QListWidgetItem *);
        void selectFirstItem();
        void selectedFormatItem(QListWidgetItem *);
        const char* getFileExtension();

    signals:
        void selectedPlugin(const QString &plugin);
        void animationFormatSelected(int format, const QString &extension);
        void animatedImageFormatSelected(int format, const QString &extension);
        void imagesArrayFormatSelected(int format, const QString &extension);

    private:
        QListWidget *m_exporterList;
        QListWidget *m_formatList;
        const char* getFormatExtension(const QString format);
};

SelectPlugin::SelectPlugin() : TupExportWizardPage(tr("Select plugin"))
{
    setTag("PLUGIN");
    QWidget *container = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(container);

    m_exporterList = new QListWidget;
    m_exporterList->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(m_exporterList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(selectedPluginItem(QListWidgetItem *)));
    layout->addWidget(m_exporterList);

    m_formatList = new QListWidget;
    connect(m_formatList, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(selectedFormatItem(QListWidgetItem *)));
    layout->addWidget(m_formatList);

    setWidget(container);
    reset();
}

SelectPlugin::~SelectPlugin()
{
}

bool SelectPlugin::isComplete() const
{
    return m_exporterList->selectedItems().count() > 0 && m_formatList->selectedItems().count() > 0;
}

void SelectPlugin::reset()
{
    m_exporterList->clearSelection();
    m_formatList->clearSelection();
    m_formatList->clear();
}

void SelectPlugin::addPlugin(const QString &plugin)
{
    new QListWidgetItem(plugin, m_exporterList);
}

void SelectPlugin::selectedPluginItem(QListWidgetItem *item)
{
    if (item) {
        emit selectedPlugin(item->text());
        emit completed();
    }
}

void SelectPlugin::selectFirstItem()
{
    if (m_exporterList->count() > 0) {
        m_exporterList->item(0)->setSelected(true);
        if (m_exporterList->item(0)) {
            emit selectedPlugin(m_exporterList->item(0)->text());
            emit completed();
        }
    }
}

void SelectPlugin::setFormats(TupExportInterface::Formats formats)
{
    m_formatList->clear();

    if (formats & TupExportInterface::WEBM) {
        QListWidgetItem *format = new QListWidgetItem(tr("WEBM Video"), m_formatList);
        format->setData(3124, TupExportInterface::WEBM);
    }

    if (formats & TupExportInterface::OGV) {
        QListWidgetItem *format = new QListWidgetItem(tr("OGV Video"), m_formatList);
        format->setData(3124, TupExportInterface::OGV);
    }

    if (formats & TupExportInterface::MPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("MPEG Video"), m_formatList);
        format->setData(3124, TupExportInterface::MPEG);
    }
	
    if (formats & TupExportInterface::SWF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Macromedia flash"), m_formatList);
        format->setData(3124, TupExportInterface::SWF);
    }
	
    if (formats & TupExportInterface::AVI) {
        QListWidgetItem *format = new QListWidgetItem(tr("AVI Video"), m_formatList);
        format->setData(3124, TupExportInterface::AVI);
    }

    if (formats & TupExportInterface::ASF) {
        QListWidgetItem *format = new QListWidgetItem(tr("ASF Video"), m_formatList);
        format->setData(3124, TupExportInterface::ASF);
    }
	
    if (formats & TupExportInterface::MOV) {
        QListWidgetItem *format = new QListWidgetItem(tr("QuickTime Video"), m_formatList);
        format->setData(3124, TupExportInterface::MOV);
    }

    // SQA: The ffmpeg procedure to export animated GIF files must be implemented
    /*
    if (formats & TupExportInterface::GIF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Gif Image"), m_formatList);
        format->setData(3124, TupExportInterface::GIF);
    }
    */

    if (formats & TupExportInterface::PNG) {
        QListWidgetItem *format = new QListWidgetItem(tr("PNG Image Array"), m_formatList);
        format->setData(3124, TupExportInterface::PNG);
    }

    if (formats & TupExportInterface::JPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("JPEG Image Array"), m_formatList);
        format->setData(3124, TupExportInterface::JPEG);
    }

    if (formats & TupExportInterface::APNG) {
        QListWidgetItem *format = new QListWidgetItem(tr("Animated PNG (APNG)"), m_formatList);
        format->setData(3124, TupExportInterface::APNG);
        format->setFlags(Qt::NoItemFlags);
    }

    if (formats & TupExportInterface::SMIL) {
        QListWidgetItem *format = new QListWidgetItem(tr("SMIL"), m_formatList);
        format->setData(3124, TupExportInterface::SMIL);
    }
}

char const* SelectPlugin::getFormatExtension(const QString format) 
{ 
    if (format.compare(tr("WEBM Video")) == 0)
        return ".webm";

    if (format.compare(tr("OGV Video")) == 0)
        return ".ogv";

    if (format.compare(tr("MPEG Video")) == 0)
        return ".mpg";

    if (format.compare(tr("Macromedia flash")) == 0)
        return ".swf";

    if (format.compare(tr("AVI Video")) == 0)
        return ".avi";

    if (format.compare(tr("RealMedia Video")) == 0) 
        return ".rm";

    if (format.compare(tr("ASF Video")) == 0)
        return ".asf";

    if (format.compare(tr("QuickTime Video")) == 0)
        return ".mov";

    if (format.compare(tr("Gif Image")) == 0)
        return ".gif";

    if (format.compare(tr("PNG Image Array")) == 0)
        return ".png";

    if (format.compare(tr("JPEG Image Array")) == 0)
        return ".jpg";

    if (format.compare(tr("Animated PNG (APNG)")) == 0)
        return ".png";

    if (format.compare(tr("SMIL")) == 0)
        return ".smil";

    return ".none";
}

void SelectPlugin::selectedFormatItem(QListWidgetItem *item)
{
    if (item) {
        extension = getFormatExtension(item->text());
        QList<QListWidgetItem *> family = m_exporterList->selectedItems();
        QListWidgetItem *familyItem = (QListWidgetItem *) family.at(0); 

        QString familyLabel = familyItem->text(); 

        if (familyLabel.compare(tr("Animated Image")) == 0) {
            emit animatedImageFormatSelected(item->data(3124).toInt(), extension);
        } else if (familyLabel.compare(tr("Image Array")) == 0) {
                   emit imagesArrayFormatSelected(item->data(3124).toInt(), extension);
        } else { 
            emit animationFormatSelected(item->data(3124).toInt(), extension);
        }

        // emit formatSelected(item->data(3124).toInt(), extension);

        emit completed();
    }
}

const char* SelectPlugin::getFileExtension()
{
    return extension;
}

class SelectScenes : public TupExportWizardPage
{
    Q_OBJECT

    public:
        SelectScenes(const TupExportWidget *widget);
        ~SelectScenes();

        bool isComplete() const;
        void reset();

        void setScenes(const QList<TupScene *> &scenes);
        void aboutToNextPage();

    private slots:
        void updateState();
        void updateScenesList();

    signals:
        void selectedScenes(const QList<int> &scenes);

    private:
        TItemSelector *m_selector;

};

SelectScenes::SelectScenes(const TupExportWidget *widget) : TupExportWizardPage(tr("Select Scenes"))
{
    setTag("SCENE");
    m_selector = new TItemSelector;

    connect(m_selector, SIGNAL(changed()), this, SLOT(updateState()));
    connect(widget, SIGNAL(updateScenes()), this, SLOT(updateScenesList()));

    setWidget(m_selector);
}

SelectScenes::~SelectScenes()
{
}

bool SelectScenes::isComplete() const
{
    return m_selector->selectedItems().count() > 0;
}

void SelectScenes::reset()
{
}

void SelectScenes::setScenes(const QList<TupScene *> &scenes)
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    m_selector->clear();
    int pos = 1;

    foreach (TupScene *scene, scenes) {
             #ifdef K_DEBUG
                    tDebug("export") << "SelectScenes::setScenes() - Adding " << scene->sceneName();
             #endif
             m_selector->addItem(QString("%1: ").arg(pos) + scene->sceneName());
             pos++;
    }

    #ifdef K_DEBUG
           tWarning("export") << "SelectScenes::setScenes() - Available Scenes: " << pos - 1;
    #endif

    m_selector->selectFirstItem();
}

void SelectScenes::aboutToNextPage()
{
    emit selectedScenes(m_selector->selectedIndexes());
}

void SelectScenes::updateState()
{
    emit completed();
}

void SelectScenes::updateScenesList()
{
    // SQA: Pending code right over here
}

class ExportTo : public TupExportWizardPage
{
    Q_OBJECT

    public:
        ExportTo(const TupProject *project, TupExportWidget::OutputFormat output, QString title, const TupExportWidget *widget);
        ~ExportTo();

        bool isComplete() const;
        void reset();
        void aboutToFinish();

    public slots:
        void exportIt();

    private slots:
        void updateState(const QString & text);
        void chooseFile();
        void chooseDirectory();
        void updateNameField();
        void enableTransparency(bool flag);

    private:
        //QString fileToExport() const;
        QList<TupScene *> scenesToExport() const;

    public slots:
        void setScenesIndexes(const QList<int> &indexes);
        void setCurrentExporter(TupExportInterface *currentExporter);
        void setCurrentFormat(int currentFormat, const QString &extension);

    signals:
        void saveFile();
        void exportArray();
        // void setFileName();
        void isDone();

    private:
        QList<int> m_indexes;
        TupExportInterface *m_currentExporter;
        TupExportInterface::Format m_currentFormat;
        TupExportWidget::OutputFormat output;

        const TupProject *m_project;
        QLineEdit *m_filePath;
        QLineEdit *m_prefix;
        QSpinBox *m_fps;
        TXYSpinBox *m_size;
        QString filename;
        QString path;
        QString extension;
        QCheckBox *bgTransparency;
        bool transparency;
};

ExportTo::ExportTo(const TupProject *project, TupExportWidget::OutputFormat outputFormat, QString title, const TupExportWidget *widget) : TupExportWizardPage(title), m_currentExporter(0), 
                   m_currentFormat(TupExportInterface::NONE), m_project(project)
{
    output = outputFormat;
    transparency = false;

    if (output == TupExportWidget::Animation) {
        setTag("ANIMATION");
    } else if (output == TupExportWidget::ImagesArray) {
        setTag("IMAGES_ARRAY");
    } else if (output == TupExportWidget::AnimatedImage) {
        setTag("ANIMATED_IMAGE");
    }

    bgTransparency = new QCheckBox(tr("Enable background transparency"));

    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);
    path = getenv("HOME");

    ////////////////

    QHBoxLayout *prefixLayout = new QHBoxLayout;
    prefixLayout->addWidget(new QLabel(tr("Image name prefix: ")));

    QHBoxLayout *filePathLayout = new QHBoxLayout;

    if (output == TupExportWidget::ImagesArray) {
        filePathLayout->addWidget(new QLabel(tr("Directory: ")));
    // } else if (output == TupExportWidget::Animation || output == TupExportWidget::AnimatedImage) {
    } else {
        filePathLayout->addWidget(new QLabel(tr("File: ")));
    }

    QString prefix = m_project->projectName() + "_img";
    m_prefix = new QLineEdit(prefix);
    m_filePath = new QLineEdit;

    connect(m_filePath, SIGNAL(textChanged (const QString &)), this, SLOT(updateState(const QString &)));

    if (output == TupExportWidget::Animation) {
        connect(widget, SIGNAL(exportAnimation()), this, SLOT(exportIt()));
        connect(widget, SIGNAL(setAnimationFileName()), this, SLOT(updateNameField()));
    }

    if (output == TupExportWidget::AnimatedImage) {
        connect(widget, SIGNAL(exportAnimatedImage()), this, SLOT(exportIt()));
        connect(widget, SIGNAL(setAnimatedImageFileName()), this, SLOT(updateNameField()));
    }

    if (output == TupExportWidget::ImagesArray) {
        connect(m_prefix, SIGNAL(textChanged(const QString &)), this, SLOT(updateState(const QString &)));
        connect(widget, SIGNAL(exportImagesArray()), this, SLOT(exportIt()));
        connect(widget, SIGNAL(setImagesArrayFileName()), this, SLOT(updateNameField()));
    } 

    filePathLayout->addWidget(m_filePath);

    QToolButton *button = new QToolButton;
    button->setIcon(QIcon(THEME_DIR + "icons/open.png"));

    if (output == TupExportWidget::ImagesArray) {
        connect(button, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    } else {
        connect(button, SIGNAL(clicked()), this, SLOT(chooseFile()));
    }

    filePathLayout->addWidget(button);

    if (output == TupExportWidget::ImagesArray) {
        prefixLayout->addWidget(m_prefix);
        prefixLayout->addWidget(new QLabel(tr("i.e. <B>%1</B>01.png / <B>%1</B>01.jpg").arg(prefix)));
        prefixLayout->addSpacing(200);
        layout->addLayout(prefixLayout);
    }

    layout->addLayout(filePathLayout);

    /////////////////

    QWidget *configure = new QWidget;
    QHBoxLayout *configureLayout = new QHBoxLayout(configure);
    configureLayout->addStretch();

    QSize dimension = m_project->dimension(); 
    m_size = new TXYSpinBox(tr("Size"));

    int maxDimension = dimension.width();
    if (maxDimension < dimension.height())
        maxDimension = dimension.height();

    m_size->setMaximum(maxDimension*2);

    m_size->setX(dimension.width());
    m_size->setY(dimension.height());

    QGroupBox *groupBox = new QGroupBox(tr("Configuration"));
    QHBoxLayout *configLayout = new QHBoxLayout(groupBox);

    m_fps = new QSpinBox;
    m_fps->setMinimum(0);
    m_fps->setMaximum(100);
    m_fps->setValue(m_project->fps());

    configureLayout->addWidget(m_size);

    if (output == TupExportWidget::ImagesArray) {
        connect(bgTransparency, SIGNAL(toggled(bool)), this, SLOT(enableTransparency(bool)));
        configureLayout->addWidget(bgTransparency);
    } else {
        configLayout->addWidget(new QLabel(tr("FPS")));
        configLayout->addWidget(m_fps);
        configureLayout->addWidget(groupBox);
    }

    configureLayout->addStretch();

    layout->addWidget(configure);
    layout->addStretch();

    setWidget(container);
}

ExportTo::~ExportTo()
{
}

bool ExportTo::isComplete() const
{
    return !m_filePath->text().isEmpty();
}

void ExportTo::reset()
{
}

void ExportTo::aboutToFinish()
{
    // exportIt();
}

void ExportTo::setScenesIndexes(const QList<int> &indexes)
{
    m_indexes = indexes;
}

void ExportTo::setCurrentExporter(TupExportInterface *currentExporter)
{
    m_currentExporter = currentExporter;
}

void ExportTo::setCurrentFormat(int currentFormat, const QString &value)
{
    m_currentFormat = TupExportInterface::Format(currentFormat);
    extension = value;
    filename = path;

#if defined(Q_OS_UNIX)

    if (m_currentFormat == TupExportInterface::APNG || (m_currentFormat != TupExportInterface::PNG && m_currentFormat != TupExportInterface::JPEG)) { // Animated Image or Animation
        if (!filename.endsWith(QDir::separator()))
            filename += QDir::separator();

        filename += m_project->projectName();
        filename += extension;
    } else { // Images Array
        filename = getenv("HOME");

        if (m_currentFormat == TupExportInterface::JPEG) {
            if (bgTransparency->isEnabled())
                bgTransparency->setEnabled(false);
        } else {
            if (!bgTransparency->isEnabled())
                bgTransparency->setEnabled(true);
        }
    } 

    m_filePath->setText(filename);

#endif
}

void ExportTo::updateNameField()
{
   if (filename.length() > 0) {
       m_filePath->setText(filename);
   } 
}

void ExportTo::enableTransparency(bool flag)
{
   transparency = flag; 
}

void ExportTo::chooseFile()
{
    QFileDialog dialog(this);
    dialog.setDirectory(filename);
    const char *filter = "Video File (*" + extension.toLocal8Bit() + ")";
    filename = dialog.getSaveFileName(this, tr("Choose a file name..."), QString(), tr(filter));

    if (filename.length() > 0) {
        if (!filename.toLower().endsWith(extension)) 
            filename += extension;

        m_filePath->setText(filename);
    }
}

void ExportTo::chooseDirectory()
{
    QString dir = getenv("HOME");
    filename = QFileDialog::getExistingDirectory(this, tr("Choose a directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (filename.length() > 0)
        m_filePath->setText(filename);
    
}

void ExportTo::updateState(const QString &name)
{
    if (name.length() > 0) 
        emit completed();
    else
        emit emptyField(); 
}

void ExportTo::exportIt()
{
    #ifdef K_DEBUG
           T_FUNCINFO;
    #endif

    bool done = false; 
    QString name = "";

    if (m_currentFormat == TupExportInterface::JPEG || m_currentFormat == TupExportInterface::PNG) { // Images Array

        name = m_prefix->text();
        path = m_filePath->text();

        if (name.length() == 0) {
            TOsd::self()->display(tr("Error"), tr("Images name prefix can't be empty! Please, type a prefix."), TOsd::Error);
            return;
        }

        if (path.length() == 0)
            path = getenv("HOME");

        filename = path + QDir::separator() + name;

    } else { // Animation or Animated Image

        filename = m_filePath->text();

        if (filename.length() == 0) {
            TOsd::self()->display(tr("Error"), tr("Directory doesn't exist! Please, choose another path."), TOsd::Error);
            #ifdef K_DEBUG
                   tError() << "ExportTo::exportIt() - Fatal Error: Directory doesn't exist! -> " << path.toLocal8Bit();
            #endif
            return;
        }

        int indexPath = filename.lastIndexOf(QDir::separator());
        int indexFile = filename.length() - indexPath;
        name = filename.right(indexFile - 1);
        path = filename.left(indexPath + 1);

        if (!name.toLower().endsWith(extension))    
            name += extension;

        if (path.length() == 0) {
            path = getenv("HOME");
            filename = path + QDir::separator() + name;
        }

        if (QFile::exists(filename)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Warning!"),
                                          tr("File exists. Overwrite it?"),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No)
                return;
        } 

    }

    QDir directory(path);
    if (!directory.exists()) {
        TOsd::self()->display(tr("Error"), tr("Directory \"" + path.toLocal8Bit() + "\" doesn't exist! Please, choose another path."), TOsd::Error);
        return;
    } else {
        QFile file(directory.filePath(name));
        if (!file.open(QIODevice::ReadWrite)) {
            file.remove();
            TOsd::self()->display(tr("Error"), tr("You have no permission to create this file. Please, choose another path."), TOsd::Error);
            return;
        }
        file.remove();
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (m_currentExporter) {

        #ifdef K_DEBUG
               tWarning() << "ExportTo::exportIt() - Exporting to file: " << filename;
        #endif

        QList<TupScene *> scenes = scenesToExport();

        #ifdef K_DEBUG
               tWarning() << "ExportTo::exportIt() - Exporting " << scenes.count() << " scenes";
        #endif

        if (scenes.count() > 0) { 
            int width = (int) m_size->x();
            int height = (int) m_size->y();
            /* ffmpeg requirement: resolution must be a multiple of two */
            if (width%2 != 0)
                width++;
            if (height%2 != 0)
                height++;

            QColor color = m_project->bgColor();
            if (m_currentFormat == TupExportInterface::PNG) {
                if (transparency)
                    color.setAlpha(0);
                else
                    color.setAlpha(255);
            }

            done = m_currentExporter->exportToFormat(color, filename, scenes, m_currentFormat, 
                                                     QSize(width, height), m_fps->value());
        }
    } else {
        TOsd::self()->display(tr("Error"), tr("Format problem. Tupi Internal error."), TOsd::Error);
    }

    QApplication::restoreOverrideCursor();

    if (done) {
        QString message = "File " + name + " was saved successful";
        TOsd::self()->display(tr("Information"), tr(message.toLocal8Bit()));
        emit isDone();
    } else {
        const char *msg = m_currentExporter->getExceptionMsg();
        QMessageBox::critical(this, tr("ERROR!"), tr(msg), QMessageBox::Ok);
    }
}

QList<TupScene *> ExportTo::scenesToExport() const
{
    QList<TupScene *> scenes;
    foreach (int index, m_indexes)
             scenes << m_project->scene(index);

    return scenes;
}

class VideoProperties : public TupExportWizardPage
{
    Q_OBJECT

    public:
        VideoProperties(const TupExportWidget *widget);
        ~VideoProperties();

        bool isComplete() const;
        void reset();
        QString title() const;
        QString topics() const;
        QString description() const;
        QList<int> scenesList() const;
        bool successful();
        // TupExportWidget::Format workType();

    signals:
        void isDone();

    private slots:
        void resetTitleColor(const QString &text);
        void resetTopicsColor(const QString &text);
        void postIt();
        void setScenesIndexes(const QList<int> &indexes);

    private:
        // QComboBox *exportCombo;
        QLineEdit *titleEdit;
        QLineEdit *topicsEdit;
        QTextEdit *descText;
        QList<int> scenes;
        bool isOk;
};

VideoProperties::VideoProperties(const TupExportWidget *widget) : TupExportWizardPage(tr("Set Animation Properties"))
{
    setTag("PROPERTIES");

    QLocale utf(QLocale::AnyLanguage, QLocale::AnyCountry);
    isOk = false;

    connect(widget, SIGNAL(saveVideoToServer()), this, SLOT(postIt()));

    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);

    QLabel *titleLabel = new QLabel(tr("Title"));
    titleEdit = new QLineEdit(tr("My Video"));
    titleEdit->setLocale(utf);
    connect(titleEdit, SIGNAL(textChanged(const QString &)), this, SLOT(resetTitleColor(const QString &)));
    titleLabel->setBuddy(titleEdit);

    QLabel *topicsLabel = new QLabel(tr("Topics"));
    topicsEdit = new QLineEdit(tr("#topic1 #topic2 #topic3"));
    topicsEdit->setLocale(utf);
    connect(topicsEdit, SIGNAL(textChanged(const QString &)), this, SLOT(resetTopicsColor(const QString &)));
    topicsLabel->setBuddy(topicsEdit);

    QLabel *descLabel = new QLabel(tr("Description"));

    descText = new QTextEdit;
    descText->setLocale(utf);
    descText->setAcceptRichText(false);
    descText->setFixedHeight(80);
    descText->setText(tr("Just a little taste of my style :)"));

    QHBoxLayout *exportLayout = new QHBoxLayout;
    exportLayout->setAlignment(Qt::AlignHCenter);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(titleEdit);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(topicsLabel);
    middleLayout->addWidget(topicsEdit);

    layout->addLayout(exportLayout);
    layout->addLayout(topLayout);
    layout->addLayout(middleLayout);
    layout->addWidget(descLabel);
    layout->addWidget(descText);

    setWidget(container);
}

VideoProperties::~VideoProperties()
{
}

bool VideoProperties::isComplete() const
{
    return isOk;
}

void VideoProperties::reset()
{
}

QString VideoProperties::title() const
{
     QString title = QString::fromUtf8(titleEdit->text().toUtf8());
     return title;
}

QString VideoProperties::topics() const
{
     QString topics = QString::fromUtf8(topicsEdit->text().toUtf8());
     return topics;
}

QString VideoProperties::description() const
{
     QString description = QString::fromUtf8(descText->toPlainText().toUtf8());
     return description;
}

QList<int> VideoProperties::scenesList() const
{
     return scenes;
}

void VideoProperties::postIt()
{
    if (titleEdit->text().length() == 0) {
        titleEdit->setText(tr("Set a title for the picture here!"));
        titleEdit->selectAll();
        isOk = false;
        return;
    }

    if (topicsEdit->text().length() == 0) {
        topicsEdit->setText(tr("Set some topic tags for the picture here!"));
        topicsEdit->selectAll();
        isOk = false;
        return;
    }

    isOk = true;
    emit isDone();
}

void VideoProperties::resetTitleColor(const QString &)
{
    QPalette palette = titleEdit->palette();
    if (titleEdit->text().length() > 0 && titleEdit->text().compare(tr("Set a title for the picture here!")) != 0)
        palette.setBrush(QPalette::Base, Qt::white);
    else
        palette.setBrush(QPalette::Base, QColor(255, 140, 138));

    titleEdit->setPalette(palette);
}

void VideoProperties::resetTopicsColor(const QString &)
{
    QPalette palette = topicsEdit->palette();
    if (topicsEdit->text().length() > 0 && topicsEdit->text().compare(tr("Set some topic tags for the picture here!")) != 0)
        palette.setBrush(QPalette::Base, Qt::white);
    else
        palette.setBrush(QPalette::Base, QColor(255, 140, 138));

    topicsEdit->setPalette(palette);
}

void VideoProperties::setScenesIndexes(const QList<int> &indexes)
{
    scenes = indexes;
}

/*
TupExportWidget::Format VideoProperties::workType()
{
    return TupExportWidget::Format(exportCombo->currentIndex());
}
*/

TupExportWidget::TupExportWidget(const TupProject *project, QWidget *parent, bool isLocal) : TupExportWizard(parent), m_project(project)
{
    #ifdef K_DEBUG
           TINIT;
    #endif

    if (isLocal) {
        setWindowTitle(tr("Export to Video"));
        setWindowIcon(QIcon(THEME_DIR + "icons/export.png"));

        m_pluginSelectionPage = new SelectPlugin();
        addPage(m_pluginSelectionPage);

        m_scenesSelectionPage = new SelectScenes(this);
        m_scenesSelectionPage->setScenes(project->scenes().values());
        addPage(m_scenesSelectionPage);

        m_exportAnimation = new ExportTo(project, TupExportWidget::Animation, tr("Export to Video File"), this);
        addPage(m_exportAnimation);

        m_exportImagesArray = new ExportTo(project, TupExportWidget::ImagesArray, tr("Export to Images Array"), this);
        addPage(m_exportImagesArray);

        m_exportAnimatedImage = new ExportTo(project, TupExportWidget::AnimatedImage, tr("Export to Animated Image"), this);
        addPage(m_exportAnimatedImage);

        connect(m_pluginSelectionPage, SIGNAL(selectedPlugin(const QString &)), this, SLOT(setExporter(const QString &)));
        connect(m_pluginSelectionPage, SIGNAL(animationFormatSelected(int, const QString &)), m_exportAnimation, SLOT(setCurrentFormat(int, const QString &)));
        connect(m_pluginSelectionPage, SIGNAL(imagesArrayFormatSelected(int, const QString &)), m_exportImagesArray, SLOT(setCurrentFormat(int, const QString &)));
        connect(m_pluginSelectionPage, SIGNAL(animatedImageFormatSelected(int, const QString &)), m_exportAnimatedImage, SLOT(setCurrentFormat(int, const QString &)));

        connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), m_exportAnimation, SLOT(setScenesIndexes(const QList<int> &)));
        connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), m_exportImagesArray, SLOT(setScenesIndexes(const QList<int> &)));
        connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), m_exportAnimatedImage, SLOT(setScenesIndexes(const QList<int> &)));

        loadPlugins();
        m_pluginSelectionPage->selectFirstItem();

    } else {
        setWindowTitle(tr("Post Animation in Tupitube"));
        setWindowIcon(QIcon(THEME_DIR + "icons/net_document.png"));

        m_scenesSelectionPage = new SelectScenes(this);
        m_scenesSelectionPage->setScenes(project->scenes().values());
        addPage(m_scenesSelectionPage);

        videoProperties = new VideoProperties(this);
        addPage(videoProperties);

        connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), videoProperties, SLOT(setScenesIndexes(const QList<int> &)));
    }
}

TupExportWidget::~TupExportWidget()
{
    #ifdef K_DEBUG
           TEND;
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
                     if (exporter->key().compare(tr("Image Array")) == 0)
                         index = 2;
                     if (exporter->key().compare(tr("Animated Image")) == 0)
                         index = 3;

                     pluginList.insert(index, exporter);
                 } else {
                     #ifdef K_DEBUG
                            tError() << "TupExportWidget::loadPlugins() - [ Fatal Error ] - Can't load export plugin";
                     #endif
                 }
             }
    }

    for (int i=0; i<pluginList.size(); i++) {
         TupExportInterface *exporter = pluginList.at(i);
         m_pluginSelectionPage->addPlugin(exporter->key());
         m_plugins.insert(exporter->key(), exporter);
    }
}

void TupExportWidget::setExporter(const QString &plugin)
{
    if (m_plugins.contains(plugin)) {
        TupExportInterface* currentExporter = m_plugins[plugin];
        m_pluginSelectionPage->setFormats(currentExporter->availableFormats());

        if (currentExporter)
            m_exportAnimation->setCurrentExporter(currentExporter);

        m_exportImagesArray->setCurrentExporter(currentExporter);
    } else {
        #ifdef K_DEBUG
               tError() << "TupExportWidget::setExporter() - [ Fatal Error ] - Can't load export plugin";
        #endif
    }
}

QString TupExportWidget::videoTitle() const
{
    return videoProperties->title();
}

QString TupExportWidget::videoTopics() const
{
    return videoProperties->topics();
}

QString TupExportWidget::videoDescription() const
{
    return videoProperties->description();
}

QList<int> TupExportWidget::videoScenes() const
{
    return videoProperties->scenesList();
}

bool TupExportWidget::isComplete()
{
    return videoProperties->isComplete();
}

/*
TupExportWidget::Format TupExportWidget::workType()
{
    return videoProperties->workType();
}
*/

#include "tupexportwidget.moc"
