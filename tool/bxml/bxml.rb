



class Node
  TAG   = 1
  VALUE = 2
  attr_accessor :name
  attr_accessor :value
  attr_accessor :parent
  attr_accessor :child
  attr_accessor :next
  attr_accessor :prev
  attr_accessor :kind
end


FNAME = '/home/bjorn/eruta/tool/bxml/test.xml'

stack = []

buf   = ""
text  = ""
tag   = ""

cdata = false
skipgt= false 
intag = false

File.open(FNAME, 'rt') do | fin | 
  ch = fin.getc
  until fin.eof?
    buf << ch
    if ch == '>' # possible end of tag 
      # there is a cdata section
      if !cdata && buf['<![CDATA[']
        cdata = true
      end
      skipgt = false
      if cdata
        skipgt = true unless buf[']]>'] # end of cdata reached 
      end
      # parse this partial tag unless in a > to skip (i.e. in cdata) 
      unless skipgt
        
        text, tag = buf.split('<', 2)
        puts "text:", text, "tag", tag
        
        buf   = ''
        cdata = false
      end
    end  
    ch = fin.getc
  end
end









