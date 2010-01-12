# encoding: utf-8

INSTALL_DIR = '.'
$LOAD_PATH << "#{INSTALL_DIR}/src"
require 'console'
require 'matrix'
require 'named'

# RNG helper functions
module Dice

  # Random number between twoo values
  def self.random(min, max)
    rand(max - min) + min 
  end
  
  # Throws the given amount of dice with the given amount of sides
  def throw(amount = 3, sides = 6, offset = -1)
    total = 0 
    amount.times { total += (rand(sides) + offset) }
    return total
  end

end

# Normal helper functions
module Aid
  def self.clamp(val, min, max)
    return min if val < min 
    return max if val > max 
    return val
  end
end


# Image determines how anything is displayed
class Image
  
  # Colors, same as the 16 official CSS colors
  COLORS = [ :aqua, :black, :blue, :fuchsia, 
            :gray, :green, :lime, :maroon, 
            :navy, :olive, :purple, :red, 
            :silver, :teal, :white, :yellow] 
  
  attr_reader :text
  attr_reader :color
  attr_reader :bright
  attr_reader :reverse
  
  def initialize(txt = '.', col = :white, rev = false)
    @text   = txt
    @color  = col
    @reverse= rev
  end
  
  def draw(window, x, y)
    window.draw(text, x, y, self.color, self.reverse)
  end  
end

class Tile
  attr_reader :image
  attr_reader :solid
  
  def initialize(text, solid = false, comment = '', color = :white, reverse = false)
    @image = Image.new(text, color, reverse)
    @solid = solid
    @comment = comment
  end
  
  def draw(screen, x, y)
    @image.draw(screen, x, y) 
  end
  
  Wall      = new('#', true , "wall"    , :gray)
  Mountain  = new('^', true , "mountain", :white)
  Pillar    = new('I', true , "pillar"  , :white)
  Tree      = new('T', true , "tree"    , :green)
  Altar     = new('8', true , "altar"   , :white)
  Floor     = new('0', false, "floor"   , :gray)
  Grass     = new('.', false, "grass"   , :green)
  Hedge     = new(';', false, "hedge"   , :green)
  River     = new('~', false, "river"   , :blue)
  Hill      = new('^', false, "hill"    , :maroon)
  Sea       = new('O', false, "sea"     , :navy)
   
end

# Generates places. Generic parent class.
class Generator

  def initialize(place)
    @place = place
  end
  
  # Generates ranndom hotspots for generation of mountains,
  # caves, etc 
  def make_hotspots(min, max, edge = 0.125)
    # Between 3 and 8 "hotspots"
    hotspot_count = Dice.random(min, max)
    hotspot       = []
    # Bias towards the middle of the map
    perc          = edge
    min_spot_x    = (@place.w * perc).round
    max_spot_x    = (@place.w * (1 - (perc))).round
    min_spot_y    = (@place.h * perc).round
    max_spot_y    = (@place.h * (1 - (perc))).round
    # Generate hotspots
    hotspot_count.times do
      x = Dice.random(min_spot_x, max_spot_x)
      y = Dice.random(min_spot_y, max_spot_y)
      hotspot << [x, y]
    end 
    return hotspot
  end
  
  # Retuns the list of the 8 points around X and Y, as array of arrays
  # with the given delta ranges
  def around(x, y, delta_x = 1, delta_y = 1)
    res   =   []
    for dx in ((-delta_x)..delta_x)
      for dy in ((-delta_y)..delta_y)
        newx = x + dx
        newy = y + dy
        # Exclude the point itself
        unless (newx == x) && (newy == y) 
          res   <<  [newx, newy]
        end  
      end
    end 
    return res
  end
  
  # stub that generates a plain grass field
  def generate
    for x in (0...place.w) 
      for y in (0...place.h)
        @place.put(x, y, Tile::Grass)
      end
    end
  end
  
end

class Generator
  class Worldmap < Generator
    
    def make_mountain(spot, height = 15)      
      x, y    = spot
      return if x < 0 || x >= @place.w
      return if y < 0 || y >= @place.h
      return if @field[y][x] > 0 
      # we already visited this point 
      # height      ||= 
      h             = height
      @field[y][x] += h  # - 2 + rand(4) # This changes the "swampiness"
      other   = self.around(x, y)
      h      -= 1
      if h > 0
        # Recursively make mountains
        for point in other do
          make_mountain(point, h)
        end
      end
    end
      
    def generate
      @field = Array.new(@place.h) { Array.new(@place.w) }
      # height field
      
      # Set all to sea first 
      for x in (0...@place.w) 
        for y in (0...@place.h)
          @field[y][x] = 0
        end
      end
      # Create mountain ranges, calculate height field
      max_spots = 1 + ((@place.w * @place.h)**0.55).round 
      min_spots = 1 + (max_spots / 10)
      hotspots  = self.make_hotspots(min_spots, max_spots)
      for spot in hotspots 
        # Dice.random(1, 15)
        make_mountain(spot, 15)
      end  
      # decide on mountains based on height field
      for x in (0...@place.w)
        for y in (0...@place.h)
          rh = @field[y][x]
          case rh 
            when (10..100)
            @place.put(x, y, Tile::Mountain)
            when (5..9)
            @place.put(x, y, Tile::Hill)
            when (1..4) 
            @place.put(x, y, Tile::Grass)
            else  
            @place.put(x, y, Tile::Sea) 
          end
          # Ensure the edge is all sea tiles
          if x == 0 || x == (@place.w - 1)
            @place.put(x, y, Tile::Sea)
          end
          if y == 0 || y == (@place.h - 1)
            @place.put(x, y, Tile::Sea)
          end
        end  
      end
    end  
  end
end  



class Place
  attr_reader :name
  attr_reader :w
  attr_reader :h
  attr_reader :field
  
  def initialize(name, w = 50, h = 20, options = {})
    @name   = name
    @w      = w
    @h      = h
    @field  = Array.new(@h) { Array.new(@w) }
    for x in (0...@w) 
      for y in (0...@h)
        self.put(x, y, Tile::Grass)
      end
    end  
  end
  
  def []=(x, y, tile)
    return @field[y][x] = tile
  end
  
  def put(x, y, tile)
    self[x, y] = tile
  end
  
  def [](x, y)
    row = @field[y]
    return nil unless row
    return row[x]
  end
  
  def draw(screen, x, y, w, h)
    xstart = x
    ystart = y
    xstop  = xstart + w - 1
    ystop  = ystart + h - 1
    for yy in (ystart..ystop)
      for xx in (xstart..xstop)
        tile = self[xx, yy] 
        next unless tile
        tile.draw(screen, xx - xstart, yy - ystart)
      end
    end
  end
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

# A stat is a numerical value that describes a Being or Item.
class Stat < Named
  attr_accessor :now
  attr_accessor :max
  
  def initialize(name, comment = nil, max = nil, now = nil)
    super(name, comment) 
    @max      = max         ; @now = now ; @now ||= @max 
    @comment  = comment || name.to_s.capitalize
  end
   
end

# An entity is something that has physiycal presence and is movable or mobile.
class Entity < Named
  attr_accessor :position 
  
  def new_stat(name, comment = nil, max = nil, now = nil)
    stat                = Stat.new(name, comment, max, now)  
    @stats[stat.to_sym] = stat
    return stat
  end
  
  def initialize(name, detail="")
    super(name, detail)
    @stats    = {}
    @position = Position.new(nil, nil, nil)
  end 
  
  def self.stat(name, comment, max)
    @stats            ||= []
    stat                = Stat.new(name, comment, max, now)  
    @stats[stat.to_sym] = stat
    return stat
  end
  
  # Returns the Stat of this entity with the given name, 
  # or nil if no such Stat exists for this entity
  def [](statname)
    statkey = statname.downcase.gsub(' ', '_').to_sym
    return  @stats[statkey]
  end
  
  def stat_set_now(stat, value)
    return self[stat].now = value
  end
  
  def stat_get_now(stat)
    return self[stat].now  
  end
  
  def stat_set_max(stat, value)
    return self[stat].max = value
  end
  
  def stat_get_max(stat)
    return self[stat].max  
  end
  
  
  
end

# A being is an entity that can actively influence it's surroundings
# and that may be mobile 
class Being < Entity
end
 
# An item is an entity that   
class Item < Entity
  attr_accessor :owner
  attr_accessor :position
end


class World
  attr_reader :beings
  attr_reader :places
  
  def initialize()
    @places = {}
    @beings = []
  end
  
  def add_place(place)
    @places[place.name.to_sym] = place
    return place 
  end
  
  def new_place(name, w, h, options = {})
    place = Place.new(name, w, h, options)
    return self.add_place(place)
  end
  
end




class UI
  
  attr_reader :mapview
  attr_reader :status
  
  def initialize
    # screen
    @screen   = Console.init_screen
    # message window
    @messages = Console.new(10, @screen.h - 5, @screen.w - 11, 4)
    @messages.scrollok(true)
    @msgbuf   = []
    # Status window
    @status   = Console.new(0, 0, 10, @screen.h)
    @mapview  = Console.new(10, 0, @screen.w - 11, @screen.h - 5)
  end
   
  # Adds a message to the message window and buffer 
  def <<(message, color = :white)
    @messages.color  = color
    @messages       << "#{message} "
    @msgbuf         << [message, color]
  end
  
  # Waits for a key press and returns it.
  def wait_key()
    res = nil
    res = @screen.getch until res
    return res
  end
  
  # Asks a simple question that requres a one-key answer and returns the answer
  def ask(question)
    self << "\n#{question} "
    self.draw
    answer = wait_key
    return answer
  end  
  
  def draw
    @screen.refresh
    @messages.refresh
    @status.refresh
  end
end




class Game

  def action(name, *key)
    @actions ||= {}
    if key.respond_to? :each
      key.each { |k| @actions[k] = name.to_sym } 
    else
      @actions[key] = name.to_sym
    end 
  end 
  
  def perform_action(key)
    action = @actions[key]
    if (!action)
      @ui << "\nNo action defined for key: #{key} #{key.class}"
      return false 
    end
    return self.send("action_#{action}")
  end

  def initialize
    @world    = World.new
    @worldmap = @world.new_place('World', 200, 200)
    wmgen = Generator::Worldmap.new(@worldmap)
    wmgen.generate
    @ui       = UI.new
    @state    = :play
    @map_x    = 0
    @map_y    = 0
    @map      = @worldmap   
    action(:quit      , 'Q'     , 27  )
    action(:actions   , :F5           )
    action(:north     , :UP     , '8' )
    action(:northeast , :PPAGE  , '9' )
    action(:east      , :RIGHT  , '6' )
    action(:southeast , :NPAGE  , '3' )
    action(:south     , :DOWN   , '2' )
    action(:southwest , :END    , '1' )
    action(:west      , :LEFT   , '4' )
    action(:northwest , :HOME   , '7' )
    action(:wait      , ' '     , '5' )
    action(:items     , 'i'           )
    action(:cast      , 'c'           )
    action(:wield     , 'w'           )
    action(:save      , 'S'           )
    action(:help      , :F1     , '?' )
    action(:debug     , :F2     , 'D' )    
  end
  
  def action_debug
    @ui << @world.inspect
  end
  
  def scroll_map(by_x, by_y)
    @map_x += by_x
    @map_y += by_y
    @map_x  = Aid.clamp(@map_x, 0, @map.w - @ui.mapview.w)
    @map_y  = Aid.clamp(@map_y, 0, @map.h - @ui.mapview.h)
    @ui.status.draw("(#@map_x,#@map_y)", 0, 0)
  end
  
  def action_north
    scroll_map(0, -1)
  end

  def action_east
    scroll_map(1, 0)
  end
  
  def action_northeast
   scroll_map(1, -1)
  end
  
  def action_southeast
    scroll_map(1, 1)
  end
  
  def action_south
    scroll_map(0, 1)
  end
  
  def action_southwest
    scroll_map(-1, 1)
  end
  
  def action_west
    scroll_map(-1, 0)
  end
  
  def action_northwest
    scroll_map(-1, -1)
  end
  
  def action_wait
  end
  
  def action_cast
  end
  
  def action_items
  end
  
  def action_wield
  end  
  
  def action_help
    @ui << "Not much help available. You can press Q or ESC to quit."
  end  

  def action_save
  end
  
  def action_quit()
#     result = @ui.ask("Quit without saving (y/n)?")
#     # Don't quit unless answer was yes
#     unless ['y', 'Y'].member?(result)
#       @ui << " OK"
#       return false
#     end
    @ui     << "Quiting Ature!"
    @state = :done
    return true 
  end
  
  def action_actions
    @ui << "\nKnown actions: #{@actions.inspect}"
  end
  
  
  def state_welcome
    @ui     << "Welcome to Ature!"
    @ui.draw
    @ui.wait_key
    @state  = :play 
  end
  
  def draw_play    
    @map.draw(@ui.mapview, @map_x, @map_y, @ui.mapview.w, @ui.mapview.h)
    @ui.draw
    @ui.mapview.refresh
  end
  
  def state_play
    while @state && @state != :done 
      draw_play
      key = @ui.wait_key
      perform_action(key)
    end
  end
  
  def main
    while @state && @state != :done
      self.send("state_#@state")
    end  
  end
  
  def self.main
    game = Game.new
    game.main
  end
  
  
end


Game.main






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








