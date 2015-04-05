
# A widget that can edit a tile map. 
# Extracted from Eruma, still under construction!!!
 
module Zori 
  class Mapeditor < Widget
    def initialize(map)
      super()
      @map    = map
      @status = @hanao.statusdisplay("")
      # @clock  = Rogaru::Clock.new()
      @camera = Rogaru::Tilemap::Camera.new(0, 0, 
                @screen.w, @screen.h, *@map.map_size)
      map.camera  = @camera
      # camera.lock_on(@mousetrack) # Let the camera of the map follow the mouse
      @edit_bg    = nil
      @edit_x     = 0
      @edit_y     = 0 
      @edit_layer = 0 
      @edit_select= 0
      @edit_tile  = 0
      @tilewide   = 32
    end

    def draw(screen)
      screen.fill_rect(0,0,640,480, Sisa::Color::Black) if edit_bg
      @map.draw_auto_with_effects(screen)
      mark_active_tile(screen)
      mark_active_selection(screen)
      status.text  = "XYZ: #{@edit_x}, #{@edit_y}," + 
                     "#{@edit_layer}/#{@map.layer_count-1}" +  
                     ". Tile: #{@edit_tile}. Select: #{@edit_select}"
    end
    
    def on_quit(*args)
      done = Zori::Dialog.yesno('Quit without saving?')
    end
      
    def on_key(sym, mod, text = nil)
      case sym
      when KEY_L      # Add new Layer.
        puts "Map new layer: #{map.tw}, #{map.th}"
        map.new_layer(@map.tw, @map.th) 
      when KEY_C      # Copy selection.        
        @edit_select.copy(@map, @edit_layer)
      when  KEY_P # Paste selection. 
        @edit_select.paste(@map, @edit_x, @edit_y, @edit_layer)
      # Set the active tile to the one currently under the mouse pointer 
      when KEY_F5
        @edit_tile = map.get(@edit_x, @edit_y, @edit_layer)  
      when KEY_F10
        @done = true 
      when KEY_ESCAPE # Exit without save.
        done = Zori::Dialog.yesno('Quit without saving?')
      # Toggle drawing of simple bright pink background
      when KEY_B
        @edit_bg = (not @edit_bg)
      when KEY_F12
        @fullscreen  = !@fullscreen
        @screen.fullscreen = @fullscreen
      # Toggle walkability of tile under cursor and all similar tiles        
      when KEY_W
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.walk = (not tile.walk)  unless tile.nil?
      # Toggle swimmable property of tile  
      when KEY_S
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.swim = (not tile.swim)  unless tile.nil?
      when KEY_J
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.jump = (not tile.jump)  unless tile.nil?
      when KEY_U
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.up = (not tile.up)  unless tile.nil?
      when KEY_D
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.down = (not tile.down)  unless tile.nil?
      when KEY_O
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.pain = (not tile.pain)  unless tile.nil?
      when KEY_E
        tile_i = map.get(edit_x, edit_y, edit_layer)
        tile = map.get_tileset_tile(tile_i)
        tile.ledge = (not tile.ledge) unless tile.nil?
        # Speed of scrolling up
      when KEY_KP_MULTIPLY
        @speed += 5 if @speed < 100
      when KEY_KP_DIVIDE
        @speed -= 5 if @speed > 5
      # Up to next layer        
      when KEY_KP_PLUS
        @edit_layer += 1 if @edit_layer  < (map.layer_count-1)
      # Down to previous layer   
      when KEY_KP_MINUS   
        @edit_layer -= 1 if @edit_layer  > 0
      when KEY_F2, KEY_F10
      # Save map
          Zori::Dialog.textsplash("Saving to #{filename}...", 1.0)
            screen.flip()
            map.save_to(filename)
            Zori::Dialog.textsplash("Saved to #{filename}", 2.0)
      end
   end
     
   def mouse_scroll(*args)
        elsif (event.is_a? Sisa::Event::MouseButtonDown) then 
          # Handle mouse button roll up.
          if event.button == 5 and edit_tile < map.tileset.last_index
            edit_tile  =  map.tileset.next_index(edit_tile)
          end
          # Handle mouse button roll down.
          if event.button == 4 
            if edit_tile > map.tileset.first_index
              edit_tile  =  map.tileset.previous_index(edit_tile)
            else
              edit_tile  = -1 # negative index for transparent tile.
            end  
          end
          # Negative tile index erases to transparency
          if event.button == 1 or event.button == 4 or event.button == 5 then
            if(edit_select.selected?)
              edit_select.fill(map, edit_layer, edit_tile)
            else
              map.set(edit_x, edit_y, edit_layer, edit_tile) 
            end
          elsif event.button == 3  then
            if(edit_select.empty?) 
              edit_select.set_first(edit_x, edit_y)
            elsif(edit_select.started?) 
              edit_select.set_second(edit_x, edit_y)
            else
              edit_select.clear!  # remove selection
            end
          end
        elsif (event.is_a? Sisa::Event::MouseButtonUp) then 
        elsif(event.is_a? Sisa::Event::MouseMotion) then 
          mouse_x = event.x
          mouse_y = event.y          
          edit_x  = map.screen_to_tile_x(edit_layer, mouse_x)
          edit_y  = map.screen_to_tile_y(edit_layer, mouse_y)
        end
      end
      calculate_map_speed(screen, map, speed, mouse_x, mouse_y, @mousetrack)
      # The function calculate_map_speed allows to scroll the map with the mouse.
      @mousetrack.move_by(@vx, @vy)
      # Move to follow cursor
      map.advance if (clock.frames % 15) == 0
      # Animate map tiles id needed. TODO: improve clock class to improve this
      map.update(screen.w, screen.h)
      # Move map to correct position, limited to these lower corners.
      clock.tick
    end
    stop   = Time.now()
    fps    = clock.framerate
    puts "Average FPS: #{fps}\n"
    return true
  end

    
  
  
  
  end # class Mapeditor
end # module Zori