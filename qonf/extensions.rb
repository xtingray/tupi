
module Kernel
    def close_stdout
        @stdout = $stdout.clone
        $stdout.close
    end
    
    def open_stdout
        if @stdout
            $stdout = @stdout
        end
    end
end

