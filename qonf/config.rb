
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
