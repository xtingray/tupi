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

module RQonf

class DetectOS
    OS = {
        0 => "Unknown",
        1 => "Windows",
        2 => "Unix",
        3 => "Linux",
        31 => "Debian",
        311 => "Ubuntu",
        312 => "14.04",
        313 => "14.10",
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
        lsb = "/etc/lsb-release"
        if File.exist?(lsb) 
           distro = ""
           version = ""
           File.open(lsb, "r") do |f|
              f.each_line do |line|
                 line = line.chop 
                 tag, value = line.split("=")

                 if tag.eql? "DISTRIB_ID"
                    distro = value 
                 end
                 if tag.eql? "DISTRIB_RELEASE"
                    version = value
                 end
              end
           end

           if distro.eql? "Ubuntu"
              if version.eql? "14.04"
                 return 312
              end
              if version.eql? "14.10"
                 return 313
              end
              return 311
           end
        end
        
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

