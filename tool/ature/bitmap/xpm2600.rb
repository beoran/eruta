#!/usr/local/bin/ruby1.9
# encoding: utf-8
# xmp2600 converts 2 color XPM files to 
# sprites for batari basic for the 2600

begin 

finname   = ARGV[0]
foutname  = ARGV[1]
if(!finname) 
  fin = $stdin
else 
  fin = File.open(finname, "rt")
end

if(!foutname) 
  fout = $stdout
else 
  fout = File.open(foutname, "wt")
end
 
raise "Could not open input file"   unless fin
raise "Could not open output file"  unless fout
 
bitmap = []

header = fin.readline
raise "Not an XPM file" if header !~ /XPM/ 
header = fin.readline
# skip header 2
header = fin.readline
parts = header.tr("\"\n,",'').split(' ')
w = parts[0].to_i
h = parts[1].to_i
c = parts[2].to_i
d = parts[3].to_i
black = nil
white = nil

c.times do
  color = fin.readline
  color.tr!("\",\n", '')
  if color =~ /#FFFFFF/
    white = color[0]
  end 
  if color =~ /#000000/
    black = color[0]
  end 
end

h.times do
  line = fin.readline
  line.tr!("\",\n};", '')
  line.tr!(black, '0')
  line.tr!(white, '1')
  bitmap << line   
end

fout.puts "  rem '#{finname}" if finname
fout.puts '  player1:'
i = bitmap.size
while i > 0 
  i = i - 1
  line = bitmap[i]
  fout.puts "  %#{line}"
end
fout.puts 'end'

rescue Exception 
  $stderr.puts $!.inspect
  $stderr.puts $!.backtrace.inspect
  $stderr.puts ARGV.inspect
end
 
