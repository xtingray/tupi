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

require 'qonf/qmake'
require 'qonf/detectos'
require 'rexml/parsers/sax2parser'
require 'rexml/sax2listener'

require 'qonf/info'

module RQonf

class Test
    include REXML
    
    attr_reader :rules
    attr_reader :optional
    
    def initialize(rulesPath, qmake)
        @rules = rulesPath
        @qmake = qmake
        
        @optional = false
    end
    
    def run(config, conf, debug, isLucid)

        parser = Parser.new
        parser.os = DetectOS::OS[DetectOS.whatOS].to_s.downcase
        
        return false if not parser.parse(@rules) or parser.name.empty?
        
        @optional = parser.optional
        
        Info.info << "Checking for " << parser.name << "... "
        
        dir = "#{File.dirname(@rules)}/#{parser.dir}"

        if $DEBUG
            Info.warn << "Running in " << dir << $endl
        end
        
        cwd = Dir.getwd

        if File.exists?(dir)
            if File.stat(dir).directory?
                Dir.chdir(dir)

                extraLib = ""
                extraInclude = ""
                if RUBY_PLATFORM == "x86_64-linux"
                   extraLib = "-L/usr/lib64 "
                end

                if File.dirname(@rules).end_with?("ffmpeg")  
                   if conf.hasArgument?("with-ffmpeg")
                      ffmpegLib = conf.argumentValue("with-ffmpeg") + "/lib"
                      extraLib += "-L#{ffmpegLib}"
                      extraInclude = conf.argumentValue("with-ffmpeg") + "/include"
                      qmakeLine = "'LIBS += #{extraLib}'";
                      qmakeLine += " 'INCLUDEPATH += #{extraInclude}'";
                   end
                else
                   qmakeLine = ""
                   if extraLib.length > 0 
                      qmakeLine = "'LIBS += #{extraLib} #{parser.libs.join(" ")}'";
                   end
                end

                if isLucid
                   qmakeLine = "'DEFINES += K_LUCID' " + qmakeLine
                end

                @qmake.run(qmakeLine, true)

                if not @qmake.compile(debug)
                    Dir.chdir(cwd)
                    
                    print "[ \033[91mFAILED\033[0m ]\n"

                    priority = "\033[92moptional\033[0m"
                    flag = "\033[92mCOULD\033[0m"
                    if @optional == false
                       priority = "\033[91mrequired\033[0m"
                       flag = "\033[91mMUST\033[0m"
                    end

                    Info.info << "Priority: " << priority << "\n"

                    
                    # Provide solution
                    solution = parser.solution
                    
                    Info.warn << "Seems like you are running " << parser.os << "..." << $endl
                    Info.warn << "You " << flag << " install these dependencies: " << solution[:package] << $endl
                    Info.warn << "URL: " << solution[:url] << $endl
                    Info.warn << solution[:comment] << $endl
                    
                    return false
                end
            else
                Dir.chdir(cwd)
                raise QonfException.new("'#{dir}' isn't a directory!")
                return false
            end
        else
            Dir.chdir(cwd)
            raise QonfException.new("'#{dir}' doesn't exists!")
            return false
        end
        
        Dir.chdir(cwd)

        parser.includes.each { |inc|
            config.addIncludePath(inc)
        }
        
        parser.libs.each { |lib|
            config.addLib(lib)
        }
        
        parser.defines.each { |define|
            config.addDefine(define)
        }

        parser.modules.each { |mod|
            config.addModule(mod)
        }
        
        print "[ \033[92mOK\033[0m ]\n"

        priority = "\033[92moptional\033[0m"
        if @optional == false 
           priority = "\033[91mrequired\033[0m" 
        end

        Info.info << "Priority: " << priority << "\n"
        
        return true
    end
    
    def to_s
        @rules
    end
    
    private
    class Parser
        include REXML::SAX2Listener
        
        attr_reader :name
        attr_reader :dir
        attr_reader :defines
        attr_reader :includes
        attr_reader :libs
        attr_reader :modules
        attr_accessor :os
        attr_reader :solution
        attr_reader :optional
        
        def initialize
            @name = ""
            @current_tag = ""
            @section = ""
            @dir = ""
            @defines = []
            @includes = []
            @libs = []
            @modules = []
            @solution = {}
            @os = ""
            @optional = false
        end
        
        def start_document
        end
        
        def end_document
        end
        
        def start_element( uri, localname, qname, attributes)
            case qname
                when "qonf"
                    @optional = (attributes["optional"] == "true")
                when "test"
                    @section = qname
                    @name = attributes["name"].to_s
                when "dir"
                    if @section == "test"
                        @dir = attributes["path"]
                    end
                when "provide"
                    @section = qname
                when "define"
                    if @section  == "provide"
                        @defines << attributes["value"]
                    end
                when "include"
                    if @section  == "provide"
                        @includes << attributes["path"]
                    end
                when "lib"
                    if @section  == "provide"
                        @libs << attributes["path"]
                    end
                when "module"
                    if @section == "provide"
                        @modules << attributes["value"]
                    end
                when "command"
                    if @section == "provide"
                        if @current_tag == "includes"
                            IO.popen(attributes["value"]) { |c|
                                output = c.readlines.join("").split(" ")
                                
                                output.each { |i|
                                    if i =~ /-I(.*)/
                                        @includes << $1.chomp
                                    elsif not i =~ /-.*/
                                        @includes << i
                                    end
                                }
                            }
                        elsif @current_tag == "libs"
                            IO.popen(attributes["value"]) { |c|
                                output = c.readlines.join("").split(" ")
                                
                                output.each { |lib|
                                    if lib =~ /-l(.*)/
                                        @libs << lib
                                    elsif lib[0].chr != "-"
                                        @libs << lib
                                    end
                                }
                                @libs.concat(output)
                            }
                        end
                    end
                when "solution"
                    @section = "solution"
                when "windows"
                    if @os == qname and @solution.size == 0
                        fillSolution(attributes)
                    end
                when "macosx"
                    if @os == qname and @solution.size == 0
                        fillSolution(attributes)
                    end
                when "linux"
                    if @os == qname and @solution.size == 0
                        fillSolution(attributes)
                    end
                when "distribution"
                    if @os == attributes["name"].to_s.downcase and @solution.size == 0
                        fillSolution(attributes)
                    end
            end
            
            @current_tag = qname
        end
        
        def end_element(uri, localname, qname)
            case qname
                when ""
            end
            
        end
        
        def characters(text)
            text = text.strip
            if not text.empty?
            end
        end
        
        def parse(path)
            sax2 = REXML::Parsers::SAX2Parser.new( File.new( path ) )
            sax2.listen(self)
            
            begin
                sax2.parse
            rescue
                return false
            end
            
            return true
        end
        
        private
        def fillSolution(attributes)
            @solution[:package] = attributes["package"]
            @solution[:url] = attributes["url"]
            @solution[:comment] = attributes["comment"]
        end
    end
end

end #module

