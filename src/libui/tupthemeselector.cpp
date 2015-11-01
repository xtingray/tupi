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

#include "tupthemeselector.h"
#include "tseparator.h"
#include "tcolorbutton.h"
#include "tapplication.h"
#include "tstylecombobox.h"

// Qt
#include <QFileDialog>
#include <QTextStream>
#include <QGridLayout>
#include <QList>
#include <QDateTime>
#include <QHeaderView>
#include <QCheckBox>
#include <QInputDialog>

/**
 * This class defines the themes manager for Tupi.
 * Here is where methods related to themes management are defined.
 * @author David Cuadrado
*/

TupThemeSelector::TupThemeSelector(QWidget *parent) : TVHBox(parent, Qt::Vertical)
{
    setupChooseColor();
    loadSchemes();
}

TupThemeSelector::~TupThemeSelector()
{
}

void TupThemeSelector::setupChooseColor()
{
    TVHBox *hbox = new TVHBox(this, Qt::Horizontal);
    TVHBox *box1 = new TVHBox(hbox, Qt::Vertical);
    box1->boxLayout()->setMargin(10);
    m_general = new QGroupBox(tr("General"), box1);
    
    QGridLayout *layout1 = new QGridLayout(m_general);
    
    QStringList labels1 = QStringList() << tr("Text") << tr("Base") << tr("Foreground") << tr("Background") 
                << tr("Button") << tr("Button Text");
    
    QStringList names = QStringList() << "Text" << "Base" << "Foreground" << "Background" << "Button" << "ButtonText";
    
    QPalette colorGroup = QApplication::palette();
    
    QList<QColor> colors = QList<QColor>() << colorGroup.text ().color() << colorGroup.base().color() 
                << colorGroup.foreground().color() << colorGroup.background().color() 
                << colorGroup.button().color() << colorGroup.buttonText().color();
    
    for (int i = 0; i < labels1.count(); i++) {
         layout1->addWidget(new QLabel(labels1[i], m_general), i, 0 );
         TColorButton *button = new TColorButton(m_general);
         button->setObjectName(names[i]);
         QPalette pal = button->palette();
         pal.setColor(QPalette::Button, colors[i]);
         button->setPalette(pal);
        
         m_generalButtonGroup.addButton(button);
         layout1->addWidget(button, i, 1);
         m_generalSection.insert(names[i], colors[i].name());
    }
    
    m_effects = new QGroupBox(tr("Effects"), box1);
    
    QGridLayout *layout2 = new QGridLayout(m_effects);
    
    QStringList labels2 = QStringList() << tr("Light") << tr("Midlight") << tr("Dark") << tr("Mid");
    QStringList names2 = QStringList() << "Light" << "Midlight" << "Dark" << "Mid";
    
    colors.clear();
    colors << colorGroup.light().color() << colorGroup.midlight().color() << colorGroup.dark().color() 
        << colorGroup.mid().color();
    
    for (int i = 0; i < labels2.count(); i++) {
         layout2->addWidget(new QLabel(labels2[i], m_effects), i, 0);
         TColorButton *button = new TColorButton(m_effects);
         button->setObjectName(names2[i]);
         QPalette pal = button->palette();
         pal.setColor(QPalette::Button, colors[i]);
         button->setPalette(pal);
         m_effectsButtonGroup.addButton(button);
         layout2->addWidget(button, i, 1);
         m_effectsSection.insert(names2[i], colors[i].name());
    }
    ////////////
    
    TVHBox *box2 = new TVHBox(hbox, Qt::Vertical);
    box2->boxLayout()->setMargin(10);
    m_selections = new QGroupBox(tr("Selections"), box2);
    
    QGridLayout *layout3 = new QGridLayout(m_selections);
    
    QStringList labels3 = QStringList() << tr("Highlight") << tr("Highlighted Text");
    QStringList names3 = QStringList() << "Highlight" << "HighlightedText";
    colors.clear();
    colors << colorGroup.highlight().color() << colorGroup.highlightedText().color();
    
    for (int i = 0; i < labels3.count(); i++) {
         layout3->addWidget(new QLabel(labels3[i], m_selections), i, 0);
         TColorButton *button = new TColorButton(m_selections);
         button->setObjectName(names3[i]);
         QPalette pal = button->palette();
         pal.setColor(QPalette::Button, colors[i]);
         button->setPalette(pal);
         m_selectionsButtonGroup.addButton(button);
         layout3->addWidget(button, i, 1);
         m_selectionsSection.insert(names3[i], colors[i].name());
    }
    
    m_textEffects = new QGroupBox(tr("Text effects"), box2);
    
    QGridLayout *layout4 = new QGridLayout(m_textEffects);
    QStringList labels4 = QStringList() << tr("Bright Text") << tr("Link") << tr("Link Visited");
    QStringList names4 = QStringList() << "BrightText" << "Link" << "LinkVisited";
    
    colors.clear();
    colors << colorGroup.brightText().color() << colorGroup.link().color() << colorGroup.linkVisited().color();
    
    for (int i = 0; i < labels4.count(); i++) {
         layout4->addWidget(new QLabel(labels4[i], m_textEffects), i, 0 );
         TColorButton *button = new TColorButton(m_textEffects);
         button->setObjectName(names4[i]);
         QPalette pal = button->palette();
         pal.setColor(QPalette::Button, colors[i]);
         button->setPalette(pal);
         m_textEffectsButtonGroup.addButton(button);
         layout4->addWidget(button, i, 1);
         m_textEffectsSection.insert(names4[i], colors[i].name());
    }
    
    QGroupBox *schemeWidget = new QGroupBox(tr("Schema"), box2); // FIXME: add vertical layout
    QVBoxLayout *schemaLayout = new QVBoxLayout;
    
    m_allSchemes = new QTreeWidget;
    m_allSchemes->setHeaderLabels(QStringList() << tr("Schema") << tr("Owner") << tr("Date"));
    // m_allSchemes->header()->setResizeMode(QHeaderView::Stretch);
    m_allSchemes->header()->setSectionResizeMode(QHeaderView::Stretch);
    
    schemaLayout->addWidget(m_allSchemes);
    
    connect(m_allSchemes, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, 
                    SLOT(loadSchemaFromListView(QTreeWidgetItem *, int)));
    
    QPushButton *saveSchemeButton = new QPushButton(tr("Save schema"));
    connect(saveSchemeButton, SIGNAL(clicked()), SLOT(saveSchema()));
    
    schemaLayout->addWidget(saveSchemeButton);
    schemeWidget->setLayout(schemaLayout);
    
    new TSeparator(this);
    new QLabel(tr("Style"), this);
    new TStyleComboBox(this);
    
    new TSeparator(this);
    m_useColors = new QCheckBox(tr("Use this colors"), this);
    
    connect(&m_generalButtonGroup, SIGNAL(buttonClicked(QAbstractButton * )), 
        SLOT(chooseGeneralColor(QAbstractButton * )));
    connect(&m_effectsButtonGroup, SIGNAL(buttonClicked(QAbstractButton * )), 
        SLOT(chooseEffectsColor(QAbstractButton * )));
    connect(&m_selectionsButtonGroup, SIGNAL(buttonClicked(QAbstractButton * )), 
        SLOT(chooseSelectionsColor(QAbstractButton * )));
    connect(&m_textEffectsButtonGroup, SIGNAL(buttonClicked(QAbstractButton * )), 
        SLOT(chooseTextEffectsColor(QAbstractButton * )));
}

void TupThemeSelector::chooseGeneralColor(QAbstractButton *  button)
{
    QColor c = qobject_cast<TColorButton *>(button)->color();
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, c);
    button->setPalette(pal);
    m_generalSection.insert(button->objectName(), c.name());
}

void TupThemeSelector::chooseEffectsColor(QAbstractButton *  button)
{
    QColor c = qobject_cast<TColorButton *>(button)->color();
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, c);
    button->setPalette(pal);
    m_effectsSection.insert(button->objectName(), c.name());
}

void TupThemeSelector::chooseSelectionsColor(QAbstractButton *  button)
{
    QColor c = qobject_cast<TColorButton *>(button)->color();
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, c);
    button->setPalette(pal);
    m_selectionsSection.insert(button->objectName(), c.name());
}

void TupThemeSelector::chooseTextEffectsColor(QAbstractButton *  button)
{
    QColor c = qobject_cast<TColorButton *>(button)->color();
    QPalette pal = button->palette();
    pal.setColor(QPalette::Button, c);
    button->setPalette(pal);
    m_textEffectsSection.insert(button->objectName(), c.name());
}

ThemeDocument TupThemeSelector::document() const
{
    ThemeDocument doc;
    
    doc.addGeneralSection(m_generalSection);
    doc.addEffectsSection(m_effectsSection);
    doc.addSelections(m_selectionsSection);
    doc.addTextEffect(m_textEffectsSection);
    
    return doc;
}

void TupThemeSelector::loadSchemes()
{
    QDir themeDir(SHARE_DIR + "themes");
    
    if (themeDir.exists()) {
        m_allSchemes->clear();
        QFileInfoList files = themeDir.entryInfoList(QStringList() <<"*.tupt");
        
        for (int i = 0; i < files.count(); i++) {
             QFileInfo iterator = files[i];
             QTreeWidgetItem *item = new QTreeWidgetItem(m_allSchemes);
             item->setText(0, iterator.fileName());
             item->setText(1, iterator.owner());
             item->setText(2, iterator.created().toString());
        }
    }
}

bool TupThemeSelector::applyColors() const
{
    return m_useColors->isChecked();
}

void TupThemeSelector::saveSchema()
{
    QDir themeDir(SHARE_DIR + "themes");
    if (! themeDir.exists())
        themeDir.mkdir(themeDir.path());
    
    QString fileName = QInputDialog::getText (this,tr("Name"), tr("Please choose a theme name"));
    
    if (!fileName.endsWith(".tupt"))
        fileName += ".tupt";
    
    QFile file(themeDir.path() + "/" + fileName);
    
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << document().toString() << endl;
        file.close();
        m_lastFile = themeDir.path() + "/" + fileName;
    }
    loadSchemes();
}

QString TupThemeSelector::lastFile() const
{
    return m_lastFile;
}

void TupThemeSelector::loadSchemaFromListView(QTreeWidgetItem *item, int)
{
    if (item) {
        kApp->applyTheme(SHARE_DIR + "themes/" + item->text(0));
        
        if (! item->text(0).isEmpty()) {
            #ifdef K_DEBUG
                #ifdef Q_OS_WIN32
                    qDebug() << "[TupThemeSelector::loadSchemaFromListView()]";
                #else
                    T_FUNCINFO;
                #endif
            #endif 			
            TCONFIG->beginGroup("General");
            TCONFIG->setValue("ThemeFile", SHARE_DIR + "themes/" + item->text(0));
            m_lastFile = SHARE_DIR + "themes/" + item->text(0);
        }
    }
}
