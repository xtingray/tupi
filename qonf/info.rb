
$endl = "\n"

class Info
    def self.info
        $stdout << "\033[92m *\033[0m "
    end
    
    def self.warn
        $stdout << "\033[93m *\033[0m "
    end
    
    def self.error
        $stdout << "\033[91m *\033[0m "
    end
    
    def self.fatal
        $stdout << "\033[91m * FATAL:\033[0m "
    end
end

