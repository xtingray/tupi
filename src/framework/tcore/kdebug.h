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

#ifndef KDEBUG_H
#define KDEBUG_H

#include <QTextStream>
#include <QStringList>
#include <QTextEdit>

#ifdef QT_GUI_LIB
#include <QColor>
#endif

#include "kglobal.h"

/**
 * @author David Cuadrado <krawek@gmail.com>
*/

#ifdef __GNUC__
#define K_FUNCINFO kDebug() << "[" << __PRETTY_FUNCTION__ << "] "
#define K_FUNCINFOX(area) kDebug(area) << "[" << __PRETTY_FUNCTION__ << "] "
#define KINIT kDebug("class") << "[Initializing " << __FUNCTION__ << "]"
#define KEND kDebug("class") << "[Destroying " << __FUNCTION__ << "]"
#define FUNC_NOT_IMPLEMENTED kWarning() << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << " not implemented yet";

#else

#define K_FUNCINFO
#define K_FUNCINFOX(area)
#define KINIT kDebug("class") << "[Initializing " << __FILE__ << ":" << __LINE__ << "] "
#define KEND kDebug("class") << "[Destroying " << __FILE__ << ":" << __LINE__ << "] "
#define FUNC_NOT_IMPLEMENTED kWarning() << __FILE__<<":"<<__LINE__ << " not implemented yet";
#endif

#define K_CHECKPTR(ptr) if ( ptr == 0 ) { kFatal() << __PRETTY_FUNCTION__ << ": " << #ptr << " is NULL"; } 
#define SHOW_VAR(arg) kDebug() << #arg << " = " << arg;

class QPalette;

class QWidget;
class QDateTime;
class QDate;
class QTime;
class QPoint;
class QPointF;
class QSize;
class QRect;
class QRegion;
class QStringList;
class QColor;
class QPen;
class QBrush;
class QVariant;
class QPixmap;
class QIcon;
class QImage;
class QMatrix;

class QLinearGradient;
class QRadialGradient;
class QConicalGradient;
class QGradient;
class QTextBrowser;

template <class T> class QList;

enum DebugType
{
    KDebugMsg = 0,
    KWarningMsg,
    KErrorMsg,
    KFatalMsg
};

enum DebugOutput
{
    KDefault = -1,
    KNone = 0,
    KFileOutput,
    KBoxOutput,
    KShellOutput,
    KBrowserOutput
};

#if !defined(K_NODEBUG)
class K_CORE_EXPORT KDebug
{
    public:
        class Streamer : public QObject
            {
                public:
                    Streamer() : space(true) {}
                    ~Streamer() {};
                    QString buffer;
                    bool space;

                    Streamer & operator << (QChar c)
                        {
                            buffer += c;
                            return *this;
                        }

                    Streamer & operator << (signed short i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (float f)
                        {
                            buffer += QString::number(f);
                            return *this;
                        }

                    Streamer & operator << (const QString & string)
                        {
                            buffer += string;
                            return *this;
                        }

                    Streamer & operator << (char c)
                        {
                            buffer += c;
                            return *this;
                        }

                    Streamer & operator << (unsigned short i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (signed int i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (unsigned int i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (signed long i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (unsigned long i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (qlonglong i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (qulonglong i)
                        {
                            buffer += QString::number(i);
                            return *this;
                        }

                    Streamer & operator << (double f)
                        {
                            buffer += QString::number(f);
                            return *this;
                        }

                    Streamer & operator << (const QByteArray & array)
                        {
                            buffer += array.data();
                            return *this;
                        }

                    Streamer & operator << (const char * string)
                        {
                            buffer += string;
                            return *this;
                        }

                    Streamer & operator << (const void *)
                        {
                            return *this;
                        }
        } *streamer;
        
        KDebug(DebugType t, const QString &area, DebugOutput o);
        KDebug(const KDebug &);
        ~KDebug();
        
        static void setForceDisableGUI();
        
        inline KDebug &operator << (QTextStreamManipulator /*m*/)
        { 
            return *this; 
        }
        
        inline KDebug &operator << (QTextStreamFunction /*f*/)
        {
            return *this;
        }
        
        inline KDebug& operator << (const QString &str)
        {
            *streamer << "\"" << str << "\"";
            return *this;
        };
        
        inline KDebug& operator << (char c)
        {
            *streamer << "\'" << c << "\'";
            return *this;
        };
        
        inline KDebug &operator << (bool t) 
        { 
            *streamer << (t ? "true" : "false"); 
            return *this; 
        }

        inline KDebug &operator << (signed short t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (unsigned short t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (signed int t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (unsigned int t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (signed long t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (unsigned long t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (float t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (double t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline KDebug &operator << (const char* t) 
        { 
            *streamer << t; 
            return *this; 
        }
        
        KDebug& operator << (const QDateTime&);
        KDebug& operator << (const QDate&);
        KDebug& operator << (const QTime&);
        KDebug& operator << (const QPoint &);
        KDebug& operator << (const QPointF &);
        KDebug& operator << (const QSize &);
        KDebug& operator << (const QRect &);
        KDebug& operator << (const QVariant &);
        KDebug& operator << (const QEvent*);
        KDebug& operator << (const QStringList &);
        
        
#ifdef QT_GUI_LIB
        void resaltWidget(QWidget *w, const QColor &color = QColor(Qt::magenta));
        
        KDebug& operator << (const QPixmap& p);
        KDebug& operator << (const QIcon& p);
        KDebug& operator << (const QImage& p);
        KDebug& operator << (const QRegion &);
        KDebug& operator << (const QColor &);
        KDebug& operator << (const QPen &);
        KDebug& operator << (const QBrush &);
        KDebug& operator << (const QWidget*);
        KDebug& operator << (const QLinearGradient &);
        KDebug& operator << (const QRadialGradient &);
        KDebug& operator << (const QConicalGradient &);
        KDebug& operator << (const QGradient *);
        KDebug& operator << (const QMatrix &);
        
        // static QTextBrowser *browser(QWidget *parent, int width);
        static QTextEdit *browser(QWidget *parent, int width);

#endif
  
        template <class T> KDebug& operator << ( const QList<T> &list );
        
    private slots:
        void write();
        
    private:
        DebugType m_type;
        QString m_toWrite;
        DebugOutput m_output;
        
        QString m_area;
};

template <class T> KDebug &KDebug::operator << (const QList<T> &list)
{
    *this << "(";
    typename QList<T>::ConstIterator it = list.begin();

    if (!list.isEmpty()) 
        *this << *it++;

    for (; it != list.end(); ++it) 
         *this << "," << *it;

    *this << ")";

    return *this;
}

// Global functions

inline KDebug kDebug(const QString &area = QString(), int output = KDefault)
{
    return KDebug(KDebugMsg, area, DebugOutput(output));
}

inline KDebug kDebug(int area, int output = KDefault)
{
    return KDebug(KDebugMsg, QString::number(area), DebugOutput(output));
}

inline KDebug kFatal(const QString &area = QString(), int output = KDefault)
{
    return KDebug(KFatalMsg, area, DebugOutput(output));
}

inline KDebug kFatal(int area, int output = KDefault)
{
    return KDebug(KFatalMsg, QString::number(area), DebugOutput(output));
}

inline KDebug kError(const QString &area = QString(), int output = KDefault)
{
    return KDebug(KErrorMsg, area, DebugOutput(output));
}

inline KDebug kError(int area, int output = KDefault)
{
    return KDebug(KErrorMsg, QString::number(area), DebugOutput(output));
}

inline KDebug kWarning(const QString &area = QString(), int output = KDefault)
{
    return KDebug(KWarningMsg, area, DebugOutput(output));
}

inline KDebug kWarning(int area, int output = KDefault)
{
    return KDebug(KWarningMsg, QString::number(area), DebugOutput(output));
}

#else // K_NODEBUG

class KNDebug
{
    public:
        inline KNDebug(){}
        inline KNDebug(const KNDebug &){}
        inline ~KNDebug(){}
        inline KNDebug &operator<<(QTextStreamFunction) { return *this; }
        inline KNDebug &operator<<(QTextStreamManipulator) { return *this; }
        inline KNDebug &space() { return *this; }
        inline KNDebug &nospace() { return *this; }
        inline KNDebug &maybeSpace() { return *this; }
        template<typename T> inline KNDebug &operator<<(const T &) { return *this; }

        #ifdef QT_GUI_LIB
               void resaltWidget(QWidget */*w*/, const QColor &/*color*/ = QColor(Qt::magenta))
               {
               }

               static QTextBrowser *browser(QWidget *parent) { return 0; };
        #endif
};

inline KNDebug kDebug(int = 0,int = KDefault)
{
    return KNDebug();
}

inline KNDebug kDebug(const QString &, int = KDefault)
{
    return KNDebug();
}

#define kFatal kDebug
#define kError kDebug
#define kWarning kDebug

#endif // K_NODEBUG

#ifdef __GNUC__
#define kfDebug kDebug() << __FILE__ << ":" <<__FUNCTION__ << ":: "
#else
#define kfDebug kDebug() << __FILE__<<":"<<__LINE__<<":: "
#endif

#endif
