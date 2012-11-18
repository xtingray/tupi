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
#   the Free Software Foundation; either version 3 of the License, or     #
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
# ./tools/build_mac_osx_app.sh /Users/username/Projects/tupi /Users/username/Projects/dist

TUPI_GIT_REPOSITORY=$1
TUPI_INSTALL=$2
INSTALL_NAME=$TUPI_GIT_REPOSITORY/tools/update_dylib_path.rb

declare -a LIBS=('libui.dylib' 'libtupistore.dylib' 'libtupi.dylib' \
'libquazip.dylib' 'libtupibase.dylib' 'libtupinet.dylib' 'libtupifwgui.dylib' \
'libtupifwcore.dylib' 'libtupifwsound.dylib');

cd $TUPI_GIT_REPOSITORY

make uninstall
make install

cd $TUPI_INSTALL
cp -r lib/plugins Tupi.app/Contents/MacOS
cp -r share Tupi.app/Contents/MacOS
cd Tupi.app/Contents/MacOS/plugins

find . -name "*.dylib" -exec $INSTALL_NAME $TUPI_INSTALL/Tupi.app/Contents/MacOS/plugins/{} \/opt\/local\/lib\/ @executable_path/../Frameworks/ \;

for lib in ${LIBS[@]}; do
    find . -name "*.dylib" -exec $INSTALL_NAME $TUPI_INSTALL/Tupi.app/Contents/MacOS/plugins/{} $lib @executable_path/../Frameworks/$lib \;
done


cd $TUPI_INSTALL/Tupi.app/Contents/MacOS

for lib in ${LIBS[@]}; do
    $INSTALL_NAME $TUPI_INSTALL/Tupi.app/Contents/MacOS/Tupi $lib @executable_path/../Frameworks/$lib  \;
done

cp -r /opt/local/lib/Resources/qt_menu.nib $TUPI_INSTALL/Tupi.app/Contents/Resources

cd $TUPI_INSTALL


mkdir Tupi.app/Contents/Frameworks/

for lib in ${LIBS[@]}; do
    cp lib/$lib Tupi.app/Contents/Frameworks/
done
for lib in ${LIBS[@]}; do
    $INSTALL_NAME $TUPI_INSTALL/Tupi.app/Contents/Frameworks/$lib  \/opt\/local\/lib\/ @executable_path/../Frameworks/
    for sublib in ${LIBS[@]}; do
        $INSTALL_NAME $TUPI_INSTALL/Tupi.app/Contents/Frameworks/$lib $sublib @executable_path/../Frameworks/$sublib
    done
done

macdeployqt Tupi.app -no-strip -dmg

