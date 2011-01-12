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

#include "kfontchooser.h"
#include "kformfactory.h"
#include "kdebug.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QFontDatabase>
#include <QFontComboBox>

KFontChooser::KFontChooser(QWidget *parent) : QFrame(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
     
    m_families = new QFontComboBox;
    connect(m_families, SIGNAL(currentFontChanged(const QFont &)), this, SLOT(loadFontInfo(const QFont &)));
    
    mainLayout->addLayout(KFormFactory::makeLine(tr("Family"), m_families));

    m_fontStyle = new QComboBox;
    connect(m_fontStyle, SIGNAL(activated (int)), this, SLOT(emitFontChanged(int)));
    mainLayout->addLayout(KFormFactory::makeLine(tr("Style"), m_fontStyle));

    m_fontSize = new QComboBox;
    connect(m_fontSize, SIGNAL(activated (int)), this, SLOT(emitFontChanged(int)));
    mainLayout->addLayout(KFormFactory::makeLine(tr("Size"), m_fontSize));

    initFont();
}

KFontChooser::~KFontChooser()
{
}

void KFontChooser::loadFontInfo(const QFont &newFont)
{
    QString currentSize = m_fontSize->currentText();
    QString currentStyle = m_fontStyle->currentText();
    
    QString family = newFont.family();
    
    QFontDatabase fdb;
    
    m_fontStyle->clear();
    
    m_fontStyle->addItem(tr("Normal"), QFont::StyleNormal);
    m_fontStyle->addItem(tr("Italic"), QFont::StyleItalic);
    m_fontStyle->addItem(tr("Oblique"), QFont::StyleOblique);
    
    // m_fontStyle->addItems();
    
    m_fontSize->clear();
    
    QList<int> points = fdb.pointSizes(family);
    
    foreach (int point, points)
             m_fontSize->addItem(QString::number(point));
    
    int sizeIndex = m_fontSize->findText(currentSize);
    int styleIndex = m_fontStyle->findText(currentStyle);

    if (sizeIndex >= 0)
        m_fontSize->setCurrentIndex(sizeIndex);
    
    if (styleIndex >= 0)
        m_fontStyle->setCurrentIndex(styleIndex);
    
    m_families->blockSignals(true);

    m_currentFont = newFont;
    m_currentFont.setPointSize(m_fontSize->currentText().toInt());
    m_currentFont.setStyle(QFont::Style(m_fontStyle->itemData(m_fontStyle->currentIndex()).toInt()));

    m_families->blockSignals(false);
    
    emit fontChanged();
    
}

void KFontChooser::emitFontChanged(int)
{
    m_currentFont = m_families->currentFont();
    m_currentFont.setPointSize(m_fontSize->currentText().toInt());
    m_currentFont.setStyle(QFont::Style(m_fontStyle->itemData(m_fontStyle->currentIndex()).toInt()));

    emit fontChanged();
}

void KFontChooser::setCurrentFont(const QFont &font)
{
    QFontDatabase fdb;

    m_families->setCurrentIndex(m_families->findText(font.family()));
    m_fontStyle->setCurrentIndex(m_fontStyle->findText(fdb.styleString(font.family())));
    m_fontSize->setCurrentIndex(m_fontSize->findText(QString::number(font.pointSize())));
}

void KFontChooser::initFont()
{
    m_families->setCurrentIndex(0);
    m_fontStyle->setCurrentIndex(0);
    m_fontSize->setCurrentIndex(0);
}

QFont KFontChooser::currentFont() const
{
     return m_currentFont;
    //return m_families->currentFont();
}

QFont::Style KFontChooser::currentStyle() const
{
    return QFont::Style(m_fontStyle->itemData(m_fontStyle->currentIndex()).toInt());
}

int KFontChooser::currentSize() const
{
    return m_fontSize->currentText().toInt();
}



