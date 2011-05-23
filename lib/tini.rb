class Tini
  def initialize
    @line = 0 
    # line currently parsing
  end
  
  
  
  
    
  STRING_RE1 = %r{\A("([^"\\]|\\.)*")}
  STRING_RE2 = %r{\A('([^'\\]|\\.)*')}
  WORD_RE    = %r{\A([^ \t]+)}
  BLANK_RE   = %r{\A([ \t]+)}
  
  RE_LIST    = [ STRING_RE1, STRING_RE2, WORD_RE, BLANK_RE ]
  
  def match_re(line)
    for re in RE_LIST
      aid   = line.scan(re)
      part  = aid.first
      if part 
        p "part:", part, aid if re == STRING_RE1
        return part.first
      end    
    end 
    return nil
  end
  
  def split_line(line)
    parts = []
    until line.empty?
      part = match_re(line)
      raise 'Unexpected data!' if(!part || part.size < 1) 
      line = line[part.size, line.size]
      # don't store blanks
      unless part.empty? || part =~ BLANK_RE
        # remove quotes
        part.gsub!(/\A"|"\Z|\A'\Z"/, '')
        parts << part
      end  
    end
    return parts
  end
  
  def parse_line(line) 
    # strip whitespace
    clean = line.strip
    comm  = clean.index('#') 
    # strip comments
    clean = clean[0, comm] if comm
    return nil if clean.empty? 
    # it was a bank line or a comment if the clean line is empty.
    p clean
    parts = split_line(clean)    
    p "parts", parts 
    
    
  end
  
  
  def parse(text)
    lines = text.split(/(\r\n|\r|\n)+/)
    p lines
    for line in lines do  
      parse_line(line)
    end  
    
    return nil
  end
  
  
  def self.parse(text)
    return self.new.parse(text)
  end
  
end





__END__





map "world" 45 45 do
  layer 1 7 7 do
    10 20 30 40
  end layer
end map


