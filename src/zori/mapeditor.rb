require 'zori'
require 'rogaru'

# A widget that can edit a tile map. 
# Extracted from Eruma
 
module Zori 
  class Mapeditor
    def initialize(map)
      super()
      @map    = map
      @status = @hanao.statusdisplay("")
      @clock  = Rogaru::Clock.new()
      @camera = Rogaru::Tilemap::Camera.new(0, 0, 
                @screen.w, @screen.h, *@map.map_size)
      map.camera  = camera
    end

    camera.lock_on(@mousetrack) # Let the camera of the map follow the mouse
    
    until done do
      screen.fill_rect(0,0,640,480, screen.format.map_rgb(255, 0, 255)) if edit_bg
      map.draw_auto_with_effects(screen)
      mark_active_tile(screen, map, edit_x, edit_y, edit_layer, tilewide)  
      mark_active_selection(screen, map, edit_select, edit_layer, tilewide)
      status.text  = "#{filename} Speed: #{speed}. XYZ: #{edit_x}, #{edit_y}," + 
                     "#{edit_layer}/#{map.layer_count-1}" +  
                     ". Tile: #{edit_tile}. Select: #{edit_select}"
      # status.draw_all(screen)
      gui.draw(screen)
      screen.flip()
      queue.each do | event | 
        if(event.is_a? Sisa::Event::Quit) then         
          done = Zori::Dialog.yesno('Quit without saving?')
        elsif (event.is_a? Sisa::Event::KeyDown) then 
          if event.sym == Sisa::Key::L
            puts "Map new layer: #{map.tw}, #{map.th}"
            map.new_layer(map.tw, map.th) 
          end
          # Add new Layer.
          edit_select.copy(map, edit_layer) if event.sym == Sisa::Key::C
          # Copy selection.
          edit_select.paste(map, edit_x, edit_y, edit_layer) if event.sym == Sisa::Key::P
          # Paste selection.
          edit_tile = map.get(edit_x, edit_y, edit_layer) if event.sym == Sisa::Key::F5
          # Set the active tile to the one currently under the mouse pointer
          done = true if event.sym == Sisa::Key::F10
          # Exit with save.
          if event.sym == Sisa::Key::ESCAPE
            done = Zori::Dialog.yesno('Quit without saving?')
          end  
          # Exit without save.

          edit_bg = (not edit_bg) if event.sym == Sisa::Key::B
          # Toggle drawing of simple bright pink background

          # Toggle full screen
          if event.sym == Sisa::Key::F12
            fullscreen  = !fullscreen
            screen      = screen_make(640, 480, fullscreen, true)
            # gui         = Zori.open(screen, queue)
          end

          if event.sym == Sisa::Key::W
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.walk = (not tile.walk)  unless tile.nil?
          end
          # Toggle walkability of tile under cursor and all similar tiles
          if event.sym == Sisa::Key::S
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.swim = (not tile.swim)  unless tile.nil?
          end
          # Toggle swimmable property of tile
          if event.sym == Sisa::Key::J
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.jump = (not tile.jump)  unless tile.nil?
          end
          # Toggle jump property of tile
          if event.sym == Sisa::Key::U
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.up = (not tile.up)  unless tile.nil?
          end
          # Toggle Up property of tile
          if event.sym == Sisa::Key::D
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.down = (not tile.down)  unless tile.nil?
          end
          # Toggle Down property of tile
          if event.sym == Sisa::Key::O
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.pain = (not tile.pain)  unless tile.nil?
          end
          # Toggle Ouch property of tile, that is pain
          if event.sym == Sisa::Key::E
            tile_i = map.get(edit_x, edit_y, edit_layer)
            tile = map.get_tileset_tile(tile_i)
            tile.ledge = (not tile.ledge) unless tile.nil?
          end
          # Toggle Edge/Ledge property of tile
           speed += 5 if event.sym == Sisa::Key::KP_MULTIPLY and speed < 100
          # Speed of scrolling up
           speed -= 5 if event.sym == Sisa::Key::KP_DIVIDE and speed > 5
          # Speed of scrolling down
          edit_layer += 1 if event.sym == Sisa::Key::KP_PLUS and edit_layer  < (map.layer_count-1)
          # Up to next layer
          edit_layer -= 1 if event.sym == Sisa::Key::KP_MINUS and edit_layer > 0
          # Down to previous layer
          if event.sym == Sisa::Key::F2  or event.sym == Sisa::Key::F10 then
          # Save map
            Zori::Dialog.textsplash("Saving to #{filename}...", 1.0)
            screen.flip()
            map.save_to(filename)
            Zori::Dialog.textsplash("Saved to #{filename}", 2.0)
          end
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