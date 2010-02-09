require 'ruby_parser'

class Semantics
  
  def initialize
    @meaning = []
  end
  
  def interpret(syntax)
    return @meaning
  end
  
  def add(op, *args)
    @meaning << [op, *args]
  end  
end


class AloxSemantics < Semantics
  
  def initialize()
    super
    @scope      = []
    @constants  = {}
  end

  def interpret(syntax)
    return @meaning
  end
end



class Orurira

  def initialize()
    @parser = RubyParser.new  
  end
  
  def compile(text)
    parsetree = @parser.parse(text)    
    return parsetree
  end
  
  def compile_file(file)
    text      = file.read
    parsetree = @parser.parse(text)
    return parsetree
  end


end






if __FILE__ == $0
  o     = Orurira.new
  alsem = AloxSemantics.new
  res   = o.compile_file(DATA)
  res2  = alsem.interpret(res)
  p res
  p res2 
end

__END__
record RSized do
  size :size  
  def size()
    return @size
  end  
end

This diagram illustrates the instatances, class hierarchy and association of 
eigenclasses after executing the following Ruby program: 

class Foo; end
class Bar < Foo; end
foo1 = Foo.new
bar1 = Bar.new
bar2 = Bar.new
class << bar2; end

record RString < RSized do
  inline_array :buffer, :Byte, :size
end

namespace Linux do
  STDOUT     = 1
  STDIN      = 2
  STDERR     = 3
  SYS_EXIT   = 1
  SYS_FORK   = 2
  SYS_READ   = 3
  SYS_WRITE  = 4
  SYS_OPEN   = 5
  SYS_CLOSE  = 6 
  SYS_BRK    = 45

  def exit(retval = 0)
    syscall(SYS_CLOSE, retval)
  end

  def write(fd = STDOUT, str="", len = 0)
    syscall(SYS_WRITE, fd, str, len)
  end
   
end

