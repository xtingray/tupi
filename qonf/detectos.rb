
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

