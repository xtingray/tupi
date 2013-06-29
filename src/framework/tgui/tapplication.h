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

#ifndef TAPPLICATION_H
#define TAPPLICATION_H

#include "thememanager.h"
#include "tvhbox.h"
#include "twizard.h"
#include "tconfig.h"
#include "tglobal.h"

#include <QApplication>
#include <QMap>
#include <QColorGroup>

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QDialog>

class QApplication;
class QString;
class ThemeDocument;
class QPalette;
class TActionManager;
class TAction;

typedef QMap<QString, QString> ParseArgs;

/**
 * @if english
 * This class manages the main settings for the others classes and modules of the application
 * @endif
 * @if spanish
 * Esta clase sirve para controlar y proveer informacion a las demas clases y modulos de la aplicacion
 * @endif
 * 
 * @author David Cuadrado
 * @TODO
 * @li We need have a cache directory (like /tmp in un*x)
*/
class T_GUI_EXPORT TApplication : public QApplication
{
    Q_OBJECT
    public:
        enum ColorSchema { DarkBlue };
        /**
         * @if english
         * Builds TApplication with the args and the args number received from the command line
         * @endif
         * @if spanish
         * Construye TApplication con los argumentos y el numero de argumentos que recibe la aplicacion 
                 * por la linea de comandos
         * @endif
         * @param argc arguments from the command line
         * @param argv number of arguments from the command line
         * @return 
         */
        TApplication(int & argc, char ** argv);
        
        
        /**
         * Destructor
         * @return 
         */
        ~TApplication();
        
        /**
         * @if english
         * Checks and validates the command line arguments
         * @endif
         * @if spanish
         * Analiza los argumentos de entrada
         * @endif
         * @param argc arguments from the command line
         * @param argv number of arguments from the command line
         */
        void parseArgs(int &argc, char **argv);
        
        /**
         * @if english
         * Applies pre-defined colors
         * @endif
         * @if spanish
         * Aplica colores predefinidos
         * @endif
         * @param cs the color schema
         */
        void applyColors(ColorSchema cs);
        
        /**
         * @if english
         * Applies a color pallete to the whole application
         * @endif
         * @if spanish
         * Aplica una paleta a toda la aplicacion
         * @endif 
         * @param p QPalette variable
         */
        void applyPalette(const QPalette &p );
        
        /**
         * @if english
         * Applies a theme to the whole application, reading the info from a path
         * @endif
         * @if spanish
         * Aplica un tema a toda la aplicacion desde una ruta
         * @endif 
         * @param file the theme path
         */
        void applyTheme(const QString &file);
        
        /**
         * @if english
         * Applies a theme to the whole application, reading the info from a XML document
         * @endif
         * @if spanish
         * Aplica un tema a toda la aplicacion desde un documento XML
         * @endif
         * @param kd ThemeDocument variable
         */
        void applyTheme(const ThemeDocument &kd);
        
        /**
         * @if english
         * This method returns true if the argument given comes from the command line
         * @endif
         * @if spanish
         * Esta funcion retorna verdadero si el argumento fue suministrado por la linea de comandos
         * @endif 
         * @param arg command line argument
         * @return 
         */
        bool isArg(const QString &arg);
        
        /**
         * @if english
         * Gets the parameter assigned to an argument
         * @endif
         * @if spanish
         * Obtiene el parametro suministrado a un argumento
         * @endif
         * @param arg the argument that contains the parameter
         * @return 
         */
        QString getParam(const QString &arg);
        
        /**
         * @if english
         * Changes the font type for the whole application
         * @endif
         * @if spanish
         * Cambia el tipo de letra de toda la aplicacion
         * @endif
         * @param font QFont variable
         */
        void changeFont(const QFont &font); // static?

        /**
         * @if english
         * 
         * @endif
         * @param group 
         */    
        TConfig *config(const QString &group = "General");

        /**
         * @if english
         * 
         * @endif
         * @param action
         * @param id
         */            
        bool insertGlobalAction(QAction *action, const QString& id);

        /**
         * @if english
         * 
         * @endif
         * @param action
         */            
        void removeGlobalAction(QAction *action);

        /**
         * @if english
         * 
         * @endif
         * @param action
         */            
        QAction *findGlobalAction(const QString &id);
        
    public slots:
        /**
         * @if english
         * Opens a settings wizard the first time the application is launched
         * @endif
         * @return 
         */
        virtual bool firstRun();
        
    private:
        ParseArgs m_parseArgs;
        ThemeManager m_themeManager;
        TActionManager *m_actionManager;
};

#define kApp static_cast<TApplication *>(qApp)

#endif
