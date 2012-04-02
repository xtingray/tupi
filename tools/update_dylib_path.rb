#!/usr/bin/ruby
Dir.new(Dir.pwd).entries.each do |n| 
	if File.file?(n) and n.end_with?(".dylib")
        libs = %x[otool -L #{n}|awk '{print $1}'|grep -v '/usr/lib'|grep -v '/System'|grep "/lib"|grep -v '@executable']
		puts "---------------------------------------------"
		puts "Libs for: #{n}"
		libs.each do |line|
            parsed = line.chop().gsub(/^\/opt\/local\/lib/,"@executable_path/../Frameworks")
            parsed = parsed.gsub(/^\/lib/,"@executable_path/../Frameworks")
            cmd = "/usr/bin/install_name_tool -change #{line.chop()} #{parsed} #{File.expand_path(File.dirname(__FILE__))}/#{n}"
            puts cmd
            system("#{cmd}")
        end
	end
end
