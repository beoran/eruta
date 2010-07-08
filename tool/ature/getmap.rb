#!/usr/local/bin/ruby1.9

#   const black=0
#   const gray=6
#   const white=14
#   const yellow=30
#   
#   const brown=32
#   const orange=32
#   const blue=144
#   const skyblue=174
#   const green=192
#   const darkgreen=191
#   const riverblue=170  
#   rem ' XXX: there is a bug in the ompiler (or my patches to it) 
#   rem ' some of my consants like the one below are translated in stead 
#   rem ' like a dim statement: eg  const turfy=218 -> dim   turfy=218
#   rem ' a workaround seem to be to reorder the constants a bit
#   rem ' or to remove a few of them, but this only bugs out other constants :p
#   rem ' so I replaced some constants by their values, and use the cpp 
#   rem ' preprocessor in stead.    
#   define SEABLUE 144
#   const seablue = 144
#   const red   = 64
#   const pink  = 78
#   const skin  = 254
#   const sand  = 230
#   const turfy = 218

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
