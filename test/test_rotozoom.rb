require 'test_common'

module Test
  class Rotozoom < Test::Common
    
    def test_flip
      sprite = Sisa::Surface.load_alpha('test_sprite.png')
      assert sprite, "Couldn't load sprite."
      target = Sisa::Surface.make(sprite.w, sprite.h, 32, nil, true, false, false)
      
#     SDL::Surface::TRANSFORM_SAFE
#         Don't asume that the src and dst surfaces has the same pixel format. This is the default when the two surfaces don't have the same BPP. This is slower but will render wierd pixel formats right. 
#     SDL::Surface::TRANSFORM_AA
#         Use the interpolating renderer. Much slower but can look better. 
#     SDL::Surface::TRANSFORM_TMAP   

      1000.times do 
        wipe_screen
        SDL::Surface.transform_draw(sprite, @screen, 0, -1,1,0,0,sprite.w,sprite.h,SDL::Surface::TRANSFORM_TMAP   )
        @screen.flip
      end
      
    end
  
  end



end
