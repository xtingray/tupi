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

#ifndef TDEBUG_H
#define TDEBUG_H

#ifndef T_CORE_EXPORT
  #if defined(QT_SHARED) || defined(QT_PLUGIN)
    #define T_CORE_EXPORT Q_DECL_EXPORT
  #else
    #define T_CORE_EXPORT
  #endif
#endif

#include <QFile>
#include <QString>
#include <QDateTime>
#include <QPoint>
#include <QPointF>
#include <QRect>
#include <QVariant>
#include <QSize>
#include <QEvent>

#ifdef QT_GUI_LIB

#include <QRegion>
#include <QPen>
#include <QBrush>
#include <QImage>
#include <QIcon>
#include <QPixmap>
#include <QWidget>
#include <QMessageBox>
#include <QSyntaxHighlighter>
#include <QMatrix>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QDebug>

#endif

#include <QTextStream>
#include <QStringList>
#include <QTextEdit>

#ifdef QT_GUI_LIB
#include <QColor>
#endif

/**
 * @author David Cuadrado
*/

#ifdef __GNUC__
#define T_FUNCINFO tDebug() << "[" << __PRETTY_FUNCTION__ << "] "
#define T_FUNCINFOX(area) tDebug(area) << "[" << __PRETTY_FUNCTION__ << "] "
#define TINIT tDebug("class") << "[Initializing " << __FUNCTION__ << "]"
#define TEND tDebug("class") << "[Destroying " << __FUNCTION__ << "]"
#define FUNC_NOT_IMPLEMENTED tWarning() << __FILE__ << ":" << __LINE__ << " " << __PRETTY_FUNCTION__ << " not implemented yet";

#else

#define T_FUNCINFO
#define T_FUNCINFOX(area)
#define TINIT tDebug("class") << "[Initializing " << __FILE__ << ":" << __LINE__ << "] "
#define TEND tDebug("class") << "[Destroying " << __FILE__ << ":" << __LINE__ << "] "
#define FUNC_NOT_IMPLEMENTED tWarning() << __FILE__<<":"<<__LINE__ << " not implemented yet";
#endif

#define K_CHECKPTR(ptr) if ( ptr == 0 ) { tFatal() << __PRETTY_FUNCTION__ << ": " << #ptr << " is NULL"; } 
#define SHOW_VAR(arg) tDebug() << #arg << " = " << arg;

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
    TDebugMsg = 0,
    TWarningMsg,
    TErrorMsg,
    TFatalMsg
};

enum DebugOutput
{
    TDefault = -1,
    TNone = 0,
    TFileOutput,
    TBoxOutput,
    TShellOutput,
    TBrowserOutput
};

#if !defined(K_NODEBUG)
class T_CORE_EXPORT TDebug
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
        
        TDebug(DebugType t, const QString &area, DebugOutput o);
        TDebug(const TDebug &);
        ~TDebug();
        
        static void setOutputChannel();
        
        inline TDebug &operator << (QTextStreamManipulator /*m*/)
        { 
            return *this; 
        }
        
        inline TDebug &operator << (QTextStreamFunction /*f*/)
        {
            return *this;
        }
        
        inline TDebug& operator << (const QString &str)
        {
            *streamer << "\"" << str << "\"";
            return *this;
        };
        
        inline TDebug& operator << (char c)
        {
            *streamer << "\'" << c << "\'";
            return *this;
        };
        
        inline TDebug &operator << (bool t) 
        { 
            *streamer << (t ? "true" : "false"); 
            return *this; 
        }

        inline TDebug &operator << (signed short t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (unsigned short t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (signed int t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (unsigned int t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (signed long t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (unsigned long t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (float t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (double t) 
        { 
            *streamer << t; 
            return *this; 
        }

        inline TDebug &operator << (const char* t) 
        { 
            *streamer << t; 
            return *this; 
        }
        
        TDebug& operator << (const QDateTime&);
        TDebug& operator << (const QDate&);
        TDebug& operator << (const QTime&);
        TDebug& operator << (const QPoint &);
        TDebug& operator << (const QPointF &);
        TDebug& operator << (const QSize &);
        TDebug& operator << (const QRect &);
        TDebug& operator << (const QVariant &);
        TDebug& operator << (const QEvent*);
        TDebug& operator << (const QStringList &);
        
        
#ifdef QT_GUI_LIB
        void resaltWidget(QWidget *w, const QColor &color = QColor(Qt::magenta));
        
        TDebug& operator << (const QPixmap& p);
        TDebug& operator << (const QIcon& p);
        TDebug& operator << (const QImage& p);
        TDebug& operator << (const QRegion &);
        TDebug& operator << (const QColor &);
        TDebug& operator << (const QPen &);
        TDebug& operator << (const QBrush &);
        TDebug& operator << (const QWidget*);
        TDebug& operator << (const QLinearGradient &);
        TDebug& operator << (const QRadialGradient &);
        TDebug& operator << (const QConicalGradient &);
        TDebug& operator << (const QGradient *);
        TDebug& operator << (const QMatrix &);
        
        // static QTextBrowser *browser(QWidget *parent, int width);
        static QTextEdit *browser(QWidget *parent, int width);
        static void setProjectStatus(bool status);
#endif
  
        template <class T> TDebug& operator << ( const QList<T> &list );
        
    private slots:
        void write();
        
    private:
        DebugType m_type;
        QString m_toWrite;
        DebugOutput m_output;
        
        QString m_area;
};

template <class T> TDebug &TDebug::operator << (const QList<T> &list)
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

inline TDebug tDebug(const QString &area = QString(), int output = TDefault)
{
    return TDebug(TDebugMsg, area, DebugOutput(output));
}

inline TDebug tDebug(int area, int output = TDefault)
{
    return TDebug(TDebugMsg, QString::number(area), DebugOutput(output));
}

inline TDebug tFatal(const QString &area = QString(), int output = TDefault)
{
    return TDebug(TFatalMsg, area, DebugOutput(output));
}

inline TDebug tFatal(int area, int output = TDefault)
{
    return TDebug(TFatalMsg, QString::number(area), DebugOutput(output));
}

inline TDebug tError(const QString &area = QString(), int output = TDefault)
{
    return TDebug(TErrorMsg, area, DebugOutput(output));
}

inline TDebug tError(int area, int output = TDefault)
{
    return TDebug(TErrorMsg, QString::number(area), DebugOutput(output));
}

inline TDebug tWarning(const QString &area = QString(), int output = TDefault)
{
    return TDebug(TWarningMsg, area, DebugOutput(output));
}

inline TDebug tWarning(int area, int output = TDefault)
{
    return TDebug(TWarningMsg, QString::number(area), DebugOutput(output));
}

#else // K_NODEBUG

class TNDebug
{
    public:
        inline TNDebug(){}
        inline TNDebug(const TNDebug &){}
        inline ~TNDebug(){}
        inline TNDebug &operator<<(QTextStreamFunction) { return *this; }
        inline TNDebug &operator<<(QTextStreamManipulator) { return *this; }
        inline TNDebug &space() { return *this; }
        inline TNDebug &nospace() { return *this; }
        inline TNDebug &maybeSpace() { return *this; }
        template<typename T> inline TNDebug &operator<<(const T &) { return *this; }

        #ifdef QT_GUI_LIB
               void resaltWidget(QWidget */*w*/, const QColor &/*color*/ = QColor(Qt::magenta))
               {
               }

               static QTextBrowser *browser(QWidget *parent) { Q_UNUSED(parent); return 0; };
        #endif
};

inline TNDebug tDebug(int = 0,int = TDefault)
{
    return TNDebug();
}

inline TNDebug tDebug(const QString &, int = TDefault)
{
    return TNDebug();
}

#define tFatal tDebug
#define tError tDebug
#define tWarning tDebug

#endif // K_NODEBUG

#ifdef __GNUC__
#define tfDebug tDebug() << __FILE__ << ":" <<__FUNCTION__ << ":: "
#else
#define tfDebug tDebug() << __FILE__<<":"<<__LINE__<<":: "
#endif

#endif
