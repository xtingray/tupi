#!/usr/bin/ruby

library = ARGV[0]
oldpath  = ARGV[1]
newpath = ARGV[2]

libs = %x[otool -L #{library} |awk '{print $1}'|grep "#{oldpath}"]

puts "---------------------------------------------"
puts "Libs for: #{library}"
libs.each do |line|
  parsed = line.chop().gsub(/#{oldpath}/, newpath)
  cmd = "install_name_tool -change #{line.chop()} #{parsed} #{library}"
  puts cmd
  system("#{cmd}")
end
