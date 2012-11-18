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

module RQonf

class DetectOS
    OS = {
        0 => "Unknown",
        1 => "Windows",
        2 => "Unix",
        3 => "Linux",
        31 => "Debian",
        311 => "Ubuntu",
        32 => "Gentoo",
        4 => "Mac OS X"
    }
    
    def self.whatOS
        @os = 0
        if File::SEPARATOR == "\\"
            return 1
        elsif File::SEPARATOR == "/"
            @os = 2
            
            linux = DetectOS.tryToFindLinuxDistribution
            @os = linux if linux > 0
        end
        
        
        return @os
    end
    
    def self.tryToFindLinuxDistribution
        cwd = Dir.getwd
        Dir.chdir("/etc/")
        
        Dir["*{-,_}{version,release}"].each { |path|
            if path.downcase =~ /^(\w*)(-|_)(version|release)/
                DetectOS::OS.each { |id, os|
                    if os.downcase == $1
                        Dir.chdir(cwd)
                        
                        if $1 == "debian"
                            if `apt-cache -v`.include?("ubuntu")
                                return 311
                            end
                        end
                        
                        return id
                    end
                }
            end
        }
        
        Dir.chdir(cwd)
        
        if `uname`.downcase == "linux"
            return 2
        end
        
        return 0
    end
end

end

if __FILE__ == $0
    puts RQonf::DetectOS::OS[RQonf::DetectOS.whatOS]
end

