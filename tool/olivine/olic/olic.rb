require 'dot'
require 'treetop'

# require 'ghost_wheel'

=begin
# Better debugging error messages
module Treetop
   module Runtime
     class TerminalParseFailure
       attr_reader :index

       def initialize(index, expected_string)
         @index = index
         @caller = caller[0..-2] # Omit mainline
         @expected_string = expected_string
       end

       def expected_string
         "#{@expected_string} from #{@caller.map{|s| 
         s.sub(/\A.*: ([0-9]+):in `([^']*)'.*/,'\2:\1')}} \n\t"
       end

       def to_s
         "String matching #{expected_string} expected."
       end
     end
   end
end 
=end


# Monkey patch base syntax note to be non-boring 
class Treetop::Runtime::SyntaxNode
  def boring?
    return false
  end
end


module Olivine
  
  # Node mix-ins for the TreetopParser Grammar
  module Grammar
    
    module Modules
      # Boring (uninteresting) nodes
      module Boring 
        def boring?
          return true
        end 
      end  
    end
    

    module Node
      class Base < Treetop::Runtime::SyntaxNode 
        def boring?
          return false
        end 
      end

      # Boring (uninteresting) nodes
      class Boring < Treetop::Runtime::SyntaxNode
        include Modules::Boring
      end
      
    end
  end   
  
  # Lines of a compilable unit.
  class Lines
    attr_reader     :real
    attr_reader     :logical
    attr_reader     :mapping
    
    def initialize(text, split_re = /\n\r|\n|\r/)
      @real      = text.split(split_re)
      # Split into lines
      @mapping   = {} 
      @logical   = []
      @indents   = [] # Indent levels of logical lines 
    end
    
    def join_escaped(escape_str = '\\')
      escape = false 
      for line in @real do      
        # escape on the end of the line-> set fold with next flag
        if line.index(escape_str) == (line.size - 1)
          logical = line[0, line.size - 1] 
          escapen = true
        else
          logical = line.dup
          escapen = false
        end

        if escape # flag is set from previous loop
          @logical.last <<  logical # Concatenate if escape on previous line
        else 
          @logical <<  logical # New line
        end
        escape = escapen
      end 
    end
    
    def add_mapping(lindex, rindex, rsize)
      @mapping[lindex] = [] unless @mapping[lindex] 
      @mapping[lindex] << [ rindex,  rsize ]
      return @mapping[lindex]
    end 
    
  end

  class Janitor
    def process(text)
      lines = Lines.new(text)
      # Split into lines
      # Merge lines ending on a single backslash
      lines.join_escaped
      
      # Add end of block statements depending on the indents, 
      # if they are missing 
      return lines  
    end
  end

  class Compiler
  
  end
  
=begin  
  GWParser    = GhostWheel.build_parser %q{      
    line_end      = "\r\n "| "\n" | \r
    block_comment = '/*' /(\*\/)+/ '*/'
    line_comment  = '#' !line_end line_end  
    comment       = line_comment | block_comment
    message       = /[a-z][a-zA-Z0-9\?\!]+/
    chain_sep     = /\s+/ | comment
    chain_end     = '.' | line_end
    chain         = chain_sep? message? (chain_sep message)* chain_sep? chain_end 
    parse        := chain+ EOF 
  }
=end  
end




if __FILE__ == $0

  Treetop.load 'grammar.treetop'
  
  TESTFILE  = 'try1.oli'
  input     = File.read(TESTFILE)
#  input     = DATA.read 
  janitor   = Olivine::Janitor.new
# p Olivine::GWParser.methods.sort - Object.methods
#  gwres     = Olivine::Parser.parse(input)
#  p gwres
  
  result    = janitor.process(input)
  parser    = Olivine::GrammarParser.new
  pres      = parser.parse(input) 
  # p result.logical
  if pres
    puts "OK"
    # p pres
    p pres.methods.sort - Object.methods
    # splines: true, overlap: false, 
    graph     = Dot.graph(fontsize: 10, rankdir: "LR")
    graph.nodeattr = { fontsize: 10, width:0, height:0, shape: :box, style: :rounded}
    graph.from_treetop(pres)
    puts "Please stand by while dot is rendering the graph..."
    p pres
    graph.display
  else  
    puts parser.failure_line
    puts parser.failure_column
    puts parser.failure_reason 
  end
  


end

__END__

foo bar baz

/*
do 
  "hello" 
end  
*/
# yes, a space delimited list !
# Space delimited list of messages (a = 10) 
# a.b = c.d 
# ( (a.b) = (c.d))
# a.b = c.f a, b, c
# ( (a,b) = (c.f) (a b c))
# list 

/*

do
  baz
  do
    bar
  end
end    

do 
  bar.baz.quux 
  baz 
  ber
end


{ 
  foo 
  bar
}
*/

# A one line comment.
# ( list ( sublist, sub2, sub3 ) ) 

/*
  Multiline comment.
*/   

# Final comment
