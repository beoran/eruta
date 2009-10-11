# encoding: utf-8

require 'curses'

class Place
end

class Position
  # Place this position is in
  attr_accessor :place
  # X coordinate in grid values
  attr_accessor :x
  # Y coordinate in grid values
  attr_accessor :y
  
  def initialize(place, x, y)
    self.x      = x
    self.y      = y
    self.place  = place
  end
end

# A stat is a numerical value that describes a Being or Item
class Stat
  attr_reader   :name
  attr_reader   :comment
  attr_accessor :now
  attr_accessor :max
  
  def initialize(name, comment = nil, max = nil, now = nil)
    @name     = name.to_sym 
    @max      = max         ; @now = now ; @now ||= @max 
    @comment  = comment || name.to_s.capitalize
  end
      
end

class Entity
  attr_accessor :position 
  
  def new_stat(name, comment = nil, max = nil, now = nil)
    stat              = Stat.new(name, comment, max, now)  
    @stats[stat.name] = stat
    return stat
  end
  
  def initialize
    @stats    = {}
    @position = Position.new(nil, nil, nil)
  end  
end

class Being < Entity
  
  
end
 
class Item < Entity
  attr_accessor :owner  
  attr_accessor :position
end


class World
end

# Image determines how anything is displayed
class Image
  COLORS = [ :black, :green, :red, :cyan, :white, :magenta, :blue, :yellow ]
  
  attr_reader :text
  attr_reader :color
  attr_reader :bright
  attr_reader :reverse
  
  def initialize(txt = '.', col = :white, bri = false, rev = false)
    @text   = txt
    @color  = col
    @bright = bri
    @reverse= rev
  end
  
  def draw(window, x, y)
    window.draw(text, x, y, self.color, self.bright, self.reverse)
  end  
end


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
    @invert   = false
    @bright   = false
  end
  
  BLACK    = 1
  GREEN    = 2
  RED      = 3
  CYAN     = 4
  WHITE    = 5
  MAGENTA  = 6
  BLUE     = 7
  YELLOW   = 8
   
  
  
  COLOR_LOOKUP =  { :black => BLACK, :green => GREEN, 
                    :red   => RED  , :cyan => CYAN, 
                    :white => WHITE, :magenta => MAGENTA, 
                    :blue  => BLUE , :yellow => YELLOW }

  # Sets the current console color
  def color=(colsym)
    col      = COLOR_LOOKUP[colsym.to_sym] || colsym.to_i
    if col  != @color
      @color = col
      if @colorok
        @window.color_set(@color)
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
  def draw(text, x, y, col = nil, bri = nil, rev = nil)
    xy(x,y)
    self.color  = col if col
    self.bright = bri unless bri.nil?
    self.reverse= rev unless rev.nil?
    self << text
  end
  
  # Draw the text, centered in the middle this console /window
  def draw_center(text, y, col = nil, bri = false, rev = false)
    x = (self.w / 2) - text.size
    self.draw(text, x, y, col, bri, rev) 
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
  @mess_win << "start"
  @mess_win << (@border_win.methods - Object.methods).join(', ')
  # @screen.refresh
  # sleep 3
  x = 2
  y = 2 
  for color, value in Console::COLOR_LOOKUP do 
    @screen.draw("# ", x, y, color, false, false)
    @screen.draw("# ", x + 2, y, color, true, false)
    @screen.draw("# ", x + 4, y, color, false, true)
    @screen.draw("# ", x + 8, y, color, true, true)
    y+=1
  end
    
  @screen << "#{@screen.w} #{@screen.h}"
  wall_image = Image.new('#', :yellow, true, false)
  wall_image.draw(@screen, 15, 15)
  @screen.refresh
   
  ch = @screen.getch
  Console.close_screen
  p ch
  # (@screen.methods.sort - Object.methods).join(' ')
end


test

=begin

require 'game'
require 'curses'
require 'action'
require 'game_actions'

include Curses

class CursesUI

  @@tileset = {
    :Floor      => {:char => ".", :color => COLOR_WHITE},
    :Trap       => {:char => "t", :color => COLOR_RED},
    :Wall       => {:char => "#", :color => COLOR_WHITE},
    :ClosedDoor => {:char => "+", :color => COLOR_RED},
    :Stair_up   => {:char => ">", :color => COLOR_WHITE},
    :Stair_down => {:char => "<", :color => COLOR_WHITE},
    :Column     => {:char => "0", :color => COLOR_WHITE},
    :Grass      => {:char => ".", :color => COLOR_GREEN},
    :Hill       => {:char => ":", :color => COLOR_GREEN},
    :Tree       => {:char => "T", :color => COLOR_RED},
    :Water      => {:char => "~", :color => COLOR_BLUE},
    :Mountain   => {:char => "^", :color => COLOR_WHITE},
    :Cave       => {:char => "*", :color => COLOR_WHITE}
  }

  @@pixset = {
    :tiger => 't',
    :frog  => 'f',
    :human => '@',
    :objects => '%',
  }

  @@actions = Hash.new

  def initialize(debug=false)

    @scr = Curses.init_screen
    Curses.start_color
    Curses.raw
    Curses.noecho
    Curses.curs_set(0)
    @scr.keypad(true)
    @scr.refresh
    
    # Color pairs definitions
    Curses.init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
    Curses.init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    Curses.init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    Curses.init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    Curses.init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    Curses.init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    Curses.init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    Curses.init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);

    
    # Init the action map
    init_actions
    
    # Init game
    @game = Game.new
    @game.generate_random

    # Draws the windows
    draw_windows
   
    game_loop
  end

  ##
  # Draws the game windows
  #
  def draw_windows
    # Map window
    @map_win = Window.new(20, 60, 0, 20)
    @map_win.refresh

    # Attributes window
    border_win = Window.new(20, 20, 0, 0)
    border_win.box(0,0)
    border_win << 'Attrs'
    border_win.refresh
    @att_win = Window.new(18, 18, 1, 1)
    @att_win.refresh

    # Messages window
    border_win = Window.new(10, 80, 20, 0)
    border_win.box(0,0)
    border_win << "Messages"
    border_win.refresh
    @mess_win = Window.new(8, 78, 21, 1)
    @mess_win.scrollok(true)
    @mess_win.refresh
  end
  
  ##
  # Draws the current view of the map onto the map window
  #
  def draw_map(win, x, y)
    map = @game.map
    a = x - win.maxx/2
    b = y - win.maxy/2
    (0..59).each do |i|
      (0..19).each do |j|
        win.setpos(j,i)
        if @game.player.memory[a+i,b+j]
          draw_tile(win, map[a+i,b+j])
        else
          draw_tile(win, nil)
        end
      end
    end
  end
  
  ##
  # Draws a tile to the given window
  #
  def draw_tile(win, tile)
    if tile
      style = tile.type
      win.attron(A_BOLD) 
      case tile.nb_items
      when 0: 
        win.color_set(@@tileset[style][:color])
        win.addstr(@@tileset[style][:char])
      #when 1: put single object pixset there
      else
        win.color_set()
        win.addstr(@@pixset[:objects])
      end
    else
      win.addstr(" ")
    end
  end

  ##
  # Draws the player
  #
  def draw_player(win, i, j)
    win.setpos(j, i)
    win.attron(A_BOLD) 
    win.color_set(COLOR_YELLOW)
    win.addstr("@")
  end
  
  ##
  # Draws a creature
  #
  def draw_creature(win, a, b, creature)
    # Changing referential
    i = creature.x - a
    j = creature.y - b
    
    if i>0 && i<win.maxx && j>0 && j<win.maxy then
      win.setpos(j,i)
      win.attron(A_BOLD)
      case creature.age.to_f/creature.max_age.to_f
        when 0..0.25 then win.color_set(COLOR_YELLOW)
        when 0.25..0.5 then win.color_set(COLOR_CYAN)
        when 0.5..0.75 then win.color_set(COLOR_MAGENTA)
        when 0.75..1 then win.color_set(COLOR_RED)
      end
      win.addstr(@@pixset[creature.pix])
    end
  end
  
  ##
  # Draws all creatures
  #
  def draw_creatures(win, x, y)
    # Referential coordinates:
    a = x - win.maxx/2
    b = y - win.maxy/2

    @game.creatures.each do |c|
      if @game.player.memory[c.x, c.y]
        draw_creature(win, a, b, c)
      end
    end
  end

  ##
  # Regroup all drawing methods
  #
  def draw_all(win, x, y)
    draw_map(win, x,y)
    draw_player(win, win.maxx/2, win.maxy/2)
    draw_creatures(win, x, y) if @game.view == :current_map
    win.refresh
  end

  ##
  # Shows the text in the message window
  #
  def draw_message(win, m)
    win << m + "\n"
    win.scroll if win.cury == win.maxy
  end
 
  ##
  # Does the game loop (including key reading and dispatching)
  #
  def game_loop
    # Loop controller
    playing = true
    
    # Draw the screen first time
    x = @game.player.x
    y = @game.player.y
    draw_all(@map_win, x, y)
    
    # Real game loop
    while playing do
      # First, we show all queued messages
      while m = @game.read_message do
        draw_message(@mess_win, m)
      end
      @mess_win.refresh

      playing = handle_input(@scr)
    
      # Launches the game logics
      @game.iterate

      x = @game.player.x
      y = @game.player.y
      draw_all(@map_win, x, y)
    end
  end

 
  ##
  # Initialize a map of key / actions
  # For each key, the first actions whose test matches will be called.
  def init_actions

    # These are the keys whose only purpose is
    # to terminate the game.
    @@show_stoppers = [KEY_F2, 'q'[0]]
    
    # I don't know for you, but I don't like the idea that
    # I have to use this [0] all the time.
    # So I JUST GET RID OF IT.
    # D.R.Y.
    # btw, if I realize that most of the actions associated
    # to keys always have this no-parameter constructor, 
    # It might be better to use a map of key / class name, 
    # and have the class be built for ourselves.
    # End of the long comment.
    keys = { 'c' => [UndirectedChatAction.new],
      '<' => [GoInAction.new],
      '>' => [GoOutAction.new]}

    keys.each do |key, action|
      @@actions[key[0]] = action
    end
    
    # Special handling for arrow keys
    dirs = {KEY_UP => :up, 
      KEY_LEFT => :left, 
      KEY_RIGHT => :right, 
      KEY_DOWN => :down}
      
    dirs.each do |key, dir|
      # Specify it here if arrow 
      # keys need to be given another meaning
      @@actions[key] = [ChatAction.new(dir), 
                        MoveAction.new(dir)]
    end
      
    
  end  
    
  # Work out the input
  def handle_input(scr)
    keep_playing = true
    
    key = scr.getch
    
    if (@@show_stoppers.include?(key))
      keep_playing = false
    else
      if (@@actions.has_key?(key))
        attempts = @@actions[key]
        # This will go through all the actions 
        # until one is executed (if any)
        attempts.find do |action| 
          action.attempt(@game) 
        end
      end
    end
        
    return keep_playing
  end

end

=end








