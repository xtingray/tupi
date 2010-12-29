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

#include "ktexportwidget.h"

// Qt
#include <QPluginLoader>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QIntValidator>
#include <QToolButton>
#include <QFileDialog>
#include <QMessageBox>

#include "kglobal.h"
#include "kdebug.h"

#include "kitemselector.h"
#include "kxyspinbox.h"

#include <QtDebug>

/**
 * This class handles the whole process to export a project into a movie format.
 * The export widget uses a wizard to guide the process: format, escenes, target file
 * @author David Cuadrado <krawek@toonka.com>
*/

class SelectPlugin : public KExportWizardPage
{
    Q_OBJECT;
    public:
        //SelectPlugin(const KTExportWidget *kt);
        SelectPlugin();
        ~SelectPlugin();

        bool isComplete() const;
        const char *extension;

        void reset();
        void addPlugin(const QString &plugin);
        void setFormats(KTExportInterface::Formats formats);

    public slots:
        void selectedPluginItem(QListWidgetItem *);
        void selectFirstItem();
        void selectedFormatItem(QListWidgetItem *);
        const char* getFileExtension();

    signals:
        void selectedPlugin(const QString &plugin);
        void formatSelected(int format, const QString &extension);

    private:
        QListWidget *m_exporterList;
        QListWidget *m_formatList;
        const char* getFormatExtension(const QString format);
};

SelectPlugin::SelectPlugin() : KExportWizardPage(tr("Select plugin"))
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

void SelectPlugin::setFormats(KTExportInterface::Formats formats)
{
    m_formatList->clear();

    if (formats & KTExportInterface::OGV) {
        QListWidgetItem *format = new QListWidgetItem(tr("OGV Video"), m_formatList);
        format->setData(3124, KTExportInterface::OGV);
    }

    if (formats & KTExportInterface::MPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("MPEG Video"), m_formatList);
        format->setData(3124, KTExportInterface::MPEG);
    }
	
    if (formats & KTExportInterface::SWF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Macromedia flash"), m_formatList);
        format->setData(3124, KTExportInterface::SWF);
    }
	
    if (formats & KTExportInterface::AVI) {
        QListWidgetItem *format = new QListWidgetItem(tr("AVI Video"), m_formatList);
        format->setData(3124, KTExportInterface::AVI);
    }

    if (formats & KTExportInterface::RM) {
        QListWidgetItem *format = new QListWidgetItem(tr("RealMedia Video"), m_formatList);
        format->setData(3124, KTExportInterface::RM);
    }

    if (formats & KTExportInterface::ASF) {
        QListWidgetItem *format = new QListWidgetItem(tr("ASF Video"), m_formatList);
        format->setData(3124, KTExportInterface::ASF);
    }
	
    if (formats & KTExportInterface::MOV) {
        QListWidgetItem *format = new QListWidgetItem(tr("QuickTime Video"), m_formatList);
        format->setData(3124, KTExportInterface::MOV);
    }
	
    if (formats & KTExportInterface::GIF) {
        QListWidgetItem *format = new QListWidgetItem(tr("Gif Image"), m_formatList);
        format->setData(3124, KTExportInterface::GIF);
    }

    if (formats & KTExportInterface::PNG) {
        QListWidgetItem *format = new QListWidgetItem(tr("PNG Image Array"), m_formatList);
        format->setData(3124, KTExportInterface::PNG);
    }

    if (formats & KTExportInterface::JPEG) {
        QListWidgetItem *format = new QListWidgetItem(tr("JPEG Image Array"), m_formatList);
        format->setData(3124, KTExportInterface::JPEG);
    }

    if (formats & KTExportInterface::SMIL) {
        QListWidgetItem *format = new QListWidgetItem(tr("SMIL"), m_formatList);
        format->setData(3124, KTExportInterface::SMIL);
    }
}

char const* SelectPlugin::getFormatExtension(const QString format) 
{ 
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

    if (format.compare(tr("SMIL")) == 0)
        return ".smil";

    return "none";
}

void SelectPlugin::selectedFormatItem(QListWidgetItem *item)
{
    if (item) {
        extension = getFormatExtension(item->text());
        emit formatSelected(item->data(3124).toInt(), extension);
        emit completed();
    }
}

const char* SelectPlugin::getFileExtension()
{
    return extension;
}

class SelectScenes : public KExportWizardPage
{
    Q_OBJECT;
    public:
        SelectScenes(const KTExportWidget *kt);
        ~SelectScenes();

        bool isComplete() const;
        void reset();

        void setScenes(const QList<KTScene *> &scenes);
        void aboutToNextPage();

    private slots:
        void updateState();
        void updateScenesList();

    signals:
        void selectedScenes(const QList<int> &scenes);

    private:
        KItemSelector *m_selector;

};

SelectScenes::SelectScenes(const KTExportWidget *kt) : KExportWizardPage(tr("Select Scenes"))
{
    setTag("SCENE");
    m_selector = new KItemSelector;

    connect(m_selector, SIGNAL(changed()), this, SLOT(updateState()));
    connect(kt, SIGNAL(updateScenes()), this, SLOT(updateScenesList()));

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

void SelectScenes::setScenes(const QList<KTScene *> &scenes)
{
    #ifdef K_DEBUG
           K_FUNCINFO;
    #endif

    m_selector->clear();
    int pos = 1;

    foreach (KTScene *scene, scenes) {
             #ifdef K_DEBUG
                    kDebug("export") << "Adding " << scene->sceneName();
             #endif
             m_selector->addItem(QString("%1: ").arg(pos)+scene->sceneName());
             pos++;
    }

    #ifdef K_DEBUG
           kDebug("export") << "Loop: " << pos;
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
    // TODO: Add code pending
}

class ExportTo : public KExportWizardPage
{
    Q_OBJECT;
    public:
        ExportTo(const KTProject *project, bool exportImages, QString title, const KTExportWidget *kt);
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

    private:
        //QString fileToExport() const;
        QList<KTScene *> scenesToExport() const;

    public slots:
        void setScenesIndexes(const QList<int> &indexes);
        void setCurrentExporter(KTExportInterface *currentExporter);
        void setCurrentFormat(int currentFormat, const QString &extension);

    signals:
        void saveFile();
        void exportArray();
        void setFileName();
        void isDone();

    private:
        QList<int> m_indexes;
        KTExportInterface *m_currentExporter;
        KTExportInterface::Format m_currentFormat;

        const KTProject *m_project;
        QLineEdit *m_filePath;
        QLineEdit *m_prefix;
        QSpinBox *m_fps;
        KXYSpinBox *m_size;
        QString filename;
        QString path;
        QString extension;
};

ExportTo::ExportTo(const KTProject *project, bool exportImages, QString title, const KTExportWidget *widget) : KExportWizardPage(title), m_currentExporter(0), 
                   m_currentFormat(KTExportInterface::NONE), m_project(project)
{
    if (exportImages) 
        setTag("IMAGES");
    else 
        setTag("EXPORT");

    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);
    path = getenv("HOME");

    ////////////////

    QHBoxLayout *prefixLayout = new QHBoxLayout;
    prefixLayout->addWidget(new QLabel(tr("Image name prefix: ")));

    QHBoxLayout *filePathLayout = new QHBoxLayout;

    if (!exportImages)
        filePathLayout->addWidget(new QLabel(tr("File: ")));
    else
        filePathLayout->addWidget(new QLabel(tr("Directory: ")));

    QString prefix = m_project->projectName() + "_img";
    m_prefix = new QLineEdit(prefix);
    m_filePath = new QLineEdit;

    connect(m_filePath, SIGNAL(textChanged (const QString &)), this, SLOT(updateState(const QString &)));

    if (exportImages) {
        connect(m_prefix, SIGNAL(textChanged(const QString &)), this, SLOT(updateState(const QString &)));
        connect(widget, SIGNAL(exportArray()), this, SLOT(exportIt()));
    } else {
        connect(widget, SIGNAL(saveFile()), this, SLOT(exportIt()));
    }

    connect(widget, SIGNAL(setFileName()), this, SLOT(updateNameField()));

    filePathLayout->addWidget(m_filePath);

    QToolButton *button = new QToolButton;
    button->setIcon(QIcon(THEME_DIR + "icons/open.png"));

    if (!exportImages)
        connect(button, SIGNAL(clicked()), this, SLOT(chooseFile()));
    else
        connect(button, SIGNAL(clicked()), this, SLOT(chooseDirectory()));

    filePathLayout->addWidget(button);

    if (exportImages) {
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
    m_size = new KXYSpinBox(tr("Size"));
    m_size->setMaximum(1024);
    m_size->setX(dimension.width());
    m_size->setY(dimension.height());

    QGroupBox *groupBox = new QGroupBox(tr("Configuration"));
    QHBoxLayout *configLayout = new QHBoxLayout(groupBox);

    m_fps = new QSpinBox;
    m_fps->setMinimum(0);
    m_fps->setMaximum(100);
    m_fps->setValue(m_project->fps());

    configureLayout->addWidget(m_size);

    if (!exportImages) {
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
    exportIt();
}

void ExportTo::setScenesIndexes(const QList<int> &indexes)
{
    m_indexes = indexes;
}

void ExportTo::setCurrentExporter(KTExportInterface *currentExporter)
{
    m_currentExporter = currentExporter;
}

void ExportTo::setCurrentFormat(int currentFormat, const QString &value)
{
    m_currentFormat = KTExportInterface::Format(currentFormat);
    extension = value;
    filename = path;

#if defined(Q_OS_UNIX)

    if ((extension.compare(".jpg") != 0) && (extension.compare(".png") != 0)) {
        filename += "/";
        filename += m_project->projectName();
        filename += extension;
    } 

    m_filePath->setText(filename);

#endif
}

void ExportTo::updateNameField()
{
   m_filePath->setText(filename);
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

    if (filename.length() > 0) {
        m_filePath->setText(filename);
    }
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
           K_FUNCINFO;
    #endif

    bool done = false; 
    QString name = "";

    if ((extension.compare(".jpg") != 0) && (extension.compare(".png") != 0)) {
        filename = m_filePath->text();

        int indexPath = filename.lastIndexOf("/");
        int indexFile = filename.length() - indexPath;
        name = filename.right(indexFile - 1);
        path = filename.left(indexPath + 1);

        if (!name.toLower().endsWith(extension))    
            name += extension;

        if (path.length() == 0) {
            path = getenv ("HOME");
            filename = path + "/" + name;
        }

        if (QFile::exists(filename)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Warning!"),
                                          tr("File exists. Overwrite it?"),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No)
                return;
        } 

    } else {
        name = m_prefix->text();
        path = m_filePath->text();

        if (name.length() == 0) {
            KOsd::self()->display(tr("Error"), tr("Images name prefix can't be empty! Please, type a prefix."), KOsd::Error);
            return;
        }
    
        if (path.length() == 0)
            path = getenv("HOME");

        filename = path + "/" + name;
    }

    QDir directory(path);
    if (!directory.exists()) {
        //QMessageBox::critical(this, tr("Error!"), tr("Directory \"" + path.toLocal8Bit() + "\" does not exist! Please, choose another path."), QMessageBox::Ok);
        KOsd::self()->display(tr("Error"), tr("Directory \"" + path.toLocal8Bit() + "\" does not exist! Please, choose another path."), KOsd::Error);
        return;
    } else {
        QFile file(directory.filePath(name));
        if (!file.open(QIODevice::ReadWrite)) {
            file.remove();
            //QMessageBox::critical(this, tr("Error!"), tr("You have no permission to create this file. Please, choose another path."), QMessageBox::Ok);
            KOsd::self()->display(tr("Error"), tr("You have no permission to create this file. Please, choose another path."), KOsd::Error);
            return;
        }
        file.remove();
    }

    if (m_currentExporter) {

        #ifdef K_DEBUG
               kDebug() << "Exporting to file: " << filename;
        #endif

        QList<KTScene *> scenes = scenesToExport();

        #ifdef K_DEBUG
               kDebug() << "Exporting " << scenes.count() << " scenes";
        #endif

        if (scenes.count() > 0) 
            done = m_currentExporter->exportToFormat(m_project->bgColor(), filename, scenes, m_currentFormat, 
                                                     QSize((int)m_size->x(),(int)m_size->y()), 
                                                     m_fps->value());
    } else {
        KOsd::self()->display(tr("Error"), tr("Format problem. Tupi Internal error."), KOsd::Error);
    }

    if (done) {
        QString message = "File " + name + " was saved successful";
        KOsd::self()->display(tr("Information"), tr(message.toLocal8Bit()));
        emit isDone();
    } else {
        const char *msg = m_currentExporter->getExceptionMsg();
        QMessageBox::critical(this, tr("ERROR!"), tr(msg), QMessageBox::Ok);
    }
}

QList<KTScene *> ExportTo::scenesToExport() const
{
    QList<KTScene *> scenes;
    foreach (int index, m_indexes)
             scenes << m_project->scene(index);

    return scenes;
}

KTExportWidget::KTExportWidget(const KTProject *project, QWidget *parent) : KExportWizard(parent), m_project(project)
{
    #ifdef K_DEBUG
           KINIT;
    #endif

    setWindowTitle(tr("Export"));
    setWindowIcon(QIcon(THEME_DIR + "icons/export.png"));

    m_pluginSelectionPage = new SelectPlugin();
    addPage(m_pluginSelectionPage);

    m_scenesSelectionPage = new SelectScenes(this);
    m_scenesSelectionPage->setScenes(project->scenes().values());
    addPage(m_scenesSelectionPage);

    m_exportToPage = new ExportTo(project, false, tr("Export to Video File"), this);
    addPage(m_exportToPage);

    m_exportImages = new ExportTo(project, true, tr("Export to Images Array"), this);
    addPage(m_exportImages);

    connect(m_pluginSelectionPage, SIGNAL(selectedPlugin(const QString &)), this, SLOT(setExporter(const QString &)));

    connect(m_pluginSelectionPage, SIGNAL(formatSelected(int, const QString &)), m_exportToPage, SLOT(setCurrentFormat(int, const QString &)));
    connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), m_exportToPage, SLOT(setScenesIndexes(const QList<int> &)));

    connect(m_pluginSelectionPage, SIGNAL(formatSelected(int, const QString &)), m_exportImages, SLOT(setCurrentFormat(int, const QString &)));
    connect(m_scenesSelectionPage, SIGNAL(selectedScenes(const QList<int> &)), m_exportImages, SLOT(setScenesIndexes(const QList<int> &)));

    loadPlugins();

    m_pluginSelectionPage->selectFirstItem();
}

KTExportWidget::~KTExportWidget()
{
    #ifdef K_DEBUG
           KEND;
    #endif
}

void KTExportWidget::loadPlugins()
{
    QDir pluginDirectory = QDir(PLUGINS_DIR);

    foreach (QString fileName, pluginDirectory.entryList(QDir::Files)) {
             QPluginLoader loader(pluginDirectory.absoluteFilePath(fileName));
             KTExportPluginObject *plugin = qobject_cast<KTExportPluginObject*>(loader.instance());

             if (plugin) {
                 KTExportInterface *exporter = qobject_cast<KTExportInterface *>(plugin);

                 if (exporter) {
                     m_pluginSelectionPage->addPlugin(exporter->key());
                     m_plugins.insert(exporter->key(), exporter);
                 } else {
                     #ifdef K_DEBUG
                            kError() << "Can't load: " << fileName;
                     #endif
                 }
             }
    }
}

void KTExportWidget::setExporter(const QString &plugin)
{
    if (m_plugins.contains(plugin)) {
        KTExportInterface* currentExporter = m_plugins[plugin];
        m_pluginSelectionPage->setFormats(currentExporter->availableFormats());
        m_exportToPage->setCurrentExporter(currentExporter);
        m_exportImages->setCurrentExporter(currentExporter);
    }
}

#include "ktexportwidget.moc"
