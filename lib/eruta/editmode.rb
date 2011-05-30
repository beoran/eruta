module Eruta
  class Editmode < Eruta::Mode
    START_MAP = 'data/map/map_0001.ra'
    
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
    
    def initialize(program, name = :edit)
      super(program, name)
      @filename  = START_MAP
      @map, err  = Tima::Map.load(@filename)
      @view      = Tima::View.new(0, 0, 
                    program.screen.w, program.screen.h,  @map.wide, @map.high)
      # @map.view  = @view
      @filename  = nil 
      @showstat  = program.ui.statusdisplay("")
    end 
  
    def mark_active_tile(screen, map, edit_x, edit_y, edit_layer, tilewide)
      x1 = map.tile_to_screen_x(edit_layer, edit_x)
      y1 = map.tile_to_screen_y(edit_layer, edit_y)
      screen.box(x1 , y1, tilewide, tilewide, [255,255,255])
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
      @screen = @map.draw(@screen, @view.x, @view.y)
      @ui.draw(screen)
    end
      
    def handle_keydown(event)
      case event.keysym
        when :f10, :escape
          @program.quit!
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
      # @map.advance if (@game.frames % 15) == 0
      # Animate map tiles if needed. 
      # @map.update(@screen.w, @screen.h)
      # Move map to correct position, limited to these lower corners.
      # Apply snow fudge factor
    end
  end 
end  
  

