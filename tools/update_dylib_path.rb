#!/usr/bin/ruby

library = ARGV[0]
libs = %x[otool -L #{library}|awk '{print $1}'|grep -v '/usr/lib'|grep -v '/System'|grep \"/lib\"|grep -v '@executable']
puts "---------------------------------------------"
puts "Libs for: #{library}"
libs.each do |line|
  parsed = line.chop().gsub(/^\/lib/,"/opt/local/lib")
  cmd = "install_name_tool -change #{line.chop()} #{parsed} #{library}"
  puts cmd
  system("#{cmd}")
end
