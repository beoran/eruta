#!../ext/bin/ruby
#
# Copyright Bjorn De Meyer 2007. Can be used freely under the zlib license.
#
#

BEGIN { $:.unshift(File.dirname(__FILE__)); }
# Include this script's directory in Ruby's search path.

require 'sisa'
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



class Eruta

START_MAP = 'map07_test.xml'
# START_MAP = 'map07_test.tmx'
# The TMX loader is curretly a LOT slower than the normal XML one, 
# probably because of the inefficient layout of the TMX file.  


def event_handle(event) 
  return true if event.is_a?(Sisa::Event::Quit) 
  return false
end

def time_block
   start_time = Time.now
   yield
   return Time.now - start_time
end 

  class Queue
    def initialize
      @posted = []
    end
    def each
      @posted.each do |event|
        yield event
      end  
      @posted = []
      event = SDL::Event2.poll
      while event do
        yield event
        event = SDL::Event2.poll
      end        
    end    
    def post(event)
      @posted << event
    end
  end  
  

  def screen_make(wide, high, fullscreen, doublebuf = true) 
    return Sisa::Screen.make!(wide, high, fullscreen)
  end 

  def event_setup()
    SDL.init(SDL::INIT_VIDEO | SDL::INIT_AUDIO | SDL::INIT_TIMER )
    queue = Sisa::Event.queue
  end
  
def main(fullscreen = false)
  # fullscreen= (ENV['HOME'] == '/home/bjorn')
  queue     = event_setup()
  screen    = screen_make(640, 480, fullscreen)
  joysticks = Utility.joystick_setup()
  b_color   = [255,125,125]
  s_color   = [255,255,255]
  dust      = Stardust::Blood.new(b_color,20)
  slash     = Stardust::Slash.new(s_color,1)
  rain_color= [200,200,255]  
  snow      = Stardust::Rain.new(rain_color, amount = 100) 
  snow.hide = true 
  dust_x    = 100
  dust_y    = 200 
  p joysticks
  gui       = Zori.open(screen, queue)
  # clock   = Sisa::Clock.new()
  font      = Sisa::Font.load("crystal", 14)
  done      = false
  statusdisp= gui.statusdisplay("")
  Tilemap::Tile.set_tile_directory(File.join("..", "share", "image", "tile"));
  printf("Loading tile map... ")
  map       = Tilemap::Map.load_from(START_MAP)
  # Map load seems quite slow... have to optimize it.
  puts("OK!")
  clock     = Rogaru::Clock.new()
  screen.fill([ 0xff, 00, 00]);
  start     = Time.now()
  times     = 1000
  i         = 0
  done      = false
  speed     = 2.5  
  # mem_count, mem_size = *Utility.used_memory
  entname  = File.join("..", "share", "image", "sprite_test_2.png")
  entimage = Sisa.load_alpha(entname)
  player   = Tilemap::Entity.new(100, 200, entimage)
  
  map.entities.push player
  printf("Setting up world and mobiles...") 
  # world    = Rogaru::Physics::World.new()  
  world    = Rogaru::World.new()
  ew       = 32 - 2
  eh       = 32 - 2
  pplayer  = world.new_actor(100, 200, 1, ew, eh)
  pfollow  = world.new_actor(100, 600, 1, ew, eh)
  pfollow.goal = pplayer
  pfollow.can_push = false
  pfollow.color = [200,100,0]
  pfollow2  = world.new_actor(100, 400, 1, ew, eh)
  pfollow2.goal = pfollow
  pfollow.can_push = false
  pfollow2.color = [200,100,50]
  
  pbox1    = world.new_mobile(150, 200, 1, ew, eh)
  pbox2    = world.new_mobile(200, 200, 1, 40, 40)
  pblock   = world.new_thing(300, 200, 1, 50, 50)
  pblock2  = world.new_thing(400, 200, 1, 20, 20)
  pbox3    = world.new_mobile(200, 300, 1, 20, 20)
  printf("OK!\nSetting up world from map...")
  world.add_map(map)
  puts("OK!")  
#  pother   = physics::Entity.new(200, 200, ew, eh, 1, 100)
#  pother2  = physics::Entity.new(300, 200, ew, eh, 1)
#  pother3  = physics::Entity.new(400, 200, ew, eh, 2)

#  world.add(pplayer)
#  world.add(pother)
#  world.add(pother2)
#  world.add(pother3)
  camera      = Rogaru::Tilemap::Camera.new(0, 0, screen.w, screen.h, *map.map_size)
  map.camera  = camera
  camera.lock_on(pplayer)
  angle       = 0.0
  angled      = 22.5
  rotosmooth  = true
  rotimage    = nil
=begin
  # D star light is somehow 1.5 times faster than A star, and 
  # even up to 20 times faster for a long, impossible path.   
  res, walk_path = Rogaru::Walker::Dstar.calculate(map, 5, 5, 6, 6)
  p res
  p walk_path
  start   = Time.now()
  10.times { res, walk_path   = Rogaru::Walker::Dstar.calculate(map, 5, 5, 10, 13) }   
  stop   = Time.now()
  p res
  p walk_path
  puts "Calculated 10 paths in #{stop-start}s: #{10/(stop-start)}"
  
  start   = Time.now()
  10.times { res, walk_path   = Rogaru::Walker::Dstar.calculate(map, 57, 57, 10, 11) }   
  stop   = Time.now()
  p res
  p walk_path
  puts "Calculated 10 impossible paths in #{stop-start}s: #{10/(stop-start)}"



  walker      = Rogaru::Walker.new(map, 1)
  walk_path   = walker.find_path(5, 5, 6, 6)
  p walk_path
  
  start   = Time.now()
  1.times { walk_path   = walker.find_path(5, 5, 10, 13) }   
  stop   = Time.now()
  p walk_path
  puts "Calculated 1 paths in #{stop-start}s: #{1/(stop-start)}"
  
  start   = Time.now()
  1.times { walk_path   = walker.find_path(57, 57, 10, 11) }   
  stop   = Time.now()
  p walk_path
  puts "Calculated 1 impossible paths in #{stop-start}s: #{1/(stop-start)}"
  
  return false
=end  

  puts "Starting render."
  until done do
    i += 1
    if (i % 500) == 0 then
      mem_count, mem_size = *Utility.used_memory
    end
    #screen.fill([0,191,0])  
    map.draw_auto(screen)
    status = "#{speed} #{map.x} #{map.y} #{(clock.framerate).round} #{mem_count}\
    #{pplayer.z} #{camera.x} #{camera.y}"
    statusdisp.text = status    
    world.draw(screen, map.x, map.y, font)
    # rotimage = entimage.rotozoom(angle, [1,1], rotosmooth)
    # Rotozoom is slow. Order of magnitude: 1 to 2 ms per rotozoom. 
    # We will have to cache it.    
    if rotimage
      rotimaged = rotimage.to_display_alpha()
      rotimaged.blit(screen, [300, 300])
      rotimage  = nil
    end
    dust.render(screen, dust_x - camera.x, dust_y - camera.y )
    slash.render(screen, dust_x - camera.x - 8, dust_y - camera.y - 16)
    # draw our spray
    snow.render(screen, 0, 0)     # draw the snowfall
    # gui.draw(screen)
    screen.flip()
    queue.each do   
      | event | done = event_handle(event) 
      case event
      when Sisa::Event::KeyDown
        done = true if event.sym == Sisa::Key::F10
        done = true if event.sym == Sisa::Key::ESCAPE
        player.vx = -speed if event.sym == Sisa::Key::LEFT
        player.vy = -speed if event.sym == Sisa::Key::UP
        player.vx =  speed if event.sym == Sisa::Key::RIGHT
        player.vy =  speed if event.sym == Sisa::Key::DOWN
        if event.sym == Sisa::Key::B 
          # Make some blood come out of the player, just as a test/joke. :p
          dust      = Stardust::Blood.new(b_color,20)
          dust_x    = pplayer.x + 16
          dust_y    = pplayer.y + 16
          slash     = Stardust::Slash.new(s_color,1)
        end
        if event.sym == Sisa::Key::R 
          # Toggle snow/rain
          snow.hide = ! snow.hide 
        end  
  
        #angle    += angled if event.sym == Sisa::Key::KP_PLUS
        #angle    -= angled if event.sym == Sisa::Key::KP_MINUS        
        speed += 1 if event.sym == Sisa::Key::KP_PLUS
        speed -= 1 if event.sym == Sisa::Key::KP_MINUS
        if event.sym == Sisa::Key::F12
          fullscreen  = !fullscreen                   
          screen      = Utility.screen_make(640, 480, fullscreen)
        end
      when Sisa::Event::KeyUp
        done = true if event.sym == Sisa::Key::F10
        player.vx = 0 if event.sym == Sisa::Key::LEFT
        player.vy = 0 if event.sym == Sisa::Key::UP
        player.vx = 0 if event.sym == Sisa::Key::RIGHT
        player.vy = 0 if event.sym == Sisa::Key::DOWN
      when Sisa::Event::JoyDown

          if pplayer.z == 1
            pplayer.z = 3
          else
            pplayer.z = 1
          end  

      when Sisa::Event::JoyAxis
        
        if event.axis == 1  then
            if event.value > 1000 then
              player.vy = 1 
            elsif event.value < -1000 then
              player.vy = -1 
            else 
              player.vy = 0
            end
        end
        if event.axis == 0  then
            if event.value > 1000 then
              player.vx = 1 
            elsif event.value < -1000 then
              player.vx = -1 
            else 
              player.vx = 0
            end
        end
      end
    end
    ffact = 25.0
    pplayer.vx = player.vx 
    pplayer.vy = player.vy    
    # pplayer.apply_impulse(player.vx, player.vy)
    pplayer.apply_force(player.vx, player.vy)
    #pplayer.shape.surface_v = CP::Vec2.new(player.vx * 1000, player.vy * 1000)
    fr = clock.framerate
    world.update(1.0)
    # A cunstant value for world.update is much bettter for Chipmunk.
    # Though, here the program should skip updates occasionally if FPS > 60
    # And should also skip drawing to screen if FPS is < 60 (or 30)
    
    # Update world, as to simulate 60 updates per second
    # player.update
    # Move player
    map.advance if (clock.frames % 15) == 0
    # Animate map tiles id needed. TODO: improve clock class to improve this
    map.update(screen.w, screen.h)
    # Move map to correct position, limited to these lower corners.
    # Apply snow fudge factor
    snow.vx_fudge = player.vx 
    snow.vy_fudge = - player.vy if player.vy <= 0
    dust.update
    slash.update
    snow.update
    # Advance stream effect and snow
    clock.tick
    # GC.enable ; GC.start; GC.disable
  end
  stop   = Time.now()
  fps    = clock.framerate
  puts "Average FPS: #{fps}\n"
  return 0
end

def test_music() 
  mix = Sisa::Mixer
  # Use the lines below to get rid of artsd and contact ALSA directly on Linux.
  # ARTSD happens to be buggy on my old, old linux distro. 
  if RUBY_PLATFORM =~ /linux/
    `killall artsd`
    ENV['SDL_AUDIODRIVER'] = "alsa"
  end  
  mix.open_audio( mix::DEFAULT_FREQUENCY, mix::DEFAULT_FORMAT, 2, 1024)
  puts "Using audi driver:" + mix.driver_name
  music = mix::Music
  
  mus = music.load_audio("../test/music_test.wav");
  
  puts('Testing fade_in 3 times 2000 ms.')
  mus.fade_in(3, 2000);
  puts('ERROR: Music not fading in') unless mus.fading_in?
  
  puts('Waiting for fading_in? to end');
  while mus.fading_in? do Thread.pass end  

  puts('Testing volume')
  mus.volume = 60;
  puts('ERROR: Volume is not 60') if 60 != mus.volume
  
  puts('Testing pause')
  mus.pause
  puts('ERROR: Music not paused') unless mus.paused?

  puts('Sleeping 1 second');
    sleep 1

  puts('Testing resume')
  mus.resume
  puts('ERROR: Music not resumed') unless mus.playing?
  
  puts('Playing until end')
  helper = 0
  while mus.playing? do Thread.pass end
  # Test playing of music to the end
  puts('Played until end')
  puts('ERROR: Music not ended') if mus.playing?
  
  puts('Testing play 1 times')
  mus.play(1)
  puts('ERROR: Music not playing') unless mus.playing?
    
  puts('Testing fade_out')
  mus.fade_out(2000);
  puts('ERROR: Music not fading out ') unless mus.fading_out?
  
    puts('Fading until end')
    while mus.playing? or mus.fading_out? do Thread.pass end
  # Test playing of music to the end

  puts('ERROR: Music not ended') if mus.playing?
  mix.close_audio
end

def play_music() 
  mix = Sisa::Mixer
  # Use the lines below to get rid of artsd and contact ALSA directly on Linux.
  # ARTSD happens to be buggy on my old, old linux distro. 
  if RUBY_PLATFORM =~ /linux/
    `killall artsd`
    ENV['SDL_AUDIODRIVER'] = "alsa"
  end  
  mix.open_audio( mix::DEFAULT_FREQUENCY, mix::DEFAULT_FORMAT, 2, 128 * 16 )
  puts "Using audio driver:" + mix.driver_name
  if (mix.driver_name!= 'alsa' && RUBY_PLATFORM =~ /linux/) 
    puts 'Warning: Set SDL_AUDIODRIVER environment variable to alsa for better sound quality.' 
  end  
  music = mix::Music
  mus = music.load_audio("../test/music_test.wav");
  mus.play(-1)
end
end

game = Eruta.new
# play_music()
game.main ARGV[0]

# music_thread = Thread.new do test_music() end
# main_thread  = Thread.new do main() end
# main_thread.join
# music_thread.join


