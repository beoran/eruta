require 'gari'
require 'fimyfi'
require 'nofont'
require 'zori' # GUI library
require 'rogaru'
require 'rogaru/physics'
require 'rogaru/utility'
require 'rogaru/tilemap'
require 'rogaru/stardust'
require 'rogaru/world'
require 'rogaru/walker'
require 'rogaru/world'
include Rogaru


module Eruta
  class Mainmode < Eruta::Mode
    START_MAP = 'data/map/map07_test.xml'
    def initialize(program, name)
      super(program, name) 
      p program, name ,self
      @b_color   = [255,125,125]
      @s_color   = [255,255,255]
      @dust      = Stardust::Blood.new(@b_color, 20)
      @slash     = Stardust::Slash.new(@s_color, 1)
      @rain_color= [200,200,255]  
      @snow      = Stardust::Rain.new(@rain_color, amount = 100) 
      @snow.hide = false 
      @dust_x    = 100
      @dust_y    = 200
      @done      = false
      @showstat  = program.ui.statusdisplay("")
      Tilemap::Tile.set_tile_directory(File.join("data", "image", "tile"));
      printf("Loading tile map... ")
      @map       = Tilemap::Map.load_from(START_MAP)
      # Map load seems quite slow... have to optimize it.
      puts("OK!")
      printf("Loading sprites... ")
      @sprites   = Rogaru::Sprite::List.load
      puts("OK!")
      p @sprites.names
#         @sprite          = @sprites[:missing]
#         @sprite.visible  = true
#         @sprite.direction= :south
#         @sprite.pose     = :stand          
      @entname  = File.join("data", "image", "sprite", "missing_body.png")
      @entimage = Gari::Image.loadraw(@entname).optimize(:alpha)
      @player   = Tilemap::Entity.new(100, 200, @entimage)
      @map.entities.push @player
      printf("Setting up world and mobiles...") 
      # world    = Rogaru::Physics::World.new()  
      @world    = Rogaru::World.new()
      ew       = 32 - 2
      eh       = 32 - 2
      @pplayer          = @world.new_actor(100, 200, 1, ew, eh)
      @pfollow          = @world.new_actor(100, 600, 1, ew, eh)
      @pfollow.goal     = @pplayer
      @pfollow.can_push = false
      @pfollow.color    = [200,100,0]
      @pfollow2         = @world.new_actor(100, 400, 1, ew, eh)
      @pfollow2.goal    = @pfollow
      @pfollow.can_push = false
      @pfollow2.color   = [200,100,50]
    
      @pbox1            = @world.new_mobile(150, 200, 1, ew, eh)
      @pbox2            = @world.new_mobile(200, 200, 1, 40, 40)
      @pblock           = @world.new_thing(300, 200, 1, 50, 50)
      @pblock2          = @world.new_thing(400, 200, 1, 20, 20)
      @pbox3            = @world.new_mobile(200, 300, 1, 20, 20)        
      @world.add_map(@map)
      puts("OK!")
      @camera           = Rogaru::Tilemap::Camera.new(0, 0, 
                          program.screen.w, program.screen.h, *@map.map_size)
      @map.camera  = @camera
      @camera.lock_on(@pplayer)
      @angle       = 0.0
      @angled      = 22.5
      @rotosmooth  = true
      @rotimage    = nil
      @fullscreen  = false
      @vx          = 0 
      @vy          = 0
      @speed       = 5.0
      @fill        = true
    end 
    
    def render(screen)
      @screen = screen
      @screen.fill([0,0,0]) if @fill
      @map.draw_with_sprites(@screen, @sprites)
      # map.draw_auto(screen)
      if (@game.frames % 20) == 0
        status = "#{@speed} (#{@map.x}, #{@map.y}) FPS: #{(@game.fps).round} " + 
      "#{@game.frames} #{@pplayer.z} #{@camera.x} #{@camera.y}"
        @showstat.text = status
      end
      @world.draw(@screen, @map.x, @map.y, @font)
      @dust.render(@screen, @dust_x - @camera.x, @dust_y - @camera.y )
      @slash.render(@screen, @dust_x - @camera.x - 8, @dust_y - @camera.y - 16)
      @snow.render(@screen, 0, 0) 
      @ui.draw(screen)
    end
      
    def handle_keydown(event)
      case event.keysym
        when :f10, :escape
          @program.quit!
        when :left
          @vx = -@speed
        when :up
          @vy = -@speed
        when :right
          @vx =  @speed
        when :down
          @vy =  @speed
        when :b 
          # Make some blood come out of the player, just as a test/joke. :p
          @dust      = Stardust::Blood.new(@b_color,20)
          @dust_x    = @pplayer.x + 16
          @dust_y    = @pplayer.y + 16
          @slash     = Stardust::Slash.new(@s_color,1)
        when :f
          @fill      = ! @fill
        when :r
          # Toggle snow/rain
          @snow.hide = ! @snow.hide 
        when :plus
          @speed += 1
        when :minus
          @speed -= 1
        when :f12 
          @fullscreen       = !@fullscreen
          @game.fullscreen  = @fullscreen
        when :s
          puts @map.save_to(File.join('data', 'map', 'map_0001.ra'))
      end
    end  
  
    def handle_keyup(event)
      case event.keysym
        when :f10 
          @program.done! 
        when :left, :right
          @vx = 0
        when :up, :down
          @vy = 0
      end
    end
    
    def handle_joypress(event)
      if @pplayer.z == 1
        @pplayer.z = 3
      else
        @pplayer.z = 1
      end  
    end
    
    def handle_joyrelease(event)
    end
    
    def handle_joymove(event)
      if event.axis == 1  then
          if event.value > 1000 then
            @vy = 1 
          elsif event.value < -1000 then
            @vy = -1 
          else 
            @vy = 0
          end
      end
      if event.axis == 0  then
          if event.value > 1000 then
            @vx = 1 
          elsif event.value < -1000 then
            @vx = -1 
          else 
            @vx = 0
          end
      end
    end
        
    def update
      @ffact = 25.0
      # pplayer.vx = player.vx 
      # pplayer.vy = player.vy    
#         @sprite.x        = @pplayer.x - @camera.x 
#         @sprite.y        = @pplayer.y - @camera.y - 32
#         @sprite.z        = @pplayer.z
#         @sprite.visible  = true 
#         @sprite.pose     = :stand  

      # pplayer.apply_impulse(player.vx, player.vy)
      @pplayer.apply_force(@vx, @vy)
      #pplayer.shape.surface_v = CP::Vec2.new(player.vx * 1000, player.vy * 1000)
      
      @world.update(1.0)
      # A constant value for world.update is much better for Chipmunk...        
      # Though, here the program should skip updates of the world 
      # occasionally if FPS > 60
      # And should also skip drawing to screen if FPS is < 60 (or 30)        
      # Update world, as to simulate 60 updates per second
      # player.update
      # Move player
      @map.advance if (@game.frames % 15) == 0
      # Animate map tiles id needed. TODO: improve clock class to improve this
      @map.update(@screen.w, @screen.h)
      # Move map to correct position, limited to these lower corners.
      # Apply snow fudge factor
      @snow.vx_fudge = @vx 
      @snow.vy_fudge = - @vy if @vy <= 0
      @dust.update
      @slash.update
      @snow.update
      # Advance stream effect and snow
      # GC.enable ; GC.start; GC.disable
    end
  end 
end  