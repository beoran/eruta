require 'test_common'

module Test
  class Rotozoom < Test::Common
    
    def setup
      super
      open_screen(320, 240, false)
    end  
    
    def test_flip
      flags  = SDL::Surface::TRANSFORM_TMAP
      flags  = SDL::Surface::TRANSFORM_SAFE #  + SDL::Surface::TRANSFORM_AA
      sprite = Sisa::Surface.load_alpha('test_sprite.png')
      assert sprite, "Couldn't load sprite."
      target = Sisa::Surface.make(sprite.w * 2, sprite.h * 2, 32, nil, true, false, false)
      angle  = (Math.atan(1.0 / 2.0) / (2 * Math::PI)) * 360.0
      angle  = 180.0
      SDL::Surface.transform_draw(sprite, target, angle, 1, 1, 0, 0,
      sprite.w, sprite.h, flags)
      frames, time, fps = draw_wait do
        wipe_screen
        target.blit(@screen, 100, 100)
      end
      puts "\nFPS: #{fps}"
    end
  
  end



end
