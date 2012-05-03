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

#include "tupstoryboarddialog.h"
#include "tupstoryboard.h"
#include "tapplicationproperties.h"
#include "tglobal.h"
#include "tconfig.h"
#include "tseparator.h"
#include "tupscene.h"
#include "talgorithm.h"
#include "tosd.h"
#include "tdebug.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QIcon>
#include <QPixmap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QDir>
#include <QPushButton>
#include <QPainter>
#include <QFileDialog>

struct TupStoryBoardDialog::Private
{
    TupExportInterface *imagePlugin;
    QColor bgColor;
    QSize size;
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
    QLineEdit *authorEdit;
    QTextEdit *summaryEdit;

    QLabel *sceneLabel;
    QLineEdit *sceneTitleEdit;
    QLineEdit *sceneDurationEdit;
    QTextEdit *sceneDescriptionEdit;
};

TupStoryBoardDialog::TupStoryBoardDialog(TupExportInterface *imagePlugin, const QColor &color, const QSize &size, TupScene *scene, QWidget *parent) : QDialog(parent), k(new Private)
{
    k->imagePlugin = imagePlugin;
    k->bgColor = color;
    k->size = size;
    k->scene = scene;
    k->storyboard = k->scene->storyboard();

    setModal(true);
    setWindowTitle(tr("Storyboard Settings"));
    setWindowIcon(QIcon(QPixmap(THEME_DIR + "icons/storyboard.png")));

    k->layout = new QHBoxLayout(this);
    k->formLayout = new QVBoxLayout;

    setListComponent();
    setPreviewScreen();
    setStoryForm();
    setSceneForm();

    QPushButton *saveButton = new QPushButton(tr("&Save"));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveStoryBoard()));

    QPushButton *exportButton = new QPushButton(tr("&Export"));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(exportStoryBoard()));

    QPushButton *closeButton = new QPushButton(tr("&Close"));
    closeButton->setDefault(true);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
    buttonBox->addButton(saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(exportButton, QDialogButtonBox::ActionRole);
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
    k->list->setIconSize(QSize(96, 84));
    k->list->setMovement(QListView::Static);
    k->list->setFixedWidth(130);
    k->list->setSpacing(12);

    k->layout->addWidget(k->list);

    connect(k->list, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(updateForm(QListWidgetItem *, QListWidgetItem*)));
}

void TupStoryBoardDialog::setPreviewScreen()
{
    QPixmap pixmap = QPixmap(520, 380);
    pixmap.fill();

    k->screenLabel = new QLabel;
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
    titleLabel->setBuddy(k->titleEdit);

    QLabel *authorLabel = new QLabel(tr("Author"));
    k->authorEdit = new QLineEdit("");
    authorLabel->setBuddy(k->authorEdit);

    QLabel *summaryLabel = new QLabel(tr("Summary"));
    k->summaryEdit = new QTextEdit;
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
    titleLabel->setBuddy(k->sceneTitleEdit);

    QLabel *durationLabel = new QLabel(tr("Duration"));
    k->sceneDurationEdit = new QLineEdit("");
    durationLabel->setBuddy(k->sceneDurationEdit);

    QLabel *descLabel = new QLabel(tr("Description"));

    k->sceneDescriptionEdit = new QTextEdit;
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
    QPixmap pixmap = QPixmap(96, 70); 
    pixmap.fill();

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 8, QFont::Bold));
    QRectF rect(QPointF(0, 0), QSizeF(96, 70));
    painter.drawText(rect, Qt::AlignCenter, tr("Storyboard"));
    painter.setPen(QColor(230, 230, 230));
    QRectF rectangle(0, 0, 95, 69);
    painter.drawRect(rectangle);

    QIcon icon = QIcon(pixmap); 
    addScene(tr("Cover"), icon);

    int framesTotal = k->scene->framesTotal();
    if (k->storyboard->size() == 0)
        k->storyboard->init(0, framesTotal);

    // SQA: This code should be temporary
    if (k->storyboard->size() < framesTotal)
        k->storyboard->init(k->storyboard->size(), framesTotal);

    // SQA: This code should be temporary
    if (k->storyboard->size() > framesTotal)
        k->storyboard->remove(k->storyboard->size() - framesTotal);

    k->path = QDir::tempPath() + QDir::separator() + TAlgorithm::randomString(8) + QDir::separator();
    QDir().mkpath(k->path);

    for (int i=0; i < framesTotal; i++) {
         QString fileName = k->path + "scene" + QString::number(i);
         bool isOk = k->imagePlugin->exportFrame(i, k->bgColor, fileName, k->scene, k->size);
         if (isOk) {
             QPixmap pixmap(fileName + ".png");
             QPainter painter(&pixmap);
             painter.setPen(Qt::darkGray);
             QRectF rectangle(0, 0, 519, 379);
             painter.drawRect(rectangle);

             pixmap.scaledToWidth(96, Qt::SmoothTransformation);
             pixmap.scaledToHeight(70, Qt::SmoothTransformation);

             QIcon icon(pixmap);
             QString label = "Scene " + QString::number(i);  
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
        pixmap.scaledToWidth(520, Qt::SmoothTransformation);
        pixmap.scaledToHeight(380, Qt::SmoothTransformation);

        if (previousIndex == 0) {
            k->storyPanel->hide();
            k->scenePanel->show();

            k->storyboard->setStoryTitle(k->titleEdit->text());
            k->storyboard->setStoryAuthor(k->authorEdit->text());
            k->storyboard->setStorySummary(k->summaryEdit->toPlainText());
        } else {
            previousIndex--;
            k->storyboard->setSceneTitle(previousIndex, k->sceneTitleEdit->text());
            k->storyboard->setSceneDuration(previousIndex, k->sceneDurationEdit->text());
            k->storyboard->setSceneDescription(previousIndex, k->sceneDescriptionEdit->toPlainText());
        }

        k->sceneTitleEdit->setText(k->storyboard->sceneTitle(index));
        k->sceneDurationEdit->setText(k->storyboard->sceneDuration(index));
        k->sceneDescriptionEdit->setPlainText(k->storyboard->sceneDescription(index));

    } else {
        if (previousIndex != 0) {
            pixmap = QPixmap(520, 380);
            pixmap.fill();

            QPainter painter(&pixmap);
            painter.setPen(Qt::black);
            painter.setFont(QFont("Arial", 30, QFont::Bold));
            QRectF rect(QPointF(0, (k->size.height()-150)/2), QSizeF(520, 150));
            painter.drawText(rect, Qt::AlignCenter, tr("Storyboard"));
            painter.setPen(Qt::lightGray);
            QRectF rectangle(5, 5, 510, 370);
            painter.drawRect(rectangle);

            k->scenePanel->hide();
            k->storyPanel->show();

            if (previousIndex > 0) {
                k->storyboard->setSceneTitle(previousIndex - 1, k->sceneTitleEdit->text());
                k->storyboard->setSceneDuration(previousIndex - 1, k->sceneDurationEdit->text());
                k->storyboard->setSceneDescription(previousIndex - 1, k->sceneDescriptionEdit->toPlainText());
            }

            k->titleEdit->setText(k->storyboard->storyTitle());
            k->authorEdit->setText(k->storyboard->storyAuthor());
            k->summaryEdit->setPlainText(k->storyboard->storySummary());
        }
    }

    k->screenLabel->setPixmap(pixmap);
}

void TupStoryBoardDialog::saveStoryBoard()
{
    if (k->currentIndex == 0) {
        k->storyboard->setStoryTitle(k->titleEdit->text());
        k->storyboard->setStoryAuthor(k->authorEdit->text());
        k->storyboard->setStorySummary(k->summaryEdit->toPlainText());
    } else {
        k->storyboard->setSceneTitle(k->currentIndex - 1, k->sceneTitleEdit->text());
        k->storyboard->setSceneDuration(k->currentIndex - 1, k->sceneDurationEdit->text());
        k->storyboard->setSceneDescription(k->currentIndex - 1, k->sceneDescriptionEdit->toPlainText());
    }

    emit saveStoryboard(k->storyboard);
}

void TupStoryBoardDialog::exportStoryBoard()
{
    QString dir = getenv("HOME");
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose the storyboard directory..."), dir,
                                                     QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty())
        return;

    QDir directory(k->path);
    QStringList files = directory.entryList();
    for (int i = 0; i < files.size(); ++i) {
         QString file = files.at(i).toLocal8Bit().constData();
         if (file != "." && file != "..")
             QFile::copy(k->path + file, path + QDir::separator() + file);
    }

    QFile::copy(kAppProp->shareDir() + "data/storyboard/tupi.css", path + QDir::separator() + "tupi.css");

    QFile file(path + QDir::separator() + "index.html");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "<html>\n";
    out << "<head>\n";
    out << "<title>" << k->storyboard->storyTitle() << "</title>\n";
    out << "<link rel=\"stylesheet\" type=\"text/css\" href=\"tupi.css\" media=\"screen\" />\n";
    out << "</head>\n";
    out << "<body>\n";
    out << "<div id=\"header\">\n";
    out << "<div id=\"title\">Storyboard</div>\n";
    out << "<div id=\"item\"><div id=\"item-header\">Title:</div>\n";
    out << "<div id=\"item-data\">" << k->storyboard->storyTitle() << "</div></div>\n";
    out << "<div id=\"item\"><div id=\"item-header\">Author:</div>\n";
    out << "<div id=\"item-data\">" << k->storyboard->storyAuthor() << "</div></div>\n";
    out << "<div id=\"item\"><div id=\"item-header\">Summary:</div>\n";
    out << "<div id=\"item-data\">" << k->storyboard->storySummary() << "</div></div>\n";
    out << "<div id=\"item\"><div id=\"item-header\">Scenes Total:</div>\n";
    out << "<div id=\"item-data\">" << QString::number(k->storyboard->size()) << "</div></div>\n";
    out << "</div>\n";

    for (int i=0; i < k->storyboard->size(); i++) {
         out << "<div id=\"scene\">\n";
         out << "<div id=\"title\">" << "Scene No. " << QString::number(i+1) << "</div>\n";
         QString image = "<img src=\"scene" + QString::number(i) + ".png\" />\n";
         out << image;
         out << "<div id=\"paragraph\">\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Title:</div>\n";
         out << " <div id=\"scene-data\">" << k->storyboard->sceneTitle(i) << "</div>\n";
         out << "</div>\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Duration:</div>\n";
         out << " <div id=\"scene-data\">" << k->storyboard->sceneDuration(i) << "</div>\n";
         out << "</div>\n";
         out << "<div id=\"scene-item\">\n";
         out << " <div id=\"scene-header\">Description:</div>\n";
         out << " <div id=\"scene-data\">" << k->storyboard->sceneDescription(i)  << "</div>\n";
         out << "</div>\n";
         out << "</div>\n";
         out << "</div>\n";
    }
    out << "</body>\n";
    out << "</html>";

    file.close(); 

    TOsd::self()->display(tr("Info"), tr("Storyboard exported successfully!"), TOsd::Info);
}

void TupStoryBoardDialog::closeDialog()
{
    QDir dir(k->path);
    QStringList files = dir.entryList();
    for (int i = 0; i < files.size(); ++i) {
         QString file = files.at(i).toLocal8Bit().constData();
         if (file != "." && file != "..")
             QFile::remove(k->path + file);
    }
    dir.rmdir(k->path); 

    close();
}
