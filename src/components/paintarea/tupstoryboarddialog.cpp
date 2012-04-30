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

struct TupStoryBoardDialog::Private
{
    TupExportInterface *imagePlugin;
    QColor bgColor;
    QSize size;
    TupScene *scene;
    TupStoryboard *story;

    QBoxLayout *layout;
    QVBoxLayout *formLayout;

    QWidget *storyPanel;
    QWidget *scenePanel;

    QListWidget *list;
    QLabel *screenLabel;

    QLineEdit *titleEdit;
    QLineEdit *authorEdit;
    QTextEdit *summaryEdit;

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
    k->story = k->scene->storyboard();

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
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

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

    QLabel *summaryLabel = new QLabel(tr("Description"));
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
    QLabel *mainTitle = new QLabel(tr("Scene Information"));
    mainTitle->setFont(font);
    mainTitle->setAlignment(Qt::AlignHCenter);

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

    sceneLayout->addWidget(mainTitle);
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
    pixmap.fill(Qt::lightGray);
    QIcon icon = QIcon(pixmap); 
    addScene(tr("Cover"), icon);

    QString path = QDir::tempPath() + QDir::separator() + "frame";
    for (int i=0; i < k->scene->framesTotal(); i++) {
         QString fileName = path + QString::number(i);
         tError() << "TupStoryBoardDialog::thumbnailGenerator() - Exporting frame -> " << fileName;
         bool isOk = k->imagePlugin->exportFrame(i, k->bgColor, fileName, k->scene, k->size);
         if (isOk) {
             QPixmap pixmap(fileName + ".png");
             pixmap.scaledToWidth(96, Qt::SmoothTransformation);
             pixmap.scaledToHeight(70, Qt::SmoothTransformation);
             QIcon icon(pixmap);
             QString label = "Scene " + QString::number(i);  
             if (k->story->sceneTitle(i).length() > 0)
                 label = k->story->sceneTitle(i);
             addScene(label, icon);
         }
    }
}

void TupStoryBoardDialog::updateForm(QListWidgetItem *current, QListWidgetItem *previous)
{
    int previousIndex = k->list->row(previous);
    int index = k->list->row(current);
    tError() << "TupStoryBoardDialog::updateForm() -> Scene has changed! : " << index;
    tError() << "TupStoryBoardDialog::updateForm() -> Previous index : " << previousIndex;
    QPixmap pixmap;

    if (index > 0) {
        index = index - 1;
        QString fileName = QDir::tempPath() + QDir::separator() + "frame";
        fileName += QString::number(index) + ".png";
        pixmap = QPixmap(fileName);
        tError() << "TupStoryBoardDialog::updateForm() - filename: " << fileName;
        pixmap.scaledToWidth(520, Qt::SmoothTransformation);
        pixmap.scaledToHeight(380, Qt::SmoothTransformation);

        if (previousIndex == 0) {
            k->storyPanel->hide();
            k->scenePanel->show();
        }
    } else {
        if (previousIndex != 0) {
            pixmap = QPixmap(520, 380);
            pixmap.fill(Qt::lightGray);

            k->scenePanel->hide();
            k->storyPanel->show();
        }
    }

    k->screenLabel->setPixmap(pixmap);
}

void TupStoryBoardDialog::saveStoryBoard()
{
    tError() << "TupStoryBoardDialog::saveStoryBoard() - Save the story!";
}

void TupStoryBoardDialog::exportStoryBoard()
{
    tError() << "TupStoryBoardDialog::exportStoryBoard() - Export the story!";
}
