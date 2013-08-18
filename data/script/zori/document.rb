#
# A document is a large text that has many styles attached to it
# It is useful for text implementing a editor area.
#

require 'forwardable'

module Zori
  class Document
    NEWLINE = /\r\n|\n|\r/
    # A line is a single logical line in a document. If it doesn't fit in 
    # the width available for the edit view, it may be expanded over 
    # several on-screen physical lines
    class Line 
      attr_reader :text
      
      def initialize(text)
        @text  = text        
      end
      
      def self.from_text(text)
        return [] unless text
        return text.split(NEWLINE).map { |line| self.new(line) }
      end
      
      def col_ok?(col)
        return false if col < 0
        return col <= @text.size
      end      
      
      def_delegators :@text, :insert, :[], :[]=, :size
      
    end
    
    
    # Lines of text
    class Lines
      def initialize(text) 
        @lines = Line.from_text(text)        
      end
      
      def row_ok?(row)
        return false if row < 0
        return row <= @lines.size
      end
      
      def [](row)
        return nil unless row_ok?(row)
        @lines[row] ||= Line.new('')
        return @lines[row]
      end
      
      def insert(row, col, text)
        line = self[row]
        line.insert(col, text)
      end
      
      def delete(row, col, amount)
        line = self[row]
        line[col, amount] = ''
      end
      
      def append(row, text)
        
      end
      
    end
    
    # A cursor is a marker in the document. It can be an insert point, 
    # or the beginning or end of a selection
    class Cursor
      attr_reader :index      
      attr_reader :row
      attr_reader :col
      
      
      
      def initialize(text, ind)
         @text  = text
         @index = ind
         update()
      end
      
      def max
        return @text.size
      end
      
      def update()
        subtext = @text[0,@index]
        @row    = subtext.scan(NEWLINE).count
        last_nl = subtext.rindex(NEWLINE) 
        p subtext
        p last_nl
        last_nl ||= 0
        @col    =  last_nl - @index + 1
      end
      
      # Moves the cursor linearly through the text
      def move(by=1)
        @index += by
        @index  = self.max  if @index > self.max
        @index  = 0         if @index < 0
        update()
      end
      
      def up()
        subtext = @text[0,@index]
        last_nl = subtext.rindex(NEWLINE)
        return false unless last_nl # do nothing if on first line 
        
      end
      
      def down()
        next_nl = @text[@index, self.max].index(NEWLINE)
        next_nl 

      end
      
      def left()
        self.move(-1)
      end
      
      def right()
        self.move(1)
      end
      
    end
    
    # A selection is an area of the text that is selected for further operations
    
    # A line is a single line of text. Because text is edited in a line by line 
    # fashion, it  makes sense to pre-split the text into lines... but it makes 
    # inserting and modifying a bit harder. On the other hand, it makes cursor 
    # implementation a lot simpler and quicker.
    
    attr_reader :cursor
    
    def initialize(text, style)
      @text     = text
      @style    = style # Default style
      @styles   = {}    # Other styles
      @nodes    = []    # Nodes
      # self.add_node(@text, 0, @text.size, @style)
      # Start with a single node
      @cursor   = Cursor.new(@text, 0)
      @selection= nil 
    end
    
    def each_node(&block)
      @nodes.each(&block)
    end

    
    def each_line(&block)
      @text.each_line(&block)
    end
    
  end # class Document
end # module Zori
  
  
  
if $0 == __FILE__ # builtin tests
  doc = Zori::Document.new("Foo\nBar\nQuux", nil)
  p doc.cursor
  doc.cursor.move(4)
  p doc.cursor
  
end