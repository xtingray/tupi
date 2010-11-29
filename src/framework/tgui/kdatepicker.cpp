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

#include "kdatepicker.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QIntValidator>
#include <QLineEdit>

KDatePicker::EditableButton::EditableButton()
{
    m_editor = new QLineEdit(this);
    m_editor->setValidator(new QIntValidator(1753, 4000, m_editor));
    m_editor->hide();
    
    connect(this, SIGNAL(clicked()), this, SLOT(edit()));
    connect(m_editor, SIGNAL(editingFinished ()), this, SLOT(emitYearSelected()));
}

KDatePicker::EditableButton::~EditableButton()
{
}

void KDatePicker::EditableButton::emitYearSelected()
{
    int year = m_editor->text().toInt();
    m_editor->hide();
    
    emit yearSelected(year);
}

void KDatePicker::EditableButton::edit()
{
    m_editor->setMaximumSize(size());
    m_editor->setText(text());
    
    m_editor->show();
    m_editor->setFocus();
}

KDatePicker::KDatePicker(QWidget *parent) : QFrame(parent)
{
    Q_INIT_RESOURCE(tgui_images);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(1);
    
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addStretch();
    
    // ":general/images/ 1day.png  1leftarrow.png  1rightarrow.png  2leftarrow.png  2rightarrow.png
    // Setup buttons
    
    QToolButton *prevYear = new QToolButton();
    prevYear->setAutoRaise(true);
    prevYear->setToolButtonStyle(Qt::ToolButtonIconOnly);
    prevYear->setIconSize( QSize(32,32));
    prevYear->setIcon(QIcon(":general/images/2leftarrow.png"));
    
    topLayout->addWidget(prevYear);
    
    QToolButton *prevMounth = new QToolButton();
    prevMounth->setAutoRaise(true);
    prevMounth->setToolButtonStyle(Qt::ToolButtonIconOnly);
    prevMounth->setIconSize(QSize(32,32));
    prevMounth->setIcon(QIcon(":general/images/1leftarrow.png"));
    
    topLayout->addWidget(prevMounth);
    
    //-------------- ****************** -------------------
    topLayout->addStretch(2);
    
    m_mounth = new QToolButton();
    m_mounth->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_mounth->setAutoRaise(true);
    
    topLayout->addWidget(m_mounth);
    
    m_year = new EditableButton();
    m_year->setToolButtonStyle (Qt::ToolButtonTextOnly);
    m_year->setAutoRaise(true);
    topLayout->addWidget(m_year);
    
    topLayout->addStretch(2);
    //-------------- ****************** -------------------
    
    QToolButton *nextMounth = new QToolButton();
    nextMounth->setAutoRaise(true);
    nextMounth->setToolButtonStyle(Qt::ToolButtonIconOnly);
    nextMounth->setIconSize(QSize(32,32));
    nextMounth->setIcon(QPixmap(":general/images/1rightarrow.png"));
    
    topLayout->addWidget(nextMounth);
    
    QToolButton *nextYear = new QToolButton();
    
    nextYear->setAutoRaise(true);
    nextYear->setToolButtonStyle(Qt::ToolButtonIconOnly);
    nextYear->setIconSize(QSize(32,32));
    nextYear->setIcon(QPixmap(":general/images/2rightarrow.png"));
    
    topLayout->addWidget(nextYear);
    
    topLayout->addStretch();
    
    mainLayout->addLayout(topLayout);
    
    // Main widget
    
    m_dateTable = new KDateTable;
    
    mainLayout->addWidget(m_dateTable);
    
    // Weeks
    m_week = new QComboBox;
    
    fillWeeks(m_dateTable->date());
    
    mainLayout->addWidget(m_week);
    
    setDate(QDate::currentDate());
    
    QMenu *mounths = new QMenu;
    QDate tmp(QDate::currentDate().year(),1,1);

    for (int i = 1; i <= 12; i++) {
         mounths->addAction( tmp.toString("MMMM") )->setData(i);
         tmp = tmp.addMonths(1);
    }

    m_mounth->setMenu(mounths);
    connect(mounths, SIGNAL(triggered(QAction *)), this, SLOT(mounthFromAction(QAction *)));
    
    // Connections
    
    connect(m_week, SIGNAL(activated(int)), this, SLOT(setWeek(int)));
    connect(prevYear, SIGNAL(clicked()), this, SLOT(previousYear()));
    connect(nextYear, SIGNAL(clicked()), this, SLOT(nextYear()));
    
    connect(m_mounth, SIGNAL(clicked()), m_mounth, SLOT(showMenu()));
    connect(m_year, SIGNAL(yearSelected(int)), this, SLOT(setYear(int)));
    
    connect(prevMounth, SIGNAL(clicked()), this, SLOT(previousMounth()));
    connect(nextMounth, SIGNAL(clicked()), this, SLOT(nextMounth()));
    
    connect(m_dateTable, SIGNAL(dateChanged( const QDate &)), this, SIGNAL(dateChanged( const QDate& )));
}

void KDatePicker::fillWeeks(const QDate &date)
{
    int year = date.year();
    
    QDate day(year, 1, 1);
    
    QDate firstDayOfLastMonth(year, 12, 1);
    QDate lastDay(year, 12, firstDayOfLastMonth.daysInMonth() );

    for (; day <= lastDay ; day = day.addDays(7)) {
         QString week = tr("Week %1").arg(day.weekNumber(&year));
         if (year != day.year()) 
             week += "*";
 
         m_week->addItem(week);
    }
    
    m_week->setCurrentIndex(date.weekNumber(&year));
}

KDatePicker::~KDatePicker()
{
}

void KDatePicker::setWeek(int week)
{
    QDate temp(m_dateTable->date().year(), 1, 1);
    
    temp = temp.addDays(week * 7 - temp.dayOfWeek() + 1);

    setDate(temp);
}

void KDatePicker::setYear(int year)
{
    QDate date(year, m_dateTable->date().month(), m_dateTable->date().day());
    setDate(date);
}

void KDatePicker::setDate(const QDate &date)
{
    if (date.isValid()) {
        m_dateTable->setDate(date);
        
        m_mounth->setText(date.toString("MMMM"));
        m_year->setText(QString::number(date.year()));
        
        setWindowTitle(date.toString());
    }
}

void KDatePicker::previousYear()
{
    QDate date = m_dateTable->date().addYears(-1);
    
    setDate(date);
}

void KDatePicker::nextYear()
{
    QDate date = m_dateTable->date().addYears(1);
    
    setDate(date);
}

void KDatePicker::previousMounth()
{
    QDate date = m_dateTable->date().addMonths(-1);
    
    setDate(date);
}

void KDatePicker::nextMounth()
{
    QDate date = m_dateTable->date().addMonths(1);
    
    setDate(date);
}

void KDatePicker::mounthFromAction(QAction *act)
{
    if (act) {
        int mounth = act->data().toInt();
        
        QDate date(m_dateTable->date().year(), mounth, m_dateTable->date().day());
        
        setDate(date);
    }
}

QDate KDatePicker::date() const
{
    return m_dateTable->date();
}
