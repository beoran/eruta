require 'curses'

include Curses

# Wrapper around Curses and it's windows 
class Console
  include Curses
  
  # Opens a new Console window 
  def initialize(x, y, w, h)
    cwin = Window.new(h, w, y, x)
    initialize_from_curses(cwin)
  end
  
  def initialize_from_curses(cwin)
    @window   = cwin
    @colorok  = Curses.has_colors?
    @color    = COLOR_WHITE
    @reverse  = false
    @bright   = false
  end
  
  
  # Color constant numbers
  BLACK    = 1
  GREEN    = 2
  RED      = 3
  CYAN     = 4
  WHITE    = 5
  MAGENTA  = 6
  BLUE     = 7
  YELLOW   = 8
   
  # Colors, same as the 16 official CSS colors
  COLORS = [ :aqua, :black, :blue, :fuchsia, 
            :gray, :green, :lime, :maroon, 
            :navy, :olive, :purple, :red, 
            :silver, :teal, :white, :yellow] 
  
  # Looks up color symbol to real color pair
  COLOR_LOOKUP =  { :aqua   => CYAN   , :black    => BLACK,
                    :blue   => BLUE   , :fuchsia  => MAGENTA,
                    :gray   => BLACK  , :green    => GREEN, 
                    :lime   => GREEN  , :maroon   => RED,
                    :navy   => BLUE   , :olive    => YELLOW,
                    :purple => MAGENTA, :red      => RED,
                    :silver => WHITE  , :teal     => CYAN,
                    :white => WHITE   , :yellow   => YELLOW }
                    
  # Contains the brightness of all colors
  BRIGHT_LOOKUP = { :aqua   => false  , :black    => false,
                    :blue   => true   , :fuchsia  => true,
                    :gray   => true   , :green    => false, 
                    :lime   => true   , :maroon   => false,
                    :navy   => false  , :olive    => false,
                    :purple => false  , :red      => true,
                    :silver => false  , :teal     => false,
                    :white  => true   , :yellow   => true }
  
  # Sets the current console color
  def color=(colsym)
    col      = COLOR_LOOKUP[colsym.to_sym] || colsym.to_i
    bright   = BRIGHT_LOOKUP[colsym]
    if (col != @color) || (bright != @bright)
      @color = col
      if @colorok
        self.bright = bright
        # if @window.respond_to? :color_set
        # @window.color_set(@color)
        # Set colors via attron, since thatworks better on 
        # Windows and maybe other platforms that don't have colot_set
        pair = Curses.color_pair(@color)
        @window.attron(pair)
      end
    end
  end
  
  def bright=(b)
    if b != @bright
      @bright = b
      if @bright 
        # @window.attron(A_)
        @window.attron(A_BOLD)
      else
        # @window.attroff(A_)
        @window.attroff(A_BOLD)
      end  
    end
  end
  
  def reverse=(r)
    if r != @reverse
      @reverse = r
      if @reverse 
        @window.attron(A_REVERSE)
      else
        @window.attroff(A_REVERSE)
      end  
    end
  end
  
  
  
  def self.new_from_curses(cwin)
    window = self.allocate
    window.initialize_from_curses(cwin)
    return window
  end
  
  def self.key2sym(key)
    unless @key2sym
      @key2sym          = {}
      Curses.constants.select{|k| k =~ /\AKEY_/ }.each do |k|
        val             = Curses.const_get(k)
        sym             = k.to_s.gsub(/\AKEY_/,'').to_sym
        @key2sym[val]   = sym
      end
    end
    return @key2sym[key] || key
  end
  
  # Opense the main screen and returns it
  def self.init_screen
    @key2sym      = nil 
    cscreen       = Curses.init_screen
    if Curses.has_colors?    
      Curses.start_color
      background  = COLOR_BLACK
      Curses.init_pair(BLACK  , COLOR_BLACK   , background)
      Curses.init_pair(GREEN  , COLOR_GREEN   , background)
      Curses.init_pair(RED    , COLOR_RED     , background)
      Curses.init_pair(CYAN   , COLOR_CYAN    , background)
      Curses.init_pair(WHITE  , COLOR_WHITE   , background)
      Curses.init_pair(MAGENTA, COLOR_MAGENTA , background)
      Curses.init_pair(BLUE   , COLOR_BLUE    , background)
      Curses.init_pair(YELLOW , COLOR_YELLOW  , background)
    end
    Curses.raw
    Curses.noecho
    Curses.curs_set(0)
    cscreen.keypad(true)
    # cscreen.timeout = 1000 timeout for nonblockig IO
    return self.new_from_curses(cscreen)
  end
  
  def self.close_screen
    Curses.close_screen
  end
  
  def refresh
    @window.refresh
  end
  
  def w
    return @window.maxx
  end
  
  def h
    return @window.maxy
  end
  
  def <<(str)
    @window << str 
  end
  
  def xy(x, y)
    @window.setpos(y,x)
  end
  
  def box(op1 = 0, op2 =0)
    @window.box(op1, op2)
  end
  
  def scrollok(s)
    @window.scrollok(s)
  end
  
  def scroll()
    @window.scroll()
  end
  
  def move(x, y)
    @window.move(y, x)
  end
  
  def scroll_by(delta)
    @window.scrl(delta)
  end
  
  # Draws text at the given place in this console/window
  def draw(text, x, y, col = nil, rev = nil)
    xy(x,y)
    self.color  = col if col
    self.reverse= rev unless rev.nil?
    self << text
  end
  
  # Draw the text, centered in the middle this console /window
  def draw_center(text, y, col = nil, rev = false)
    x = (self.w / 2) - text.size
    self.draw(text, x, y, col, rev) 
  end
  
  
  def sym2key(sym)
    key = "KEY_#{sym.upcase}".to_sym
    return Curses.const_get(key)
  end
  
  # Gets one key from the interface. Does not echo.
  def getch()
    return self.class.key2sym(@window.getch)
  end
  
  # Gets one line of text from the interface
  def gets(echo = true, cursor = true)
    Curses.curs_set(1) if cursor
    Curses.echo if echo 
    res = @window.getstr
    Curses.noecho if echo
    Curses.curs_set(0) if cursor
    return res
  end
  
end


if __FILE__ == $0
def test
  @screen = Console.init_screen
  @screen.refresh
  @border_win = Console.new(0, @screen.h - 6, @screen.w, 6)  
  
  # @border_win.gotoxy(20,0)
  @border_win.box
  @border_win.draw_center("Messages", 0)
  @border_win.refresh
  @mess_win = Console.new(1, @screen.h - 5, @screen.w - 2, 4)   
  @mess_win.scrollok(true)  
  @mess_win << "start "
  @mess_win << (@border_win.methods - Object.methods).join(', ')
  @mess_win.refresh
  # sleep 3
  x = 2
  y = 2 
  for color in Console::COLORS do 
    @screen.draw("# ", x, y, color, false)
    @screen.draw("# ", x + 2, y, color, true)
    y+=1
  end
    
  
  @screen.draw("#\n", x, y, :white, false)
  @screen << "#{@screen.w} #{@screen.h} "
  @screen <<  (Curses.methods.sort - Object.methods).join(', ')
  @screen << "\n#{Curses.color_pairs}\n"
  
  
  
  # wall_image = Image.new('#', :yellow, true, false)
  # wall_image.draw(@screen, 15, 15)
  @screen.refresh
   
  ch = @screen.getch
  Console.close_screen
  p ch
  # (@screen.methods.sort - Object.methods).join(' ')
end


test

end