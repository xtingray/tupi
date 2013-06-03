#!/usr/bin/ruby

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

# TODO: This script must detect if every command line given is valid 
#       Currently, it just try to check if some of them are included or not

require 'fileutils'
require_relative 'qonf/configure'
require_relative 'qonf/info'
require_relative 'qonf/defaults'

begin
    conf = RQonf::Configure.new(ARGV)

    if conf.hasArgument?("help") or conf.hasArgument?("h")
       puts <<_EOH_
Use: ./configure [options]
  options:
  --help:               Show this message
  --prefix=[path]:      Sets installation path [/usr]
  --bindir=[path]:      Set binaries path [/usr/bin]
  --libdir=[path]:      Set library path [/usr/lib/tupi | /usr/lib64/tupi]
  --sharedir=[path]:    Set data path [/usr/share]
  --with-ffmpeg=[path]: Set ffmpeg installation path [/usr]
  --without-ffmpeg:     Disable ffmpeg support
  --without-debug:      Disable debug
  --with-qtdir=[path]:  Set Qt directory [i.e. /usr/local/qt]
  --package-build:      Option exclusive for package maintainers
  --install-headers:    Include header files as part of installation
_EOH_
        exit 0
    end

    if conf.hasArgument?("with-ffmpeg") and conf.hasArgument?("without-ffmpeg")  
       Info.error << " ERROR: Options --with-ffmpeg and --without-ffmpeg are mutually exclusive\n"
       exit 0
    end

    config = RQonf::Config.new

    distro = ""
    if FileTest.exists?("/etc/lsb-release")
       conf.load_properties("/etc/lsb-release")
       if conf.hasProperty?("DISTRIB_CODENAME")
          distro = conf.propertyValue("DISTRIB_CODENAME")
       end
       if conf.hasProperty?("DISTRIB_ID")
          distroName = conf.propertyValue("DISTRIB_ID") 
          if distroName == "Ubuntu"
             config.addDefine("UBUNTU")
          end
       end
    end

    if conf.hasArgument?("with-qtdir")
       qtdir = conf.argumentValue("with-qtdir")
       conf.verifyQtVersion("4.7.0", qtdir)
       if distro == "lucid"
          config.addDefine("K_LUCID")
       end
    else
       if distro == "lucid"
          Info.error << " ERROR: If you are using Ubuntu Lucid (10.04). You must use the parameter --with-qtdir\n"
          Info.error << " Try the option --help for more info\n"
          exit 0
       else
          conf.verifyQtVersion("4.7.0", "")
       end
    end

    avcodecFile = "/usr/include/libavcodec/version.h"

    if conf.hasArgument?("with-ffmpeg")
       ffmpegDir = conf.argumentValue("with-ffmpeg")
       if File.directory? ffmpegDir 
          ffmpegLib = conf.argumentValue("with-ffmpeg") + "/lib"
          ffmpegInclude = conf.argumentValue("with-ffmpeg") + "/include"
          avcodecFile = ffmpegInclude + "/libavcodec/version.h"
          config.addLib("-L" + ffmpegLib)
          config.addIncludePath(ffmpegInclude)
       else
          Info.error << " ERROR: ffmpeg directory does not exist!\n"
          exit 0
       end
    end

    if FileTest.exists?(avcodecFile)
       major = `egrep LIBAVCODEC_VERSION_MAJOR #{avcodecFile} | head -n 1`
       minor = `egrep LIBAVCODEC_VERSION_MINOR #{avcodecFile} | head -n 1`
       majorVersion = major.split
       minorVersion = minor.split
       destination = "src/plugins/export/ffmpegplugin/tffmpegmoviegenerator.cpp"
       if majorVersion[2] >= "54" and minorVersion[2] >= "92"
          FileUtils.cp("src/plugins/export/ffmpegplugin/tffmpegmoviegenerator.new.cpp", destination)
       else
          FileUtils.cp("src/plugins/export/ffmpegplugin/tffmpegmoviegenerator.old.cpp", destination)
       end
    end

    debug = 1
    if conf.hasArgument?("without-debug")
       debug = 0
    end

    if conf.hasArgument?("without-ffmpeg")
       Info.warn << "Disabling ffmpeg support: " << $endl
       conf.disableFFmpeg()
    end

    conf.createTests
    conf.setTestDir("configure.tests")

    if distro == "lucid"
       conf.runTests(config, conf, debug, true)
    else
       conf.runTests(config, conf, debug, false)
    end

    config.addModule("core")
    config.addModule("gui")
    config.addModule("svg")
    config.addModule("xml")
    config.addModule("network")

    config.addLib("-ltupifwgui")
    config.addLib("-ltupifwcore")
    # config.addLib("-ltupifwsound")
    
    config.addDefine('VERSION=\\\\\"0.2\\\\\"')
    config.addDefine('CODE_NAME=\\\\\"Ajuba\\\\\"')
    config.addDefine('REVISION=\\\\\"git02\\\\\"')

    if conf.hasArgument?("install-headers")
       config.addDefine("ADD_HEADERS");
    end

    Info.info << "Debug support... "

    if debug == 1 
       config.addDefine("K_DEBUG")
       print "[ \033[92mON\033[0m ]\n"
    else
       config.addDefine("K_NODEBUG")
       config.addOption("silent")
       print "[ \033[91mOFF\033[0m ]\n"
    end
    
    unix = config.addScope("unix")
    unix.addVariable("MOC_DIR", ".moc")
    unix.addVariable("UI_DIR", ".ui")
    unix.addVariable("OBJECTS_DIR", ".obj")

    # The file tupiglobal.pri contains all the global variables for the compilation process        
    config.save("tupiglobal.pri")
    conf.createMakefiles
    
rescue => err
    Info.error << "Configure failed. error was: #{err.message}\n"
    if $DEBUG
        puts err.backtrace
    end
end

