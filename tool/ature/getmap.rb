#!/usr/local/bin/ruby1.9
map       = []
index     = nil
in_field  = false 
fin = File.open('ature.bas', 'rt')  
while line = fin.gets

  if line =~ /\Ar[0-9][0-9]/
    index = line.tr("r \n", '').to_i
  end
  
  if line =~ /\Aend/
    index    = nil if in_field
    in_field = false
  end  
  
  if in_field && index
    map[index] << line.tr("\n ", '')
  end

  if index && line =~ /  playfield:/ 
    in_field = true
    field = ''
    map[index] = []
  end
  
end
fin.close


fout = $stdout  # fopen('map.txt', 'wt')
fout.puts('+' + ('...............................+' * 8))
for y in 0..7
  for r in 0..10
    sep =  ((r+1) % 11) == 0 ? '+' : '|'
    fout.printf("%s", sep)
    for x in 0..7
      i = x + y * 8
      if map[i]
         
        fout.printf("%s", map[i][r])
      else
        raise "Map #{i} is missing!"
      end  
    end
    fout.puts
  end
end

fout.close
