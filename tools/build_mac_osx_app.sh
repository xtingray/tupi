#!/bin/bash

###########################################################################
#   Project TUPI: Magia 2D                                                #
#   Project Contact: info@maefloresta.com                                 #
#   Project Website: http://www.maefloresta.com                           #
#   Project Leader: Gustav Gonzalez <info@maefloresta.com>                #
#                                                                         #
#   Developers:                                                           #
#   2010:                                                                 #
#    Gustavo Gonzalez / xtingray                                          #
#                                                                         #
#   KTooN's versions:                                                     #
#                                                                         #
#   2006:                                                                 #
#    David Cuadrado                                                       #
#    Jorge Cuadrado                                                       #
#   2003:                                                                 #
#    Fernado Roldan                                                       #
#    Simena Dinas                                                         #
#                                                                         #
#   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       #
#   License:                                                              #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>. #
###########################################################################

#Usage:
# ./tools/build_mac_osx_app.sh /Users/username/tupi/sources/tupi /Users/username/tupi/installer

QT_PATH=/Users/xtingray/Qt5.8.0
export PATH=$QT_PATH/5.8/clang_64/bin:$PATH
export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH:$QT_PATH/5.8/clang_64/lib
export DYLD_FRAMEWORK_PATH=$QT_PATH/5.8/clang_64/lib

TUPI_GIT_REPOSITORY=$1
INSTALLATION_PATH=$2
INSTALLER_SCRIPT=$TUPI_GIT_REPOSITORY/tools/update_dylib_path.rb

declare -a LIBS=('libtupigui.dylib' 'libtupistore.dylib' 'libtupi.dylib' \
'libtupibase.dylib' 'libtupinet.dylib' 'libtupifwgui.dylib' 'libtupifwcore.dylib' 'libtupicolorpalette.1.dylib' 'libtupipaintarea.1.dylib' \
'libtupianimation.1.dylib' 'libtupibrush.1.dylib' 'libtupihelp.1.dylib' 'libtupimport.1.dylib' 'libtupiexport.1.dylib' 'libtupiexposure.1.dylib' \
'libtupitimeline.1.dylib' 'libtupilibrary.1.dylib' 'libtupiscenes.1.dylib' 'libtupitwitter.1.dylib' 'libtupiplugincommon.1.dylib');

cd $TUPI_GIT_REPOSITORY

make install

cd $INSTALLATION_PATH
cp -r lib/tupi/plugins Tupi.app/Contents/MacOS

mkdir Tupi.app/Contents/MacOS/share
cp -r share/pixmaps Tupi.app/Contents/MacOS/share/
cp -r share/tupi/data Tupi.app/Contents/MacOS/share/
cp -r share/tupi/themes Tupi.app/Contents/MacOS/share/

cd Tupi.app/Contents/MacOS/plugins

find . -name "*.dylib" -exec $INSTALLER_SCRIPT $INSTALLATION_PATH/Tupi.app/Contents/MacOS/plugins/{} \/usr\/local\/lib\/ @executable_path/../Frameworks/ \;

for lib in ${LIBS[@]}; do
    find . -name "*.dylib" -exec $INSTALLER_SCRIPT $INSTALLATION_PATH/Tupi.app/Contents/MacOS/plugins/{} $lib @executable_path/../Frameworks/$lib \;
done

cd $INSTALLATION_PATH/Tupi.app/Contents/MacOS

for lib in ${LIBS[@]}; do
    $INSTALLER_SCRIPT $INSTALLATION_PATH/Tupi.app/Contents/MacOS/Tupi $lib @executable_path/../Frameworks/$lib  \;
done

cp -r $QT_PATH/MaintenanceTool.app/Contents/Resources/qt_menu.nib $INSTALLATION_PATH/Tupi.app/Contents/Resources

cd $INSTALLATION_PATH

mkdir Tupi.app/Contents/Frameworks/

for lib in ${LIBS[@]}; do
    cp lib/tupi/$lib Tupi.app/Contents/Frameworks/
done
for lib in ${LIBS[@]}; do
    $INSTALLER_SCRIPT $INSTALLATION_PATH/Tupi.app/Contents/Frameworks/$lib  \/usr\/local\/lib\/ @executable_path/../Frameworks/
    for sublib in ${LIBS[@]}; do
        $INSTALLER_SCRIPT $INSTALLATION_PATH/Tupi.app/Contents/Frameworks/$lib $sublib @executable_path/../Frameworks/$sublib
    done
done

macdeployqt Tupi.app -dmg -libpath=/usr/local/lib

