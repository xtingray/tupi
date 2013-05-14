# encoding: UTF-8

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

require './qonf/test'
require './qonf/config'
require './qonf/info'
require './qonf/qonfexception'
require './qonf/makefile'

module RQonf

  class Configure
    attr_reader :qmake, :statusFile

    def initialize(args)
      @statusFile = Dir.getwd+"/configure.status"

      @tests = []
      @testsDir = Dir.getwd

      @options = {}
      parseArgs(args)

      @qmake = QMake.new
      @statusFile = Dir.getwd + "/configure.status"

      @tests = []
      @testsDir = Dir.getwd

      @options = {}
      parseArgs(args)

      @qmake = QMake.new
      @properties = {}

      @ffmpeg = true

      setPath()
      Makefile::setArgs(@options)
    end

    def load_properties(properties_filename)
      File.open(properties_filename, 'r') do |properties_file|
        properties_file.read.each_line do |line|
          line.strip!
          if (line[0] != ?# and line[0] != ?=)
            i = line.index('=')
            if (i)
              @properties[line[0..i - 1].strip] = line[i + 1..-1].strip
            else
              @properties[line] = ''
            end
          end
        end
      end
      @properties
    end

    def hasProperty?(arg)
      @properties.has_key?(arg)
    end

    def propertyValue(arg)
      @properties[arg].to_s
    end

    def hasArgument?(arg)
      @options.has_key?(arg)
    end

    def argumentValue(arg)
      @options[arg].to_s
    end

    def setTestDir(dir)
      @testsDir = dir
    end

    def disableFFmpeg()
      @ffmpeg = false
    end

    def verifyQtVersion(minqtversion, qtdir)
      Info.info << "Checking for Qt >= " << minqtversion << "... "

      if @qmake.findQMake(minqtversion, true, qtdir)
        print "[ \033[92mOK\033[0m ]\n"
      else
        print "[ \033[91mFAILED\033[0m ]\n"
        raise QonfException.new("\033[91mInvalid Qt version\033[0m.\n   Please, upgrade to #{minqtversion} or higher (Visit: http://qt.nokia.com)")
      end
    end

    def createTests
      @tests.clear
      findTest(@testsDir)
    end

    def runTests(config, conf, debug, isLucid)
      @tests.each { |test|
        if not test.run(config, conf, debug, isLucid) and not test.optional
          raise QonfException.new("\033[91mMissing required dependency\033[0m")
        end
      }
    end

    def createMakefiles
      Info.info << "Creating makefiles..." << $endl

      if RUBY_PLATFORM.downcase.include?("darwin")
        qmakeLine = "'CONFIG += console warn_on' 'INCLUDEPATH += /usr/local/include/quazip LIBS += -L/usr/local/lib -lavcodec -lavutil -lavformat -framework CoreFoundation'"
        @qmake.run(qmakeLine, true)
      else
        @qmake.run("", true)
      end

      Info.info << "Updating makefiles and source code..." << $endl

      @makefiles = Makefile::findMakefiles(Dir.getwd)

      @makefiles.each { |makefile|
        Makefile::override(makefile)
      }
    end

    private
    def parseArgs(args)

      optc = 0
      last_opt = ""

      while args.size > optc

        arg = args[optc].strip

        if arg =~ /^--([\w-]*)={0,1}([\W\w]*)/

          opt = $1.strip
          val = $2.strip
          @options[opt] = val
          last_opt = opt

        else

          # arg is an arg for option
          if not last_opt.to_s.empty? and @options[last_opt].to_s.empty?
            @options[last_opt] = arg
          else
            raise "Invalid option: #{arg}"
          end

        end

        optc += 1

      end
    end

    def findTest(path)
      if $DEBUG
        Info.warn << "Searching qonfs in: " << path << $endl
      end
      Dir.foreach(path) { |f|

        file = "#{path}/#{f}"

        if File.stat(file).directory?
          if not f =~ /^\./
            findTest(file)
          end
        elsif file =~ /.qonf$/
          if file.include? "ffmpeg"
             if @ffmpeg
                Info.warn << "Adding ffmpeg support: " << @ffmpeg << $endl
                @tests << Test.new(file, @qmake)
             end
          else
             @tests << Test.new(file, @qmake)
          end
        end
      }
    end

    private
    def setPath()
      if @options['prefix'].nil? then
        @options['prefix'] = "/usr"
      end

      if @options['bindir'].nil? then
         @options['bindir'] = @options['prefix'] + "/bin"
      end

      if @options['libdir'].nil? then
        if RUBY_PLATFORM == "x86_64-linux"
           @options['libdir'] = @options['prefix'] + "/lib64/tupi"
        else
           @options['libdir'] = @options['prefix'] + "/lib/tupi"
        end
      elsif !@options['libdir'].end_with? "tupi" then
            @options['libdir'] = @options['libdir'] + "/tupi"
      end

      if @options['includedir'].nil? then
         @options['includedir'] = @options['prefix'] + "/include"
      end

      if @options['sharedir'].nil? then
        @options['sharedir'] = @options['prefix'] + "/share/tupi"
      end

      launcher_prefix = @options['prefix']
      launcher_sharedir = @options['sharedir']
      launcher_libdir = @options['libdir']
      launcher_bindir = @options['bindir']

      if @options['package-build'].nil? then
        @options['package-build'] = "/usr"
      else
        @options['package-build'] = @options['prefix']
        launcher_prefix = "/usr"
        launcher_sharedir = "/usr/share/tupi"
        if RUBY_PLATFORM == "x86_64-linux"
           launcher_libdir = "/usr/lib64/tupi"
        else
           launcher_libdir = "/usr/lib/tupi"
        end
        launcher_bindir = "/usr/bin"
      end

      newfile = "#!/bin/bash\n\n"
      newfile += "export TUPI_HOME=\"" + launcher_prefix + "\"\n"
      newfile += "export TUPI_SHARE=\"" + launcher_sharedir + "\"\n"
      newfile += "export TUPI_LIB=\"" + launcher_libdir + "\"\n"
      newfile += "export TUPI_PLUGIN=\"" + launcher_libdir + "/plugins\"\n"
      newfile += "export TUPI_BIN=\"" + launcher_bindir + "\"\n\n"

      if RUBY_PLATFORM.downcase.include?("darwin")
        newfile += "export DYLD_FALLBACK_LIBRARY_PATH=\"\$\{TUPI_LIB\}:\$\{TUPI_PLUGIN\}:$DYLD_FALLBACK_LIBRARY_PATH\"\n\n"
        newfile += "open ${TUPI_BIN}/Tupi.app $*"
      else
        if @options['with-ffmpeg'].nil? then 
           newfile += "export LD_LIBRARY_PATH=\"\$\{TUPI_LIB\}:\$\{TUPI_PLUGIN\}:$LD_LIBRARY_PATH\"\n\n"
        else
           newfile += "export LD_LIBRARY_PATH=\"" + @options['with-ffmpeg'] + "/lib:\$\{TUPI_LIB\}:\$\{TUPI_PLUGIN\}:$LD_LIBRARY_PATH\"\n\n" 
        end
        newfile += "exec ${TUPI_BIN}/tupi.bin $*"
      end

      launcher = File.open("launcher/tupi", "w") { |f|
        f << newfile
      }

        newfile = "[Desktop Entry]\n"
        # newfile += "Encoding=UTF-8\n"
        newfile += "Name=Tupi: Open 2D Magic\n"
        newfile += "Name[es]=Tupí: Magia 2D Libre\n"
        newfile += "Name[pt]=Tupí: Magia 2D Libre\n"
        newfile += "Name[ru]=Tupi: Open 2D Magic\n"
        newfile += "Exec=" + launcher_bindir + "/tupi\n"
        newfile += "Icon=tupi\n"
        newfile += "Type=Application\n"
        newfile += "MimeType=application/tup;\n"
        newfile += "Categories=Graphics;2DGraphics;RasterGraphics;\n"
        newfile += "Comment=2D Animation Toolkit\n"
        newfile += "Comment[es]=Herramienta para Animación 2D\n"
        newfile += "Comment[pt]=Ferramenta de animação 2D\n"
        newfile += "Comment[ru]=Создание двухмерной векторной анимации\n"
        newfile += "Terminal=false\n"

        File.open("launcher/tupi.desktop", "w") { |f|
          f << newfile
        }

      newmakefile = ""
      File.open("src/components/help/help/css/tupi.ini", "r") { |f|
        lines = f.readlines
        index = 0
        while index < lines.size
          line = lines[index]
          if line.include? "TUPI_SHARE" then
            newmakefile += "#{line.gsub(/\$\(TUPI_SHARE\)/, launcher_sharedir)}"
          else
            newmakefile += line
          end

          index += 1
        end
      }

      File.open("src/components/help/help/css/tupi.css", "w") { |f|
        f << newmakefile
      }

    end
  end
end # module
