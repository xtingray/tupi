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

class Config
    def initialize
        @includePath = []
        @libs = []
        @modules = []
        @defines = []
        @scopes = []
        @options = []
    end
    
    def addIncludePath(path)
        @includePath << path
    end
    
    def addLib(lib)
        @libs << lib
    end
    
    def addModule(mod)
        @modules << mod
    end
    
    def addDefine(define)
        @defines << define
    end
    
    def addScope(name)
        scope = Scope.new(name)
        @scopes << scope
        
        scope
    end

    def addOption(opt)
        @options << opt
    end
    
    def save(path)
        if path[0].chr == File::SEPARATOR
            path = path
        else
            path = "#{Dir.getwd}/#{path}"
        end
        
        File.open(path, "w") { |f|
            f << "# Generated automatically at #{Time.now}! PLEASE DO NOT EDIT!"<< $endl
            if not @includePath.empty?
                f << "INCLUDEPATH += " << @includePath.uniq.join(" ") << $endl
            end
            
            if not @libs.empty?
                f << "LIBS += " << @libs.uniq.join(" ") << $endl
            end
            
            if not @modules.empty?
                f << "QT += " << @modules.uniq.join(" ") << $endl
            end
            
            if not @defines.empty?
                f << "DEFINES += " << @defines.uniq.join(" ") << $endl
            end
            
            if not @scopes.empty?
                @scopes.each { |scope|
                    f << scope.to_s
                }
            end

            if not @options.empty?
                f << "CONFIG += " << @options.uniq.join(" ") << $endl
            end
        }

        File.open("src/framework/tupconfig.pri", "w") { |f|
            f << "# Generated automatically at #{Time.now}! PLEASE DO NOT EDIT!"<< $endl
            f << "contains(DEFINES, HAVE_FFMPEG){" << $endl
            f << "LIBS += -lavcodec -lavformat -lavutil" << $endl
            f << "}" << $endl
            if @defines.include? 'ADD_HEADERS'
               f << "DEFINES += ADD_HEADERS" << $endl
            end
            f << $endl
            f << "QT += opengl core gui svg xml network" << $endl
            f << "unix {" << $endl
            f << "   OBJECTS_DIR = .obj" << $endl
            f << "   UI_DIR = .ui" << $endl
            f << "   MOC_DIR = .moc" << $endl
            f << "}" << $endl

            if not @options.empty?
               f << "CONFIG += " << @options.uniq.join(" ") << $endl
            end
        }

    end
    
    private
    class Scope
        def initialize(name)
            @variables = {}
            @name = name
        end
        
        def addVariable(var, val)
            @variables[var] = val
        end
        
        def to_s
            ret = "#{@name} {\n"
            @variables.each { |key, val|
                ret += "    #{key} = #{val}\n"
            }
            ret += "}\n"
            
            ret
        end
    end
end

end # module
