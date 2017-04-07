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

require_relative 'info'
require_relative 'extensions'

module RQonf

class QMake

    $found = ""

    def initialize
        @make = "make"
    end

    # This method check if the current version of Qt is valid for Tupi compilation    
    def findQMake(minqtversion, verbose, qtdir)
        command = ""
        ["qmake", "qmake-qt5"].each do |path|

            Info.info << "Testing for #{path}... "

            IO.popen("which #{path}") { |result|
                 if qtdir.length > 0
                    command = qtdir + "/bin/qmake"
                 else
                    pathVar = result.readlines.join("").split(":")
                    if pathVar.length > 0
                       command = pathVar[0].chop
                    end
                 end
            }
            if command.length != 0
                break
            end
        end
        if command.length == 0
            return false
        end
        qtversion = ""
        version = []

        IO.popen("#{command} -query QT_VERSION") { |prc|
                 found = prc.readlines.join("")
                 version = found.split(".")
                 if (found.length != 0)
                     qtversion = found.chop
                 end
        }

        minver = minqtversion.split(".")

        version.size.times { |i|
                if i == 0
                   if version[i] < minver[i]
                      return false 
                   end
                end

                if i == 1
                   if version[i] < minver[i]
                      return false
                   end
                end

                if i == 2
                   if version[i] < minver[i]
                      return false
                   end
                end
        }
                
        @path = command

        if verbose == 1
            print "(Found: #{qtversion}) "
        end

        return true
    end

    def query(var)
        output = `#{@path} -query #{var}`
        output
    end
    
    def run(args = "", recur = false)
        options = ""
        if recur
            options += "-recursive"
        end
        output = `#{@path} #{options} #{args}`

        if output.strip.empty?
            return true
        end
        
        return false
    end
    
    def compile(debug)
         flags = "2>/dev/null"
         if debug == 1 
             flags = ""
             print $endl
             Info.info << "Compiling test..." <<  $endl
         end

        fork do
            IO.popen("#{@make} clean #{flags}") { |c|
                output = c.readlines
                if debug == 1
                    print "#{output}"
                end
            }
            
            times = 0
            endcode = 2
            
            while endcode == 2 and times <= 3
                IO.popen("#{@make} #{flags}", "r") { |c|
                    output = c.readlines
                    if debug == 1
                        print "#{output}"
                    end
                }

                if debug == 1
                    Info.info << "Result: "
                end
                
                endcode = $? >> 8
                
                times += 1
            end
            
            exit -1 if endcode != 0
        end
        
        Process.wait
        
        if $? != 0
            return false
        end
        
        return true
    end
end

end #module


