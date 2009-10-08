# Tests screen related functions
require 'test_common'
require 'sdl'

END { SDL.quit() }

module Test
  class Sprite < Test::Common

    
    def setup
      super
      @sprites          = Rogaru::Sprite::List.load
      @sprite           = @sprites[:fake]
      @sprite.visible   = true 
      @sprite.x         = 10
      @sprite.y         = 20
      @sprite.z         = 1
      @sprite.pose      = :stand
      @sprite.direction = :south      
    end
    
    def test_part
      part =  @sprite.drawing[0]
      p part.poses[:south][:stand]
      p part.pose
      frame = part.pose.frames.first
      assert frame.w > 0
      assert frame.h > 0
      assert frame.pixels !~ /\A(\0)+\Z/, "Frame has only blank pixels" 
    end
    
  
    def test_sprite
      assert @screen 
      @sprites          = Rogaru::Sprite::List.load
      assert @sprites , "Load sprites."
      @sprite           = @sprites[:fake]
      assert @sprite, "Load fake sprite."
      @sprite.visible           = true 
      @sprite.visible   = true 
      @sprite.x         = 10
      @sprite.y         = 20
      @sprite.z         = 1
      @sprite.pose      = :stand
      @sprite.direction = :south      

      assert @sprite.visible, "Make sprite visible."
      assert @sprite.x          == 10,      "Sprite set x"
      assert @sprite.y          == 20,      "Sprite set y"
      assert @sprite.z          == 1,       "Sprite set Z"  
      assert @sprite.pose       == :stand,  "Sprite set pose"
      assert @sprite.direction  == :south,  "Sprite set direction"
      
      part =  @sprite.drawing[0]
      
      frames, time, fps = draw_wait do
        wipe_screen
        for layer in 0..3 do 
          @sprites.draw_layer(@screen, layer)
        end  
        # part.pose.draw_at(@screen, 0, 0)        
      end
      assert_drawn(20,50)
      
    end
    
  
  end
end  
