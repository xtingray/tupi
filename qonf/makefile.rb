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

require 'rbconfig'

module RQonf
    class Makefile

        def self.setArgs(paths)
            @options = paths 
        end

        def self.findMakefiles(path)
            makefiles = []
            Dir.foreach(path) { |f|
                file = "#{path}/#{f}"
                
                if File.stat(file).directory?
                    if not f =~ /^\./
                        makefiles.concat findMakefiles(file)
                    end
                elsif f.downcase == "makefile"
                    makefiles << file
                end
            }
            
            makefiles
        end
        
        def self.override(makefile)
            newmakefile = ""
            File.open(makefile, "r") { |f|
                      lines = f.readlines
                      index = 0
                      while index < lines.size
                            line = lines[index]
                            if line.include? "INSTALL_ROOT" then
                               if line.include? "\$\(INSTALL_ROOT\)\/lib" then
                                  newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/lib/, "\$\(DESTDIR\)" + @options['libdir'])}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/include" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/include/, @options['includedir'])}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/plugins" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/plugins/, "\$\(DESTDIR\)" + @options['libdir'] + "/plugins")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/data" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/data/, "\$\(DESTDIR\)" + @options['sharedir'] + "/data")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/bin" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/bin/, "\$\(DESTDIR\)" + @options['bindir'])}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/themes" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/themes/, "\$\(DESTDIR\)" + @options['sharedir'] + "/themes")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/applications" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/applications/, "\$\(DESTDIR\)" + @options['prefix'] + "/share/applications")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/pixmaps" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/pixmaps/, "\$\(DESTDIR\)" + @options['prefix'] + "/share/pixmaps")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/man1" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/man1/, "\$\(DESTDIR\)" + @options['prefix'] + "/share/man/man1")}"
                               elsif line.include? "\$\(INSTALL_ROOT\)\/share\/mime\/packages" then
                                     newmakefile += "#{line.gsub(/\$\(INSTALL_ROOT\)\/share\/mime\/packages/, "\$\(DESTDIR\)" + @options['prefix'] + "/share/mime/packages")}"
                               end
                            else
                               if line.include? "DESTDIR" then
                                  newmakefile += ""
                               else
                                  newmakefile += line
                               end
                            end
                            index += 1
                      end
            }

            File.open(makefile, "w") { |f|
                f << newmakefile
            }
        end
    end
end


