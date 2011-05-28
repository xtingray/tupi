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

#include "tweenerpanel.h"
#include "positionsettings.h"
#include "kdebug.h"
#include "kradiobuttongroup.h"
// #include "ktitemtweener.h"
// #include "kttweenerstep.h"
#include "kimagebutton.h"
#include "kseparator.h"
#include "tweenertable.h"
#include "kosd.h"

#include <QLabel>
#include <QLineEdit>
#include <QBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHeaderView>

struct TweenerPanel::Private
{
    QWidget *optionsPanel;
    QWidget *tweenerTablePanel; QWidget *buttonsPanel;

    QList<QWidget*> *panelList;

    QBoxLayout *layout;
    Mode mode;
    QLineEdit *input;
    KRadioButtonGroup *options;
    QComboBox *comboInit;
    QComboBox *comboEnd;

    TweenerTable *tweenerTable;
    int currentTweenIndex;
    QList<TweenerPanel::TweenerType> tweenerList;

    PositionSettings *positionPanel;

    QLabel *totalLabel;
    int totalSteps;

    bool selectionDone;
    bool propertiesOpen;

    KImageButton *applyButton;
    KImageButton *closeButton;
};

TweenerPanel::TweenerPanel(QWidget *parent) : QWidget(parent), k(new Private)
{
    k->selectionDone = false;
    k->propertiesOpen = false;
    k->totalSteps = 0;

    k->layout = new QBoxLayout(QBoxLayout::TopToBottom, this);
    k->layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    setFont(QFont("Arial", 8, QFont::Normal, false));

    QLabel *nameLabel = new QLabel(tr("Name") + ": ");
    k->input = new QLineEdit;

    QHBoxLayout *nameLayout = new QHBoxLayout;
    nameLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    nameLayout->setMargin(0);
    nameLayout->setSpacing(0);
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(k->input);

    k->layout->addLayout(nameLayout);

    setOptionsPanel();

    setTweenerTableForm();
    loadTweenComponents();

    // k->layout->addSpacing(10);
    setButtonsPanel();
    k->layout->setSpacing(0);

    activateMode(TweenerPanel::Selection);
}

TweenerPanel::~TweenerPanel()
{
    delete k;
}

void TweenerPanel::setOptionsPanel()
{
    k->optionsPanel = new QWidget;

    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->optionsPanel);
    //innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    innerLayout->setMargin(0);
    innerLayout->setSpacing(0);

    k->options = new KRadioButtonGroup(tr("Options"), Qt::Vertical);
    k->options->addItem(tr("Select object"), 0);
    k->options->addItem(tr("Set Tweeners"), 1);
    connect(k->options, SIGNAL(clicked(int)), this, SLOT(emitOptionChanged(int)));

    innerLayout->addWidget(k->options);

    k->layout->addWidget(k->optionsPanel);
    activeOptionsPanel(true);
}

void TweenerPanel::activeOptionsPanel(bool enable)
{
    if (enable && !k->optionsPanel->isVisible())
        k->optionsPanel->show();
    else
        k->optionsPanel->hide();
}

void TweenerPanel::setTweenerTableForm()
{
    k->tweenerTablePanel = new QWidget;

    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->tweenerTablePanel);
    innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QLabel *startingLabel = new QLabel(tr("Tweeners") + ": ");

    QHBoxLayout *startLayout = new QHBoxLayout;
    startLayout->setAlignment(Qt::AlignHCenter);
    startLayout->setMargin(0);
    startLayout->setSpacing(0);
    startLayout->addWidget(startingLabel);

    k->tweenerTable = new TweenerTable;
    connect(k->tweenerTable, SIGNAL(callTweenerSettings(int)), this, SLOT(showTweenSettings(int)));

    innerLayout->addLayout(startLayout);
    innerLayout->addWidget(k->tweenerTable);

    k->layout->addWidget(k->tweenerTablePanel);
    activeTweenerTableForm(false);
}

void TweenerPanel::activeTweenerTableForm(bool enable)
{
    if (enable && !k->tweenerTablePanel->isVisible()) {
        k->propertiesOpen = true;
        k->tweenerTablePanel->show();
    } else {
        k->propertiesOpen = false;
        k->tweenerTablePanel->hide();
    }
}

void TweenerPanel::setButtonsPanel()
{
    k->buttonsPanel = new QWidget;

    QBoxLayout *innerLayout = new QBoxLayout(QBoxLayout::TopToBottom, k->buttonsPanel);
    innerLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    k->applyButton = new KImageButton(QPixmap(THEME_DIR + "icons/save.png"), 22);
    k->applyButton->setDisabled(true);
    connect(k->applyButton, SIGNAL(clicked()), this, SLOT(applyTween()));

    k->closeButton = new KImageButton(QPixmap(THEME_DIR + "icons/close.png"), 22);
    connect(k->closeButton, SIGNAL(clicked()), this, SIGNAL(clickedResetTween()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);
    buttonsLayout->setMargin(0);
    buttonsLayout->setSpacing(10);
    buttonsLayout->addWidget(k->applyButton);
    buttonsLayout->addWidget(k->closeButton);

    innerLayout->addLayout(buttonsLayout);

    k->layout->addWidget(k->buttonsPanel);
    activeButtonsPanel(true);
}

void TweenerPanel::activeButtonsPanel(bool enable)
{
    if (enable && !k->buttonsPanel->isVisible())
        k->buttonsPanel->show();
    else
        k->buttonsPanel->hide();
}

void TweenerPanel::loadTweenComponents()
{
    QStringList labels;
    labels << tr("Position") << tr("Rotation") << tr("Scale") << tr("Shear") << tr("Opacity") << tr("Coloring");
    k->panelList = new QList<QWidget*>();

    for (int i = 0; i < labels.size(); ++i) {

         switch(i)  {
                case TweenerPanel::Position:
                     kFatal() << "TweenerPanel::loadTweenComponents() - Opening Position gui";
                     k->positionPanel = new PositionSettings;
                     connect(k->positionPanel, SIGNAL(clickedApplyTween(TweenerPanel::TweenerType, const QString &)), this, SLOT(activateTweenersTable(TweenerPanel::TweenerType, const QString &)));  
                     connect(k->positionPanel, SIGNAL(clickedCloseTweenProperties(TweenerPanel::Mode)), this, SLOT(updateTweenersTable(TweenerPanel::Mode)));
                     connect(k->positionPanel, SIGNAL(startingPointChanged(int)), this, SIGNAL(startingPointChanged(int))); 

                     k->positionPanel->setParameters(k->totalSteps, 0);
                     k->panelList->append(k->positionPanel);
                break;
                case TweenerPanel::Rotation:
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Scale:
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Shear:
                     k->panelList->append(new QWidget);
                break;
                case TweenerPanel::Opacity:
                     k->panelList->append(new QWidget); 
                break;
                case TweenerPanel::Coloring:
                     k->panelList->append(new QWidget);
                break;
         }

         k->layout->addWidget(k->panelList->at(i));
         activeTweenComponent(i, false);
    }
}

void TweenerPanel::activeTweenComponent(int index, bool enable)
{
    if (enable && !k->panelList->at(index)->isVisible()) {
        k->panelList->at(index)->show();
    } else {
        k->panelList->at(index)->hide();
    }
}

// Adding new Tween

void TweenerPanel::setParameters(const QString &name, int framesTotal, int startFrame)
{
    k->tweenerTable->resetTable();

    k->mode = Add;
    k->input->setText(name);

    activateMode(TweenerPanel::Selection);

    k->applyButton->setToolTip(tr("Save Tween"));
    k->closeButton->setIcon(QPixmap(THEME_DIR + "icons/close.png"));
    k->closeButton->setToolTip(tr("Cancel Tween"));
}

// Editing Tween

void TweenerPanel::setParameters(KTItemTweener *currentTween)
{
    k->tweenerTable->resetTable();

    k->mode = Edit;
    k->input->setText(currentTween->name());

    activateMode(TweenerPanel::TweenList);

    for (int i=0; i < 6; i++) {
         if (currentTween->contains(KTItemTweener::Type(i))) {
             k->tweenerTable->checkTween(i, true);
             switch(i) {
                    case 0:
                         k->positionPanel->setParameters(currentTween);
                    break;
             }
         }
    }
}

void TweenerPanel::emitOptionChanged(int option)
{
    switch (option) {
            case 0:
             {
                 emit clickedSelect();
                 activeTweenerTableForm(false);
             }
            break;
            case 1:
             {
                 if (k->selectionDone) {
                     activeTweenerTableForm(true);
                     emit clickedTweenProperties();
                 } else {
                     k->options->setCurrentIndex(0);
                     KOsd::self()->display(tr("Info"), tr("Select objects for Tweening first!"), KOsd::Info);
                 }
             }
    }
}

void TweenerPanel::activateMode(TweenerPanel::EditMode mode)
{
    k->options->setCurrentIndex(mode);
}

void TweenerPanel::notifySelection(bool flag)
{
    k->selectionDone = flag;
}

void TweenerPanel::showTweenSettings(int tweenType)
{
    k->currentTweenIndex = tweenType;

    activeOptionsPanel(false);
    activeTweenerTableForm(false);
    activeButtonsPanel(false);
    activeTweenComponent(tweenType, true);

    kFatal() << "TweenerPanel::showTweenSettings() - Opening tween: " << tweenType;
    emit tweenPropertiesActivated(TweenerPanel::TweenerType(tweenType));
}

void TweenerPanel::activateTweenersTable(TweenerPanel::TweenerType type, const QString &message)
{
    k->tweenerList.append(type);

    if (!k->applyButton->isEnabled())
        k->applyButton->setEnabled(true);

    KOsd::self()->display(tr("Info"), message, KOsd::Info);
}

void TweenerPanel::updateTweenersTable(TweenerPanel::Mode mode)
{
    kFatal() << "TweenerPanel::updateTweenersTable() - Just tracing!"; 

    activeTweenComponent(k->currentTweenIndex, false);
    activeOptionsPanel(true);
    activeTweenerTableForm(true);
    activeButtonsPanel(true);

    if (mode == TweenerPanel::Add)
        k->tweenerTable->checkTween(k->currentTweenIndex, false);

    if (TweenerPanel::TweenerType(k->currentTweenIndex) == TweenerPanel::Position)
        emit resetPathFromWorkSpace();
}

void TweenerPanel::updateSteps(const QGraphicsPathItem *path, QPointF offset)
{
    k->positionPanel->updateSteps(path, offset);
}

void TweenerPanel::initStartCombo(int framesTotal, int currentFrame)
{
    k->positionPanel->initStartCombo(framesTotal, currentFrame);
}

int TweenerPanel::startComboSize()
{
    return k->positionPanel->startComboSize();
}

void TweenerPanel::applyTween()
{
    kFatal() << "TweenerPanel::applyTween() - Just tracing!";
    setEditMode();

    emit clickedApplyTween();
}

void TweenerPanel::setEditMode()
{
    k->mode = TweenerPanel::Edit;
    k->closeButton->setIcon(QPixmap(THEME_DIR + "icons/close_properties.png"));
    k->closeButton->setToolTip(tr("Close Tween properties"));
}

QString TweenerPanel::tweenToXml(int currentFrame, QPointF point)
{
   QString name = k->input->text();

   if (name.length() <= 0) {
       // print error message here
       return 0;
   }

   QDomDocument doc;

   QDomElement tweening = doc.createElement("tweening");
   tweening.setAttribute("name", name);
   tweening.setAttribute("type", TweenerPanel::Compound);
   tweening.setAttribute("init", currentFrame);
   tweening.setAttribute("frames", k->positionPanel->totalSteps());
   tweening.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));

   QDomElement settings = doc.createElement("settings");

   for (int i=0; i < k->tweenerList.size(); i++) {

        kFatal() << "TweenerPanel::tweenToXml() - tweenerList: " << k->tweenerList.at(i);

        if (k->tweenerList.at(i) == TweenerPanel::Position) {
            QDomElement position = doc.createElement("position");
            position.setAttribute("init", currentFrame);
            position.setAttribute("frames", k->positionPanel->totalSteps());
            position.setAttribute("coords", k->positionPanel->pathString()); 
            settings.appendChild(position);
        }
   }

   tweening.appendChild(settings);

   foreach (KTTweenerStep *step, k->positionPanel->steps())
            tweening.appendChild(step->toXml(doc));

   doc.appendChild(tweening);

   kFatal() << "LOOK THIS";
   kFatal() << doc.toString();

   return doc.toString();

   /*
   QString text;

   text = "<tweening name=\"test\" type=\"7\" init=\"0\" frames=\"3\" origin=\"22,26\">";
   text += "<settings>";
   text += " <position init=\"0\" frames=\"3\" coords=\"M 53.5 53.5  C 245 140  229  333  229  333   392 2 4  387  331  387  331   522 47  587  137  587  137\" />";
   text += "  <rotation init=\"0\" frames=\"3\" rotateDirection=\"0\" rotateSpeed=\"5\" rotationType=\"0\" />";
   text += "</settings>";
   text += "<step value=\"0\">";
   text += "  <position x=\"0\" y=\"0\"/>";
   text += "  <rotation angle=\"0\"/>";
   text += "</step>";
   text += " <step value=\"1\">";
   text += "  <position x=\"20\" y=\"20\"/>";
   text += "  <rotation angle=\"45\"/>";
   text += " </step>";
   text += " <step value=\"2\">";
   text += "  <position x=\"40\" y=\"40\"/>";
   text += "  <rotation angle=\"90\"/>";
   text += " </step>";
   text += "</tweening>";

   return text;
   */
}

int TweenerPanel::totalSteps()
{
    return k->positionPanel->totalSteps();
}

QString TweenerPanel::currentTweenName() const
{
    QString tweenName = k->input->text();
    if (tweenName.length() > 0)
        k->input->setFocus();

    return tweenName;
}
