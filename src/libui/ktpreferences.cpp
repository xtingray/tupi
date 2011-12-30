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

#include "ktpreferences.h"
#include "ktpaintareaconfig.h"
#include "tglobal.h"
#include "tdebug.h"
#include "tapplication.h"
#include "kformfactory.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

/**
 * This class handles the preferences dialog for Tupi.
 * Here is where the forms of the Tupi preferences dialog are defined.
 * @author David Cuadrado
*/

class KTPreferences::GeneralPage : public QWidget
{
    Q_OBJECT

    public:
        GeneralPage();
        ~GeneralPage();
        void saveValues();
        bool autoSaveUpdated(); 
        // Tupi::RenderType m_renderType;
        
    private:
        QLineEdit *m_home;
        QLineEdit *m_repository; 
        QLineEdit *m_browser;
        QSpinBox *m_minutes;
        int autoSave;
        
        //QComboBox *m_renderType;
        QCheckBox *m_openLastProject;
        
    // private slots:
    // void syncRenderType(int index);
};

KTPreferences::GeneralPage::GeneralPage()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    // m_renderType = Tupi::RenderType(0);
    TCONFIG->beginGroup("General");
    
    m_home = new QLineEdit;
    
    QString str = TCONFIG->value("Home").toString();
    if (!str.isEmpty())
        m_home->setText(str);
    
    m_repository = new QLineEdit;
    str = TCONFIG->value("Cache").toString();
    if (!str.isEmpty())
        m_repository->setText(str);
    
    m_browser = new QLineEdit;
    str = TCONFIG->value("Browser").toString();
    if (!str.isEmpty())
        m_browser->setText(str);
    else
        m_browser->setText("/usr/bin/firefox");
    
    str = TCONFIG->value("RenderType").toString();

    /*
    m_renderType = new QComboBox();
    
    m_renderType->addItem(tr("Image"), Tupi::Image);
    m_renderType->addItem(tr("OpenGL"), Tupi::OpenGL);
    m_renderType->addItem(tr("Native"), Tupi::Native);
    if (!str.isEmpty())
        m_renderType->setCurrentIndex(str.toInt());
    */
    
    bool openLast = TCONFIG->value("OpenLastProject", true).toBool();
    m_openLastProject = new QCheckBox();
    m_openLastProject->setChecked(openLast);

    autoSave = TCONFIG->value("AutoSave").toInt();

    m_minutes = new QSpinBox(this);
    m_minutes->setMinimum(0);
    m_minutes->setMaximum(15);
    m_minutes->setValue(autoSave); 

    QLayout *form = KFormFactory::makeGrid(QStringList() << tr("Tupi Home") << tr("Cache") << tr("Browser") 
                    << tr("Open last project") << tr("Auto save (minutes)"), QWidgetList() << m_home 
                    << m_repository << m_browser << m_openLastProject << m_minutes);
    
    layout->addLayout(form);
    layout->addStretch(3);
}

KTPreferences::GeneralPage::~GeneralPage()
{
}

void KTPreferences::GeneralPage::saveValues()
{
    TCONFIG->beginGroup("General");
    
    QString str = m_home->text();
    if (!str.isEmpty() && m_home->isModified())
        TCONFIG->setValue("Home", str);
    
    str = m_repository->text();
    if (!str.isEmpty() && m_repository->isModified())
        TCONFIG->setValue("Cache", str);
    
    str = m_browser->text();
    if (!str.isEmpty() && m_browser->isModified())
        TCONFIG->setValue("Browser", str);

    if (autoSaveUpdated())
        TCONFIG->setValue("AutoSave", m_minutes->value()); 

    //TCONFIG->setValue("RenderType", QString::number((m_renderType->itemData(m_renderType->currentIndex ()).toInt())));
    TCONFIG->setValue("OpenLastProject", m_openLastProject->isChecked());

    TCONFIG->sync();
}

bool KTPreferences::GeneralPage::autoSaveUpdated()
{
    return autoSave != m_minutes->value();
}

class KTPreferences::FontPage : public QWidget
{
    Q_OBJECT;
    public:
        FontPage();
        ~FontPage();
        
        QFont currentFont() const;
        
    private:
        KFontChooser *m_fontChooser;
};

KTPreferences::FontPage::FontPage()
{
    m_fontChooser = new KFontChooser(this);
    //m_fontChooser->setCurrentFont(font());
    m_fontChooser->initFont();
}

KTPreferences::FontPage::~FontPage()
{
}

QFont KTPreferences::FontPage::currentFont() const
{
    return m_fontChooser->currentFont();
}

//--------------- CONSTRUCTOR --------------------

KTPreferences::KTPreferences(QWidget *parent) : TConfigurationDialog(parent)
{
    setWindowTitle(tr("Application KTPreferences"));
    
    m_generalPage = new GeneralPage;
    addPage(m_generalPage, tr("General"), QPixmap(THEME_DIR + "icons/tupi_general_preferences.png"));
    
    m_themeSelector = new KTThemeSelector;
    addPage(m_themeSelector, tr("Theme"), QPixmap(THEME_DIR + "icons/tupi_theme_properties.png"));
    
    m_fontChooser = new FontPage;
    addPage(m_fontChooser, tr("Font"), QPixmap(THEME_DIR + "icons/tupi_font_properties.png"));
    
    m_drawingAreaProperties = new KTPaintAreaConfig;
    addPage(m_drawingAreaProperties, tr("Workspace"), QIcon(THEME_DIR + "icons/tupi_workspace_properties.png"));

    setCurrentItem(0);
    
    // resize(400,400);
}

//-------------- DESTRUCTOR -----------------

KTPreferences::~KTPreferences()
{
}

void KTPreferences::ok()
{
    apply();
    TConfigurationDialog::ok();
}

void KTPreferences::apply()
{
    if (static_cast<KTThemeSelector *>(currentPage()) ==  m_themeSelector) {
        if (m_themeSelector->applyColors())
            kApp->applyTheme(m_themeSelector->document());
    } else if (static_cast<GeneralPage *>(currentPage()) == m_generalPage) {
             m_generalPage->saveValues();
             if (m_generalPage->autoSaveUpdated())
                 emit timerChanged();
    } else if (qobject_cast<FontPage *>(currentPage()) == m_fontChooser) {
             kApp->setFont(m_fontChooser->currentFont());
    } else if (qobject_cast<KTPaintAreaConfig *>(currentPage()) == m_drawingAreaProperties) {
    }
}

#include "ktpreferences.moc"

