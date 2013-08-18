class Stascril
  
  def initialize
    @dict     = {}
    @defines  = []
    @stack    = []
    @strings  = []
    @program  = []
    @retstack = []
    @mode     = [ :normal ]
    @word     = ''
    @defines[0] = @program
    @ip         = 0
    @dp         = 0
  end
  
  def add_word
    lookup = @dict[@word.to_s.dup] 
    if lookup 
      @program << [:call, lookup]
    else 
      @program << [:string, @word.to_s.dup]
    end
    @word = ''
  end
  
  def parse_char_normal(ch)
    case ch
      when ' ', "\t", "\r", "\n"
        add_word
      when "`"
        @mode.push  :backtick
      when '"'
        @mode.push :string
      when '\\'
        @mode.push :escape
    else
        @word << ch
    end
    return nil
  end
  
  def parse_char_string(ch)
    case ch
      when '\\'
        @mode.push :escape
      when '"'
        add_word
        @mode.pop
    else
        @word << ch
    end
    return nil
  end
  
  def parse_char_backtick(ch)
    case  ch
      when '\\'
        @mode.push :escape
      when '`'
        add_word
        @mode.pop
    else
        @word << ch
    end
    return nil
  end
  
  def parse_char_escape(ch)
    lookup = { 'n' => "\n", 'r' => "\r", 't' => "\t" } 
    aid = lookup[ch]
    aid = ch if(!aid)
    @word << ch
    @mode.pop
    return nil
  end

  def parse_char(ch)
    case @mode.last
      when :normal
        return parse_char_normal(ch)
      when :string
        return parse_char_string(ch)
      when :backtick
        return parse_char_backtick(ch)
      when :escape
        return parse_char_escape(ch)
      else
        raise "Unknown mode in parser: >#{@mode}<!"
    end
    return res
  end
  
  def parse_file(file) 
    file.each_char do |ch|
      parse_char(ch)
    end
  end
  
  def parse_string(str)
    str.each_char do |ch|
      parse_char(ch)
    end
  end
  
  def execute_once()    
    @ip  += 1
    cell = @defines[@dp][@ip]
    p @ip, @dp, cell
    action, data = *cell
    if action == :call
      if data.respond_to? (:call) 
        data.call(self)
      else
        @retstack.push @ip
        @retstack.push @dp
        @dp = data
      end
    else 
      @stack.push(data)
    end
  end
  
  def execute
    while @ip < @defines[@dp].size
      execute_once
    end
  end
  
  def builtin(name, &body) 
    @defines << body
    @dict[name.to_s] = @defines.size
  end
  
  
end
  
  
p = Stascril.new
p.builtin("puts") do |o| 
  data = o.stack.pop
  puts(data)
end

p.parse_string('"Hello World" puts ')
p.execute








