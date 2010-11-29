require 'qonf/info'
require 'qonf/extensions'

module RQonf

class QMake

    $found = ""

    def initialize
        if not findQMake("4.0.0",false)
            raise QonfException.new("Can't find valid qmake - qt4")
        end
        
        @make = "make"
    end

    # This method check if the current version of Qt is valid for Tupi compilation    
    def findQMake(minqtversion, verbose)
        qtversion = ""
        paths = [ "qmake", "qmake-qt4", "qmake4" ]
        minver = minqtversion.split(".")
        valid = true

        paths.each { |path|
            begin
                version = []
                sites = []
                distance = 0
                IO.popen("whereis #{path}") { |result|
                          sites = result.readlines.join("").split(":")
                          word = sites[1].chop
                          distance = word.length
                }

                if distance > 0
                    IO.popen("#{path} -query QT_VERSION") { |prc|
                        found = prc.readlines.join("")
                        version = found.split(".")
                        if (found.length != 0)
                            qtversion = found.chop
                        end
                    }
                    next if $? != 0

                    version.size.times { |i|
                        if i = 0
                           if version[i] < minver[i]
                              valid = false 
                              break
                           end
                        end

                        if i = 1
                           if version[i] < minver[i]
                              valid = false
                              break
                           else
                              if version[i] > minver[i]
                                 break
                              end
                           end
                        end

                        if i = 2
                           if version[i] < minver[i]
                              valid = false
                              break
                           end
                        end
                    }
                
                    if valid  
                        @path = path
                        break    
                    end
                else
                    valid = false
                    return valid
                end
            end
        }

        if verbose 
            print "(Found: #{qtversion}) "
        end

        return valid
    end

    def query(var)
        output = `#{@path} -query #{var}`
        output
    end
    
    def run(args = "", recur = false)
        options = ""
        if recur
            options += "-r"
        end
        
        output = `#{@path} #{args} #{options} `
        
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


