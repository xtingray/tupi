#!/usr/bin/ruby

library = ARGV[0]
command = "otool -L #{library}|awk '{print $1}'|grep -v '/usr/lib'|grep -v '/System'|grep \"/lib\"|grep -v '@executable'"

libs = %x[#{command}]
puts "---------------------------------------------"
puts "Libs for: #{library}"
libs.each do |line|
  parsed = line.chop().gsub(/^\/opt\/local\/lib/,"@executable_path/../Frameworks").gsub(/^\/lib/,"@executable_path/../Frameworks")
  cmd = "install_name_tool -change #{line.chop()} #{parsed} #{library}"
  puts cmd
  system("#{cmd}")
end
