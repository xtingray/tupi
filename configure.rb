#!/usr/bin/ruby

# TODO: This script must detect if every command line given is valid 
#       Currently, it just try to check if some of them are included or not

require './qonf/configure'
require './qonf/info'
require './qonf/defaults'

begin
    conf = RQonf::Configure.new(ARGV)

    if conf.hasArgument?("help") or conf.hasArgument?("h")
       puts <<_EOH_
Use: ./configure [options]
  options:
  --help:               Show this message
  --prefix=[path]:      Sets installation path [/usr/local]
  --bindir=[path]:      Set binaries path [/usr/local/bin]
  --libdir=[path]:      Set library path [/usr/local/lib]
  --includedir=[path]:  Set include path [/usr/local/include]
  --sharedir=[path]:    Set data path [/usr/local/share]
  --with-ffmpeg=[path]: Set ffmpeg installation path [/usr]
  --with-aspell=[path]: Set apell installation path [/usr]
  --with-debug:         Enable debug
  --with-qtdir=[path]:  Set Qt directory [i.e. /usr/local/qt]
  --debian-build:       Option exclusive for Debian maintainer
_EOH_
        exit 0
    end

    distro = ""
    if FileTest.exists?("/etc/lsb-release")
       conf.load_properties("/etc/lsb-release")
       if conf.hasProperty?("DISTRIB_CODENAME")
          distro = conf.propertyValue("DISTRIB_CODENAME")
       end
    end

    config = RQonf::Config.new

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

    if conf.hasArgument?("with-ffmpeg")
       ffmpegLib = conf.argumentValue("with-ffmpeg") + "/lib"
       ffmpegInclude = conf.argumentValue("with-ffmpeg") + "/include"
       config.addLib("-L" + ffmpegLib)
       config.addIncludePath(ffmpegInclude)
    end

    if conf.hasArgument?("with-aspell")
       aspellLib = conf.argumentValue("with-spell") + "/lib"
       aspellInclude = conf.argumentValue("with-aspell") + "/include"
       config.addLib("-L" + aspellLib)
       config.addIncludePath(aspellInclude)
    end

    debug = 0
    if conf.hasArgument?("with-debug")
        debug = 1
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
    config.addLib("-ltupifwsound")
    config.addLib("-lz")
    
    # config.addLib("-L#{RQonf::CONFIG["libdir"]}")
    # config.addIncludePath(RQonf::CONFIG["includepath"])
    
    config.addDefine('VERSION=\\\\\"0.1\\\\\"')
    config.addDefine('CODE_NAME=\\\\\"Gemo\\\\\"')
    config.addDefine('REVISION=\\\\\"git12\\\\\"')

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

    if RUBY_PLATFORM.downcase.include?("darwin")
       config.addLib("-L/sw/lib")
       config.addIncludePath("/sw/include")
    end

     # The file tupiglobal.pri contains all the global variables for the compilation process        
    config.save("tupiglobal.pri")
    conf.createMakefiles
    
rescue => err
    Info.error << "Configure failed. error was: #{err.message}\n"
    if $DEBUG
        puts err.backtrace
    end
end

