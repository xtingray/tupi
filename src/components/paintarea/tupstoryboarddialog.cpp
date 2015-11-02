/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
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

#include "tupstoryboarddialog.h"

struct TupStoryBoardDialog::Private
{
    bool isNetworked;
    TupExportInterface *imagePlugin;
    QColor bgColor;
    QSize size;
    QSize scaledSize;
    int sceneIndex;
    TupScene *scene;

    TupStoryboard *storyboard;
    int currentIndex;
    QString path;

    QBoxLayout *layout;
    QVBoxLayout *formLayout;

    QWidget *storyPanel;
    QWidget *scenePanel;

    QListWidget *list;
    QLabel *screenLabel;

    QLineEdit *titleEdit;
    QLineEdit *topicsEdit;
    QLineEdit *authorEdit;
    QTextEdit *summaryEdit;

    QLabel *sceneLabel;
    QLineEdit *sceneTitleEdit;
    QLineEdit *sceneDurationEdit;
    QTextEdit *sceneDescriptionEdit;

    QLocale utf;
    TupLibrary *library;
};

TupStoryBoardDialog::TupStoryBoardDialog(bool isNetworked, TupExportInterface *imagePlugin, const QColor &color, 
                                         const QSize &size, TupScene *scene, int sceneIndex, TupLibrary *library, QWidget *parent) : QDialog(parent), k(new Private)
{
    k->isNetworked = isNetworked;
    k->imagePlugin = imagePlugin;
    k->bgColor = color;
    k->size = size;
    k->scene = scene;
    k->sceneIndex = sceneIndex;
    k->storyboard = k->scene->storyboard();
    k->library = library;
    k->utf = QLocale(QLocale::AnyLanguage, QLocale::AnyCountry);

    QDesktopWidget desktop;
    k->scaledSize = QSize();

    if (size.width() > size.height()) {
        if (k->size.width() + 500 > desktop.screenGeometry().width()) {
            int w = desktop.screenGeometry().width() - 500;
            int h = (k->size.height() * w) / k->size.width(); 
            k->scaledSize.setWidth(w);                                     
            k->scaledSize.setHeight(h);
        } else {
            k->scaledSize = k->size; 
        }
    } else {
        if (k->size.height() + 400 > desktop.screenGeometry().height()) {
            int h = desktop.screenGeometry().height() - 400;
            int w = (k->size.width() * h) / k->size.height();
            k->scaledSize.setWidth(w);
            k->scaledSize.setHeight(h);
        } else {
            k->scaledSize = k->size;
        }
    }

    if (k->scaledSize.height() + 400 > desktop.screenGeometry().height()) {
        int h = desktop.screenGeometry().height() - 400;
        int w = (k->size.width() * h) / k->size.height();
        k->scaledSize.setWidth(w);
        k->scaledSize.setHeight(h);
    }

    setModal(true);
    setWindowTitle(tr("Storyboard Settings"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "storyboard.png")));

    k->layout = new QHBoxLayout(this);
    k->formLayout = new QVBoxLayout;

    setListComponent();
    setPreviewScreen();
    setStoryForm();
    setSceneForm();

    QPushButton *pdfButton = new QPushButton(tr("&PDF"));
    pdfButton->setToolTip(tr("Export as PDF"));
    connect(pdfButton, SIGNAL(clicked()), this, SLOT(exportAsPDF()));

    QPushButton *htmlButton = new QPushButton(tr("&HTML"));
    htmlButton->setToolTip(tr("Export as HTML"));
    connect(htmlButton, SIGNAL(clicked()), this, SLOT(exportAsHTML()));

    QPushButton *closeButton = new QPushButton(QIcon(QPixmap(THEME_DIR + "icons" + QDir::separator() + "close.png")), "");
    closeButton->setToolTip(tr("Close"));
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(pdfButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(htmlButton, QDialogButtonBox::ActionRole);

    if (k->isNetworked) {
        QPushButton *postButton = new QPushButton(tr("&Post"));
        connect(postButton, SIGNAL(clicked()), this, SLOT(postStoryboardAtServer()));
        buttonBox->addButton(postButton, QDialogButtonBox::ActionRole);
    }

    buttonBox->addButton(closeButton, QDialogButtonBox::ActionRole);

    k->formLayout->addWidget(new TSeparator());
    k->formLayout->addWidget(buttonBox);

    k->layout->addLayout(k->formLayout);

    thumbnailGenerator();
}

TupStoryBoardDialog::~TupStoryBoardDialog()
{
}

void TupStoryBoardDialog::setListComponent()
{
    k->list = new QListWidget(this);
    k->list->setViewMode(QListView::IconMode);
    k->list->setWrapping(false);
    k->list->setFlow(QListView::TopToBottom);
    k->list->setIconSize(QSize(96, (k->scaledSize.height() * 96) / k->scaledSize.width()));
    k->list->setMovement(QListView::Static);
    k->list->setFixedWidth(130);
    k->list->setSpacing(12);

    k->layout->addWidget(k->list);

    connect(k->list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(updateForm(QListWidgetItem *, QListWidgetItem*)));
}

void TupStoryBoardDialog::setPreviewScreen()
{
    QPixmap pixmap = QPixmap(k->scaledSize.width(), k->scaledSize.height());
    pixmap.fill();

    k->screenLabel = new QLabel;
    k->screenLabel->setAlignment(Qt::AlignHCenter);
    k->screenLabel->setPixmap(pixmap);
    k->formLayout->addWidget(k->screenLabel);
}

void TupStoryBoardDialog::setStoryForm()
{
    k->storyPanel = new QWidget;

    QBoxLayout *sceneLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->storyPanel);

    QFont font = this->font();
    font.setPointSize(10);
    font.setBold(true);
    QLabel *mainTitle = new QLabel(tr("Storyboard General Information"));
    mainTitle->setFont(font);
    mainTitle->setAlignment(Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(tr("Title"));
    k->titleEdit = new QLineEdit("");
    k->titleEdit->setLocale(k->utf);

    titleLabel->setBuddy(k->titleEdit);

    QLabel *authorLabel = new QLabel(tr("Author"));
    k->authorEdit = new QLineEdit("");
    k->authorEdit->setLocale(k->utf);
    authorLabel->setBuddy(k->authorEdit);

    QLabel *summaryLabel = new QLabel(tr("Summary"));
    k->summaryEdit = new QTextEdit;
    k->summaryEdit->setLocale(k->utf);
    k->summaryEdit->setAcceptRichText(false);
    k->summaryEdit->setFixedHeight(80);
    k->summaryEdit->setText("");

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(k->titleEdit);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(authorLabel);
    middleLayout->addWidget(k->authorEdit);

    sceneLayout->addWidget(mainTitle);
    sceneLayout->addLayout(topLayout);

    if (k->isNetworked) {
        QLabel *topicsLabel = new QLabel(tr("Topics"));
        k->topicsEdit = new QLineEdit("");
        k->topicsEdit->setLocale(k->utf);
        topicsLabel->setBuddy(k->topicsEdit);

        QHBoxLayout *topicsLayout = new QHBoxLayout;
        topicsLayout->addWidget(topicsLabel);
        topicsLayout->addWidget(k->topicsEdit);

        sceneLayout->addLayout(topicsLayout);
    }

    sceneLayout->addLayout(middleLayout);
    sceneLayout->addWidget(summaryLabel);
    sceneLayout->addWidget(k->summaryEdit);

    k->formLayout->addWidget(k->storyPanel);
}

void TupStoryBoardDialog::setSceneForm()
{
    k->scenePanel = new QWidget;

    QBoxLayout *sceneLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->scenePanel);
    sceneLayout->setAlignment(Qt::AlignVCenter | Qt::AlignBottom);

    QFont font = this->font();
    font.setPointSize(10);
    font.setBold(true);
    k->sceneLabel = new QLabel(tr("Scene Information"));
    k->sceneLabel->setFont(font);
    k->sceneLabel->setAlignment(Qt::AlignHCenter);

    QLabel *titleLabel = new QLabel(tr("Title"));
    k->sceneTitleEdit = new QLineEdit("");
    k->sceneTitleEdit->setLocale(k->utf);
    titleLabel->setBuddy(k->sceneTitleEdit);

    QLabel *durationLabel = new QLabel(tr("Duration"));
    k->sceneDurationEdit = new QLineEdit("");
    k->sceneDurationEdit->setLocale(k->utf);
    durationLabel->setBuddy(k->sceneDurationEdit);

    QLabel *descLabel = new QLabel(tr("Description"));

    k->sceneDescriptionEdit = new QTextEdit;
    k->sceneDescriptionEdit->setLocale(k->utf);
    k->sceneDescriptionEdit->setAcceptRichText(false);
    k->sceneDescriptionEdit->setFixedHeight(80);
    k->sceneDescriptionEdit->setText("");

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(titleLabel);
    topLayout->addWidget(k->sceneTitleEdit);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addWidget(durationLabel);
    middleLayout->addWidget(k->sceneDurationEdit);

    sceneLayout->addWidget(k->sceneLabel);
    sceneLayout->addLayout(topLayout);
    sceneLayout->addLayout(middleLayout);
    sceneLayout->addWidget(descLabel);
    sceneLayout->addWidget(k->sceneDescriptionEdit);

    k->formLayout->addWidget(k->scenePanel);

    k->scenePanel->hide();
}

void TupStoryBoardDialog::addScene(const QString &label, const QIcon &icon)
{
    QListWidgetItem *sceneItem = new QListWidgetItem(k->list);
    sceneItem->setIcon(icon);
    sceneItem->setText(label);
    sceneItem->setTextAlignment(Qt::AlignHCenter);
    sceneItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    if (label.compare(tr("Cover")) == 0)
        sceneItem->setSelected(true);
}

void TupStoryBoardDialog::thumbnailGenerator()
{
    int height = (k->scaledSize.height() * 96) / k->scaledSize.width();
    QPixmap pixmap = QPixmap(96, height); 
    pixmap.fill();

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    QFont font = this->font();
    font.setPointSize(8);
    font.setBold(true);
    painter.setFont(font);
    // painter.setFont(QFont("Arial", 8, QFont::Bold));
    QRectF rect(QPointF(0, 0), QSizeF(96, height));
    painter.drawText(rect, Qt::AlignCenter, tr("Storyboard"));
    painter.setPen(QColor(230, 230, 230));
    QRectF rectangle(0, 0, 95, height - 1);
    painter.drawRect(rectangle);

    QIcon icon = QIcon(pixmap); 
    addScene(tr("Cover"), icon);

    int framesCount = k->scene->framesCount();
    if (k->storyboard->size() == 0)
        k->storyboard->init(0, framesCount);

    k->path = QDir::tempPath() + QDir::separator() + TAlgorithm::randomString(8) + QDir::separator();
    QDir().mkpath(k->path);

    for (int i=0; i < framesCount; i++) {
         QString fileName = k->path + "scene" + QString::number(i);
         bool isOk = k->imagePlugin->exportFrame(i, k->bgColor, fileName, k->scene, k->size, k->library);
         fileName += ".png";
         QPixmap resized(fileName);
         resized = resized.scaledToWidth(k->scaledSize.width(), Qt::SmoothTransformation);
         resized.save(fileName);

         if (isOk) {
             QPixmap pixmap(fileName);
             QPainter painter(&pixmap);
             painter.setPen(Qt::darkGray);
             QRectF rectangle(0, 0, k->scaledSize.width()-1, k->scaledSize.height()-1);
             painter.drawRect(rectangle);
             pixmap.scaledToWidth(96, Qt::SmoothTransformation);

             QIcon icon(pixmap);
             QString label = tr("Scene") + " " + QString::number(i);  
             addScene(label, icon);
         }
    }
}

void TupStoryBoardDialog::updateForm(QListWidgetItem *current, QListWidgetItem *previous)
{
    int previousIndex = k->list->row(previous);
    k->currentIndex = k->list->row(current);
    QPixmap pixmap;

    if (k->currentIndex > 0) {
        int index = k->currentIndex - 1;
        k->sceneLabel->setText(tr("Scene No %1 - Information").arg(QString::number(index)));
        QString fileName = k->path + "scene" + QString::number(index) + ".png";
        pixmap = QPixmap(fileName);

        if (previousIndex == 0) {
            k->storyPanel->hide();
            k->scenePanel->show();

            k->storyboard->setStoryTitle(getStoryTitle());

            if (k->isNetworked)
                k->storyboard->setStoryTopics(getStoryTopics());
            else
                k->storyboard->setStoryTopics(""); 

            k->storyboard->setStoryAuthor(getStoryAuthor());
            k->storyboard->setStorySummary(getStorySummary());
        } else {
            previousIndex--;
            k->storyboard->setSceneTitle(previousIndex, getSceneTitle());
            k->storyboard->setSceneDuration(previousIndex, getSceneDuration());
            k->storyboard->setSceneDescription(previousIndex, getSceneDescription());
        }

        k->sceneTitleEdit->setText(k->storyboard->sceneTitle(index));
        k->sceneDurationEdit->setText(k->storyboard->sceneDuration(index));
        k->sceneDescriptionEdit->setPlainText(k->storyboard->sceneDescription(index));

    } else {
        if (previousIndex != 0) {
            pixmap = QPixmap(k->scaledSize.width(), k->scaledSize.height());
            pixmap.fill();

            QPainter painter(&pixmap);
            painter.setPen(Qt::black);
            QFont font = this->font();
            int fontSize = k->scaledSize.width()*30/520;
            font.setPointSize(fontSize);
            font.setBold(true);
            painter.setFont(font);
            // painter.setFont(QFont("Arial", k->scaledSize.width()*30/520, QFont::Bold));

            QRectF rect(QPointF(0, (k->scaledSize.height()-150)/2), QSizeF(k->scaledSize.width(), 150));
            painter.drawText(rect, Qt::AlignCenter, tr("Storyboard"));
            painter.setPen(Qt::lightGray);
            QRectF rectangle(5, 5, k->scaledSize.width() - 10, k->scaledSize.height() - 10);
            painter.drawRect(rectangle);

            k->scenePanel->hide();
            k->storyPanel->show();

            if (previousIndex > 0) {
                k->storyboard->setSceneTitle(previousIndex - 1, getSceneTitle());
                k->storyboard->setSceneDuration(previousIndex - 1, getSceneDuration());
                k->storyboard->setSceneDescription(previousIndex - 1, getSceneDescription());
            }

            k->titleEdit->setText(k->storyboard->storyTitle());

            if (k->isNetworked)
                k->topicsEdit->setText(k->storyboard->storyTopics());

            k->authorEdit->setText(k->storyboard->storyAuthor());
            k->summaryEdit->setPlainText(k->storyboard->storySummary());
        }
    }

    k->screenLabel->setPixmap(pixmap);
}

void TupStoryBoardDialog::createHTMLFiles(const QString &path, DocType type)
{
    if (k->scaledSize.width() <= 520) {
        QDir directory(k->path);
        QStringList files = directory.entryList();
        for (int i = 0; i < files.size(); ++i) {
             QString file = files.at(i).toLocal8Bit().constData();
             if (file != "." && file != "..") {
                 QString target = path + QDir::separator() + file;
                 if (QFile::exists(target))
                     QFile::remove(target);       
                 QFile::copy(k->path + file, target);
             }
        }
    } else {
        QDir directory(k->path);
        QStringList files = directory.entryList();
        for (int i = 0; i < files.size(); ++i) {
             QString file = files.at(i).toLocal8Bit().constData();
             QPixmap pixmap(k->path + file); 

             QString destination = path + QDir::separator() + file;

             if (QFile::exists(destination))
                 QFile::remove(destination); 

             QPixmap resized;
             resized = pixmap.scaledToWidth(520, Qt::SmoothTransformation);
             resized.save(destination);
        }
    }

    QString base = kAppProp->shareDir() + "data" + QDir::separator() + "storyboard" + QDir::separator();

    if (type == HTML) 
        QFile::copy(base + "tupi.html.css", path + QDir::separator() + "tupi.css");
    else
        QFile::copy(base + "tupi.pdf.css", path + QDir::separator() + "tupi.css");

    QString index = path + QDir::separator() + "index.html";

    if (QFile::exists(index))
        QFile::remove(index);  

    QFile file(index);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "<html>\n";
    out << "<head>\n";
    QString record = k->storyboard->storyTitle();
    if (record.length() == 0)
        record = "&nbsp;";
    out << "<title>" << record << "</title>\n";
    out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"tupi.css\" media=\"screen\" />\n";
    out << "</head>\n";
    out << "<body>\n";
    out << "<div id=\"header\">\n";
    out << "<div id=\"title\">Storyboard</div>\n";
    out << "<div id=\"item\">\n";
    out << "     <div id=\"item-header\">Title:</div>\n";
    out << "     <div id=\"item-data\">" << record << "</div>\n";
    out << "     </div>\n";
    out << "<div id=\"item\">\n";
    out << "     <div id=\"item-header\">Author:</div>\n";
    record = k->storyboard->storyAuthor();
    if (record.length() == 0)
        record = "&nbsp;";
    out << "     <div id=\"item-data\">" << record << "</div>\n";
    out << "</div>\n";
    out << "<div id=\"item\">\n";
    out << "     <div id=\"item-header\">Summary:</div>\n";
    record = k->storyboard->storySummary();
    if (record.length() == 0)
        record = "&nbsp;";
    out << "     <div id=\"item-data\">" << record << "</div>\n";
    out << "</div>\n";
    out << "<div id=\"item\">\n";
    out << "     <div id=\"item-header\">Scenes Total:</div>\n";
    out << "     <div id=\"item-data\">" << QString::number(k->storyboard->size()) << "</div>\n";
    out << "</div>\n";
    out << "</div>\n";
    if (type == PDF) {
        out << "<div id=\"page-break\">\n";
        out << "</div>\n";
    }

    int scenes = k->storyboard->size();
    for (int i=0; i < scenes; i++) {
         out << "<div id=\"scene\">\n";
         QString image = "<img src=\"scene" + QString::number(i) + ".png\" />\n";
         out << image;
         out << "<div id=\"paragraph\">\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Title:</div>\n";
         record = k->storyboard->sceneTitle(i);
         if (record.length() == 0)
             record = "&nbsp;";
         out << " <div id=\"scene-data\">" << record << "</div>\n";
         out << "</div>\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Duration:</div>\n";
         record = k->storyboard->sceneDuration(i);
         if (record.length() == 0)
             record = "&nbsp;";
         out << " <div id=\"scene-data\">" << record << "</div>\n";
         out << "</div>\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Description:</div>\n";
         record = k->storyboard->sceneDescription(i);
         if (record.length() == 0)
             record = "&nbsp;";
         out << " <div id=\"scene-data\">" << record << "</div>\n";
         out << "</div>\n";
         out << "</div>\n";
         out << "</div>\n";
         if (type == PDF) {
             if (i < (k->storyboard->size() - 1)) {
                 out << "<div id=\"page-break\">\n";
                 out << "</div>\n";
             }
         }
    }
    out << "</body>\n";
    out << "</html>";

    file.close(); 
}

void TupStoryBoardDialog::exportAsHTML()
{
    saveLastComponent();

    QString path = QFileDialog::getExistingDirectory(this, tr("Choose a directory..."), QDir::homePath(),
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!path.isEmpty()) {
        createHTMLFiles(path, HTML);
        TOsd::self()->display(tr("Info"), tr("Storyboard exported successfully!"), TOsd::Info);
    }
}

void TupStoryBoardDialog::exportAsPDF()
{
    saveLastComponent();

    QString path = QDir::tempPath() + QDir::separator() + TAlgorithm::randomString(8) + QDir::separator();
    QDir().mkpath(path);
    if (!path.isEmpty())
        createHTMLFiles(path, PDF);

    QString pdfPath = QFileDialog::getSaveFileName(this, tr("Save PDF file"), QDir::homePath(), tr("PDF file (*.pdf)"));

    if (!pdfPath.isEmpty()) {
        QFile file(path + "index.html");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        QString htmlContent;
        QTextStream in(&file);
        htmlContent = in.readAll();

        QPrinter printer;
        printer.setPageSize(QPrinter::A4);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(pdfPath);

        QTextBrowser *document = new QTextBrowser;
        QStringList paths;
        paths << path;
        document->setSearchPaths(paths);
        document->setHtml(htmlContent);
        document->print(&printer);
        delete document;
        cleanDirectory(path);

        TOsd::self()->display(tr("Info"), tr("Storyboard exported successfully!"), TOsd::Info);
    }
}

void TupStoryBoardDialog::postStoryboardAtServer()
{
    #ifdef K_DEBUG
        QString msg = "TupStoryBoardDialog::postStoryBoardAtServer() - Posting in Tupitube!";
        #ifdef Q_OS_WIN
            qWarning() << msg;
        #else
            tWarning() << msg;
        #endif
    #endif

    saveLastComponent();

    // SQA: This "save call" line should be enhanced
    emit updateStoryboard(k->storyboard, k->sceneIndex);

    emit postStoryboard(k->sceneIndex);
}

void TupStoryBoardDialog::saveLastComponent()
{
    if (k->currentIndex == 0) {
        k->storyboard->setStoryTitle(getStoryTitle());
        k->storyboard->setStoryAuthor(getStoryAuthor());
        if (k->isNetworked)
            k->storyboard->setStoryTopics(getStoryTopics());
        else
            k->storyboard->setStoryTopics("");
        k->storyboard->setStorySummary(getStorySummary());
    } else {
        k->storyboard->setSceneTitle(k->currentIndex - 1, getSceneTitle());
        k->storyboard->setSceneDuration(k->currentIndex - 1, getSceneDuration());
        k->storyboard->setSceneDescription(k->currentIndex - 1, getSceneDescription());
    }
}

void TupStoryBoardDialog::closeDialog()
{
    saveLastComponent();
    cleanDirectory(k->path);

    if (k->isNetworked)
        emit updateStoryboard(k->storyboard, k->sceneIndex);

    close();
}

QString TupStoryBoardDialog::getStoryTitle() const
{
    return QString::fromUtf8(k->titleEdit->text().toUtf8());
}

QString TupStoryBoardDialog::getStoryAuthor() const
{
    return QString::fromUtf8(k->authorEdit->text().toUtf8());
}

QString TupStoryBoardDialog::getStoryTopics() const
{
    return QString::fromUtf8(k->topicsEdit->text().toUtf8());
}

QString TupStoryBoardDialog::getStorySummary() const
{
    return QString::fromUtf8(k->summaryEdit->toPlainText().toUtf8());
}

QString TupStoryBoardDialog::getSceneTitle() const
{
    return QString::fromUtf8(k->sceneTitleEdit->text().toUtf8());
}

QString TupStoryBoardDialog::getSceneDuration() const
{
    return QString::fromUtf8(k->sceneDurationEdit->text().toUtf8());
}

QString TupStoryBoardDialog::getSceneDescription() const
{
    return QString::fromUtf8(k->sceneDescriptionEdit->toPlainText().toUtf8());
}

void TupStoryBoardDialog::exportStoyrboard(const QString &type)
{
    if (type.compare(tr("PDF")) == 0) {
        exportAsPDF();
    } else if (type.compare(tr("Html")) == 0) {
               exportAsHTML();
    }
}

void TupStoryBoardDialog::cleanDirectory(const QString &path)
{
    QDir dir(path);
    QStringList files = dir.entryList();
    for (int i = 0; i < files.size(); ++i) {
         QString file = files.at(i).toLocal8Bit().constData();
         if (file != "." && file != "..")
             QFile::remove(path + file);
    }
    dir.rmdir(path);
}

