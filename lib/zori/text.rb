# Text handling for Zori
# The Text class contains text that can stretch over several lines, 
# and needs to be displayed as such. Furthermore, only a few lines of it my be  
# displayed when a vertical range window is set.


module Zori
  class Text
  
    # Line a helper class that models a virtual line of text
    # (One that fits the viewport) 
    class Line 
      extend Forwardable
      attr_reader :w
      attr_reader :h
      def initialize(style, text=nil)
        @style  = style
        @h      = @style.font.lineskip
        @w      = 0
        @line   = ""
        if text
          self << text
        end   
      end
      
      def <<(text)
        @line << text
        w, h   = @style.font.size_text(text)
        @w    += w 
        @h     = h if h > @h
      end
      
      def draw(target, x, y, style = nil)
        style ||= @style
        target.text(x, y, @line, @style.font, 
                    style.colors.text, style.colors.background)
      end
      
      
      
      def_delegators :@line, :empty?, :size, :length, :to_s
      
      def self.split_multi(style, text, max_wide)
        splitby         = /([ \n\t\r])/ 
        # between brackets because we need the separators (spaces) also!
        words           = text.split(splitby)
        result          = []
        line            = Zori::Text::Line.new(style)           
        for word in words do
          word_wide     = style.font.width_of(word)          
          if line.w + word_wide > max_wide 
            # Line width plus word width is too wide.
            # Insert the line, and put the word in the next line  
            result     << line 
            line        = Zori::Text::Line.new(style, word)
          else # Not too wide, add child to the line
            line       << word
          end
        end
        # Don't forget to add the final line if not empty! 
        result          << line unless line.empty?
        return result
      end
  
      def self.split_multi_paragraph(style, text, maxwide) 
        result      = []
        paragraphs  = text.split(/\n/);
        # Split in paragraphs by newline characters.
        paragraphs.each do | paragraph |
          lines     = self.split_multi(style, paragraph, maxwide);
          result    += lines
        end
        return result
      end
    end # Class line 
    
    
    def self.split_multi(font, text, maxwide) 
      result                = []
      line                  = ""
      split                 = /[ \n\t\r]/
      words                 = text.split(split)
      linewide              = 0;
      spacewide, spacehigh  = *font.size_text(' ')
      linestart             = true
      wordwide              = 0;
      lastwordwide          = 0;
      stop                  = words.size  
      for word in words do
        wordwide, wordhigh  = *font.size_text(word);
        linewide, linehigh  = *font.size_text(line);
        if wordwide >= maxwide then
        # The word is too wide to ever fit on one line. Put it in the line anyway.
          line += " " unless linestart
          line += word;  
          result.push(line);
          line = "" 
        elsif (wordwide + spacewide + linewide) > maxwide then
        # The word doesn't fit on this line. Move on to the beginning of the next line.
          result.push(line);
          line     = word;        
          linestart   = false;
        else 
          line += " " unless linestart
          line += word;  
          linestart = false;
        end
      end
      result.push(line) unless line.empty?;
      # Do not forget to add the last line, unless if it is empty.
      result.shift while result.first.empty?
      result.pop   while result.last.empty?
      # Hack to fix strange empty lines that are inserted inconsistently at the top 
      # or bottom for some reason. 
      return result;
    end
  
    def self.split_multi_paragraph(font, text, maxwide) 
      result = []
      paragraphs = text.split(/\n/);
      # Split in paragraphs by newline characters.
      paragraphs.each do | paragraph |
        lines     = self.split_multi(font, paragraph, maxwide);
        result    += lines
      end
      return result;
    end
    
    attr_reader :window

    def initialize(text, maxwide, style) 
      @style      = style
      @font       = style.font
      @lines      = Zori::Text::Line.split_multi_paragraph(@style, text, maxwide)  
      @line_count = @lines.size
      @wide       = maxwide
      @line_high  = @font.lineskip
      @high       = @line_count * @line_high
      @textcolor  = style.colors.text
      @backcolor  = style.colors.background
      @window     = 0...@lines.size
    end

    def draw(target, x, y)
      dx        = x
      dy        = y      
      # Draw the text line by line.   
      for index in @window do
        line  = @lines[index];
        unless line.nil? or line.empty?
          draw_line(target, dx, dy, line)
          # line.draw(target, dx, dy)
          # @font.draw(target, dx, dy, line.to_s, @textcolor, @backcolor)
        end  
        # Draw one line if we have it, otherwise skip a line.         
        dy      += @line_high;
      end
    end
    
    
    def draw_line(target, x, y, line)
      case @style.alignment
        when :right
          draw_line_right(target, x ,y, line)
        when :middle
          draw_line_middle(target, x ,y, line)
        when :left
          draw_line_left(target, x ,y, line)
        else
          draw_line_left(target, x ,y, line)
      end
    end
    
    def draw_line_left(target, x, y, line)
      dx  = x + @style.padding
      line.draw(target, x, y)      
    end
    
    def draw_line_middle(target, x, y, line)
      middle  = x +  @wide / 2 + @style.padding
      dx      = middle - (line.w / 2)
      line.draw(target, dx, y)      
    end
    
    def draw_line_right(target, x, y, line)
      right   = x     + @wide
      dx      = right - line.w - @style.padding
      line.draw(target, dx, y)
    end

    
    def draw_middle(target, x, middle)
      # y is a middle position
      dy        = u
      for index in @window do
        line  = @lines[index];
        unless line.nil? or line.empty?
          line.draw(target, dx, dy)
        end  
        # Draw one line if we have it, otherwise skip a line.         
        dy      += @line_high;
      end
    end

    def wide
      linew    = 0
      @lines.each do |l| 
        linew  = l.w if l.w > linew
      end
      return linew
    end  

    def high
      return (@lines.size * @line_high)
    end  
    
    def set_window(start, count)      
      start   = 0               if start  < 0
      start   = @lines.size - 1 if start  > @lines.size - 1
      finish  = start + count
      finish  = 0               if finish < 0
      finish  = @lines.size - 1 if finish > @lines.size - 1
      @window = start...(start + count)
    end
    
    def scroll(direction)
      set_window(@window.first + direction, @window.count)
    end

    def self.best_width(text, style)
      return 0 unless text
      raw_lines = text.split(/\n/)
      wide      = 0
      raw_lines.each do |l| 
        w, h = style.font.size_text(l)
        wide = w if w > wide
      end
      return wide 
    end
  
    def self.make_text(text, maxwide, style)
      prepared = Zori::Text.new(text, maxwide, style)
      return prepared 
    end 
  
    def self.make_draw_text(target, x, y, text, maxwide, style)
      prepared = self.make_text(text, maxwide, style)
      prepared.draw(target, x, y)
      return prepared
    end
  
  end # class Text
end # module Zori 
  