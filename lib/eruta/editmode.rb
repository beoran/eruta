module Eruta
  class Editmode < Eruta::Mode
    START_MAP = File.join(Fimyfi.map_dir, 'map_0001.ra')
    
#     
#     def ask_for_new_filename()
#       if (!@filename) or @filename.empty?
#         @filename = @ui.ask('File name for new map, or name of map to load?')
#         if (!@filename) or @filename.empty?
#           @ui.textsplash('No file to load or create.', 1.0)
#           return false
#         end  
#         @filename += '.ra' unless @filename =~ /\.ra/
#       end
#       return @filename
#     end
    
    # Class to help track the mouse.
    class Mouse
      attr_accessor :x
      attr_accessor :y
      attr_accessor :b
      def initialize()
        @x, @y, @b = 0, 0, 0
      end
      
      def move(event)
        @x = event.x ; @y = event.y ; @b = event.button
      end
    end
    
    
    # active tile set index.
    attr_reader :index
    # Tile indexes
    attr_reader :xtile
    attr_reader :ytile
    attr_reader :ztile
    
    def initialize(program, name = :edit)
      super(program, name)
      @mouse     = Mouse.new
      @filename  = START_MAP
      @map, err  = Tima::Map.load(@filename)
      unless @map
        raise "Could not load map #{err.fail_message}." 
      end
      @view      = Tima::View.new(0, 0, 
                    program.screen.w, program.screen.h,  @map.wide, @map.high)
      # @map.view  = @view
      # @filename  = nil 
      @showstat  = program.ui.statusdisplay("")
      p program.game.joysticks
      p program.game.joystick(0)
      @index     = 0
      self.index = @map.list.images.keys.sort.first
      @xtile, @ytile, @ztile, = 0, 0, 0 # current tile map coordinates.
    end 
    
    def index=(value)
      value    = @map.list.first if value < @map.list.first
      value    = @map.list.last  if value > @map.list.last
      @nowtile = @map.list.images[value]
      @index   = value
      return @index
    end
    
    def ztile=(value)
      @ztile = CP.clamp(value, 0, @map.layer_count).to_i
    end
    
    def xtile=(value)
      value  = 0 if value < 0
      @xtile = value.to_i
    end
    
    # Sets current y tile coordinate
    def ytile=(value)
      value  = 0 if value < 0
      @ytile = value.to_i
    end
    
    # Sets the current active tile in the map at the active tile coordinates
    def place(ind = nil)
      ind ||= self.index
      @map.layers[@ztile].set(@xtile, @ytile, ind)
    end
    
    # Sets the current active tile in the map from the active tile coordinates
    def pick()
      self.index = @map.layers[@ztile].get(@xtile, @ytile)
    end
    
    # Saves the map being edited, overwriting the old version.
    def save!
      res = @map.save_to(@filename)
      p res
    end
    
  
    def mark_active_tile(screen)
      x1 = @xtile * Tima::TILE_WIDE - @view.x
      y1 = @ytile * Tima::TILE_HIGH - @view.y
      screen.box(x1 , y1, Tima::TILE_WIDE, Tima::TILE_HIGH, [255,255,0])
    end

    def mark_active_selection(screen, map, edit_select, edit_layer, tilewide)
      return false unless edit_select.selected?
      x1 = map.tile_to_screen_x(edit_layer, edit_select.left)
      x2 = map.tile_to_screen_x(edit_layer, edit_select.right + 1)
      y1 = map.tile_to_screen_y(edit_layer, edit_select.top)
      y2 = map.tile_to_screen_y(edit_layer, edit_select.bottom + 1) 
      screen.box(x1, y1, x2 - x1, y2 - y1, [255,255,255])
      return true
    end

    def render(screen)
      @screen = screen
      @map.draw(@screen, @view.x, @view.y)
      # Show active tile under mouse.
      @screen.blit(@mouse.x, @mouse.y, @nowtile) if @nowtile
      # Show tile index
      @font.draw(@screen, @mouse.x, @mouse.y + 20, 
                 "#{@index}", [255,255,255], [0,0,0])
      mark_active_tile(screen)
      
      @ui.draw(screen)
    end
      
    def handle_keydown(event)
      case event.keysym
        when :f2
          self.save!
        when :f10, :escape
          @program.quit!
        when :f12
          @game.fullscreen = !@game.fullscreen
        when :kp_plus
          self.index += 1
        when :kp_minus
          self.index -= 1
        when :kp_multiply
          self.ztile += 1
        when :kp_divide
          self.ztile -= 1
        else 
          puts "Unknown key #{event.keysym}"
      end
    end  
  
    def handle_keyup(event)
      case event.keysym
        when :f10 
          @program.done!
        else
      end
    end
    
    
    # Moves the view to track the mouse
    def move_view()
      dx = dy = 0
      dx += 1 if @mouse.x > (@screen.w - 24) 
      dx += 1 if @mouse.x > (@screen.w - 8) 
      dx -= 1 if @mouse.x  < 24
      dx -= 1 if @mouse.x  < 8
      dy += 1 if @mouse.y > (@screen.h - 24) 
      dy += 1 if @mouse.y > (@screen.h - 8) 
      dy -= 1 if @mouse.y  < 24
      dy -= 1 if @mouse.y  < 8
      if dx != 0 || dy != 0
        @view.move(dx, dy)
      end
    end
    
    # Updates the tile coordinates
    def update_tile_coordinates
      tx = (@mouse.x + @view.x) / 32
      ty = (@mouse.y + @view.y) / 32
      self.xtile = tx
      self.ytile = ty
    end
    
    def handle_mousemove(event)
      @mouse.move(event)
      update_tile_coordinates
    end
    
    def handle_mousescroll(event)
      self.index += event.value
    end
    
    def handle_mousepress(event)
      if event.button == 1
        self.place
      else
        self.pick
      end
    end
    
    def handle_joypress(event)
    end
    
    def handle_joyrelease(event)
    end
    
    def handle_joymove(event)
    end
    
    def load_new_file_if_needed
      return @map if @map || @loading
      @loading = true
      
      @filename = ask_for_new_filename if (!@filename)
      @loading = false
      unless @filename
        return @program.quit!
      end
      return @map
    end
    
    def update
      move_view
      # @map.advance if (@game.frames % 15) == 0
      # Animate map tiles if needed. 
      # @map.update(@screen.w, @screen.h)
      # Move map to correct position, limited to these lower corners.
      # Apply snow fudge factor
    end
  end 
end  
  

