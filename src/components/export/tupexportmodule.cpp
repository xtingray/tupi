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

#include "tupexportmodule.h"

TupExportModule::TupExportModule(TupProject *project, TupExportWidget::OutputFormat outputFormat, 
                                 QString title, const TupExportWidget *widget) : TupExportWizardPage(title), m_currentExporter(0), 
                                 m_currentFormat(TupExportInterface::NONE), m_project(project)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExportModule::TupExportModule()]";
        #else
            TINIT;
        #endif
    #endif

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
    // path = getenv("HOME");
    path = QDir::homePath();

    ////////////////

    QHBoxLayout *prefixLayout = new QHBoxLayout;
    prefixLayout->addWidget(new QLabel(tr("Image name prefix: ")));

    QHBoxLayout *filePathLayout = new QHBoxLayout;

    if (output == TupExportWidget::ImagesArray)
        filePathLayout->addWidget(new QLabel(tr("Directory: ")));
    else // Animation or AnimatedImage
        filePathLayout->addWidget(new QLabel(tr("File: ")));

    QString prefix = m_project->projectName() + "_img";
    m_prefix = new QLineEdit(prefix);
    m_filePath = new QLineEdit;

    connect(m_filePath, SIGNAL(textChanged(const QString &)), this, SLOT(updateState(const QString &)));

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
    button->setIcon(QIcon(THEME_DIR + "icons" + QDir::separator() + "open.png"));

    if (output == TupExportWidget::ImagesArray)
        connect(button, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    else
        connect(button, SIGNAL(clicked()), this, SLOT(chooseFile()));

    filePathLayout->addWidget(button);

    if (output == TupExportWidget::ImagesArray) {
        prefixLayout->addWidget(m_prefix);
        prefixLayout->addWidget(new QLabel(tr("i.e. ") + "<B>" + prefix + "</B>01.png / jpeg / svg"));
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

TupExportModule::~TupExportModule()
{
}

bool TupExportModule::isComplete() const
{
    return !m_filePath->text().isEmpty();
}

void TupExportModule::reset()
{
}

void TupExportModule::aboutToFinish()
{
    // exportIt();
}

void TupExportModule::setScenesIndexes(const QList<int> &indexes)
{
    m_indexes = indexes;
}

void TupExportModule::setCurrentExporter(TupExportInterface *currentExporter)
{
    m_currentExporter = currentExporter;
}

void TupExportModule::setCurrentFormat(int currentFormat, const QString &value)
{
    m_currentFormat = TupExportInterface::Format(currentFormat);
    extension = value;
    filename = path;

    if (m_currentFormat == TupExportInterface::APNG || (m_currentFormat != TupExportInterface::PNG 
        && m_currentFormat != TupExportInterface::JPEG && m_currentFormat != TupExportInterface::SVG)) { // Animated Image or Animation
        if (!filename.endsWith(QDir::separator()))
            filename += QDir::separator();

        filename += m_project->projectName();
        filename += extension;
    } else { // Images Array
        // filename = getenv("HOME");
        filename = QDir::homePath();

        if (m_currentFormat == TupExportInterface::JPEG || m_currentFormat == TupExportInterface::SVG) {
            if (bgTransparency->isEnabled())
                bgTransparency->setEnabled(false);
        } else {
            if (!bgTransparency->isEnabled())
                bgTransparency->setEnabled(true);
        }
    } 

#ifdef Q_OS_WIN
    filename.replace(QString("/"), QString("\\"));
#endif

    m_filePath->setText(filename);
}

void TupExportModule::updateNameField()
{
   if (filename.length() > 0) 
       m_filePath->setText(filename);
}

void TupExportModule::enableTransparency(bool flag)
{
   transparency = flag; 
}

void TupExportModule::chooseFile()
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

void TupExportModule::chooseDirectory()
{
    QString dir = QDir::homePath();
    filename = QFileDialog::getExistingDirectory(this, tr("Choose a directory..."), dir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (filename.length() > 0)
        m_filePath->setText(filename);
}

void TupExportModule::updateState(const QString &name)
{
    if (name.length() > 0) 
        emit completed();
    else
        emit emptyField(); 
}

void TupExportModule::exportIt()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN
            qDebug() << "[TupExportModule::exportIt()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    bool done = false; 
    bool isArray = false;
    QString name = "";

    if (m_currentFormat == TupExportInterface::JPEG || m_currentFormat == TupExportInterface::PNG || m_currentFormat == TupExportInterface::SVG) { // Images Array
        isArray = true;
        name = m_prefix->text();
        path = m_filePath->text();

        if (name.length() == 0) {
            TOsd::self()->display(tr("Error"), tr("Images name prefix can't be empty! Please, type a prefix."), TOsd::Error);
            return;
        }

        if (path.length() == 0)
            path = QDir::homePath();

        filename = path + QDir::separator() + name;

        if (QFile::exists(QString(filename + "0000" + extension))) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, tr("Warning!"), tr("Image sequence already exists. Overwrite it?"),
                                          QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No)
                return;
        }
    } else { // Animation or Animated Image
        filename = m_filePath->text();

        if (filename.length() == 0) {
            TOsd::self()->display(tr("Error"), tr("Directory doesn't exist! Please, choose another path."), TOsd::Error);

            #ifdef K_DEBUG
                QString file = path.toLocal8Bit();
                QString msg = "TupExportModule::exportIt() - [Tracer 01] Fatal Error: Directory doesn't exist! -> " + file;
                #ifdef Q_OS_WIN
                    qDebug() << msg;
                #else
                    tError() << msg;
                #endif
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
            //path = getenv("HOME");
            path = QDir::homePath();
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
        TOsd::self()->display(tr("Error"), tr("Directory doesn't exist! Please, choose another path."), TOsd::Error);
        #ifdef K_DEBUG
            QString file = path.toLocal8Bit();
            QString msg = "TupExportModule::exportIt() - [Tracer 02] Fatal Error: Directory doesn't exist! -> " + file;
            #ifdef Q_OS_WIN
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    } else {
        if (m_currentFormat == TupExportInterface::JPEG || m_currentFormat == TupExportInterface::PNG || m_currentFormat == TupExportInterface::SVG) { // Images Array
            QFileInfo dir(path);
            if (!dir.isReadable() || !dir.isWritable()) {
                TOsd::self()->display(tr("Error"), tr("Insufficient permissions. Please, choose another directory."), TOsd::Error);
                return;
            }
        } else {
            QFile file(directory.filePath(name));
            if (!file.open(QIODevice::ReadWrite)) {
                file.remove();
                TOsd::self()->display(tr("Error"), tr("Insufficient permissions. Please, choose another path."), TOsd::Error);
                return;
            }
            file.remove();
        }
    }

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    if (m_currentExporter) {
        #ifdef K_DEBUG
            QString file = path.toLocal8Bit();
            QString msg = "TupExportModule::exportIt() -  Exporting to file: " + file;
            #ifdef Q_OS_WIN
                qWarning() << msg;
            #else
                tWarning() << msg;
            #endif
        #endif

        QList<TupScene *> scenes = scenesToExport();

        #ifdef K_DEBUG
            QString msg1 = "TupExportModule::exportIt() - Exporting " + QString::number(scenes.count()) + " scenes";
            #ifdef Q_OS_WIN
                qWarning() << msg1;
            #else
                tWarning() << msg1;
            #endif
        #endif

        if (scenes.count() > 0) { 
            int width = (int) m_size->x();
            int height = (int) m_size->y();
            /* libav requirement: resolution must be a multiple of two */
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
                                                     QSize(width, height), m_fps->value(), m_project->library());
        }
    } else {
        TOsd::self()->display(tr("Error"), tr("Format problem. Tupi Internal error."), TOsd::Error);
    }

    QApplication::restoreOverrideCursor();

    if (done) {
        QString message = tr("Video file") + " " + name + " " + tr("was saved successful");
        if (isArray)
            message = tr("Image sequence was saved successful");
        TOsd::self()->display(tr("Information"), tr(message.toLocal8Bit()));
        emit isDone();
    } else {
        QString msg = m_currentExporter->getExceptionMsg();
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Fatal Error: Can't export video"));
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setTextFormat(Qt::RichText);
        msgBox.setText(msg);
        msgBox.exec();
    }
}

QList<TupScene *> TupExportModule::scenesToExport() const
{
    QList<TupScene *> scenes;
    foreach (int index, m_indexes)
             scenes << m_project->scene(index);

    return scenes;
}

