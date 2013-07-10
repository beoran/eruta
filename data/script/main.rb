# Flags of a Sprite, frame or layer. */
# Zero flag means sprite , frame, etc not in use. */
  SPRITE_EMPTY          = 0
# Sprite elemnt is in use if this is set, not if not. */ 
  SPRITE_ACTIVE         = 1
# Sprite element does not need to be updated. */
  SPRITE_FREEZE         = 2
# Direction flags */
  SPRITE_NO_DIRECTION   = 0
  SPRITE_SOUTH          = 1 << 8
  SPRITE_EAST           = 1 << 9
  SPRITE_NORTH          = 1 << 10
  SPRITE_WEST           = 1 << 11
  SPRITE_ALL            = SPRITE_SOUTH + SPRITE_EAST + SPRITE_NORTH + SPRITE_WEST

# /* Type of the sprite action. The numbers correspond with the 
#  * row in the liberated pixel cup sprite sheet compilation divided by 4, 
#  * which is used  placeholder art now. The real art will have the same structure 
#  * for it's sprite sheets.
#  * 
#  * To limit art requirements, emotions will be showed with emoticons and through
#  * the character portraits in the dialogues.
#  * 
#  */
  SPRITE_CAST           = 0
  SPRITE_STAB           = 1
  SPRITE_WALK           = 2
  SPRITE_SLASH          = 3
  SPRITE_SHOOT          = 4
  SPRITE_DOWN           = 5
  
#   /* The positions below are not in the LPC 
#    * sprite sheet but could be synthesized or used in the real art. */  
  SPRITE_RUN            = 6
  SPRITE_HURT           = 7
  SPRITE_STAND          = 8
  SPRITE_DEFEND         = 9
  

# /* Sprite layer suggested uses. */
  SPRITELAYER_BEHINDBODY = 0
  SPRITELAYER_BODY       = 1
  SPRITELAYER_HEAD       = 2
  SPRITELAYER_EYES       = 3
  SPRITELAYER_HAIR       = 4
  SPRITELAYER_HANDS      = 5
  SPRITELAYER_FEET       = 6
  SPRITELAYER_LEGS       = 7
  SPRITELAYER_TORSO      = 8
  SPRITELAYER_ACCESSORIES= 9
  SPRITELAYER_WEAPONS    = 10 



script "helpers.rb"

puts "Hi from ruby!"

def Alex(text)
  puts "Alex: #{text}"
end

def Berun(text)
  puts "Berun: #{text}"
end

# [[
# Put your director's instructions and comments here.

# Alex "I will have you know that it's possible to talk like this!"
# Berun %{Sure, but like this is also "possible"...} 
# Alex 'I this is also "fine".'

# pa = path("script/main.lua")
# pa = nil

def ulpcss_load(sprite, layer, name) 
  full_name = "image/ulpcss/#{name}"
  sprite_loadulpcss sprite, layer, full_name

end

def torso_tint(sprite, r, g, b, a = 255) 
  sprite_tint_rgba sprite, SPRITELAYER_TORSO, r, g, b, a
end 
  

def hair_tint(sprite, r, g, b, a = 255) 
  sprite_tint_rgba sprite, SPRITELAYER_HAIR, r, g, b, a
end 

def start_load_sprites
  puts "Loading some things and sprites"
  thing_new  100, 1, 300, 400, 1, 32, 32
  thing_new  101, 0, 400, 400, 1, 32, 32
  sprite_new 100
  sprite_new 101
  ulpcss_load 100, 1, "body/female/light.png"
  ulpcss_load 101, 1, "body/female/dark.png"
  ulpcss_load 100, SPRITELAYER_TORSO, "torso/dress_female/underdress.png"
  ulpcss_load 101, SPRITELAYER_TORSO, "torso/dress_w_sash_female.png"
  ulpcss_load 100, SPRITELAYER_HAIR , "hair/female/bangslong.png"
  ulpcss_load 101, SPRITELAYER_HAIR , "hair/female/bangsshort.png"
  hair_tint   101, 255, 255, 0
  hair_tint   100,   0, 255, 0
  torso_tint  100, 255, 64 , 64
  torso_tint  101,  64,  64, 255
  sprite_tint_rgba  1,  2,  64,  64, 255, 255
  
  
            
            

  thing_sprite_ 100, 100
  thing_sprite_ 101, 101
  thing_pose_ 100, SPRITE_STAND
  thing_pose_ 101, SPRITE_STAND
  thing_direction_ 100, SPRITE_SOUTH
  thing_direction_ 101, SPRITE_SOUTH
 
  
  
  
end

ZIGZAG_LEAF = 10001

def start_load_stuff
  res = load_bitmap(ZIGZAG_LEAF, "image/ui/icon/gin/zigzag-leaf_64.png")
  puts "start_load_stuff: #{res}"
  res = store_kind(ZIGZAG_LEAF)
  puts "type: #{res}"
end

def on_start(s)
  puts "on_start #{s}"
  # start_load_sprites
  start_load_stuff
  return 1234567
end

def on_bump(t1, t2, kind = nil)
  puts "BUMP! #{t1} #{t2} #{kind}"
  return true
end

def ok!
  log "OK, it works!"
end

$count = 0

def on_update(dt)  
  $count += 1 
  # log "dt: #{dt}"
  # log "Update nr #{$count}!"
  return nil
end



# log "this is for the console."
# 
# test()
# log "Hi from lua! Björn"
# path = PathForData "script/main.lua"
# if path then
#   log(path:to_s())
# else
#   log("path not found")
# end
# path = nil
# collectgarbage()
# 
# mainmenu_image = Image("image/background/eruta_mainmenu.png")
# if mainmenu_image then
#   local w = mainmenu_image:w()
#   local h = mainmenu_image:h()
#   log("Image dimensions:" .. w .. "," .. h)
# end
# 
# font_tuffy = Font("Tuffy.ttf", 16, 0)
# if font_tuffy then
#   local w = font_tuffy:w("M")
#   local h = font_tuffy:h()
#   log("Font:" .. w .. "," .. h)
# end
# 
# 
# 
# function on_start(s)
#   log("Start! " .. s)
# end
# 
# le = nil
# 
# function on_event(eve)
#   le = eve
#   local type = eve:type()
#   log("Event type: " .. type)
#   eve = nil
#   collectgarbage()
# --  io.write("Draw!\n")  
# end
# 
# 
# function on_draw(s)
#   -- log("draw! " .. s)
#   -- log("Draw!")
# --  io.write("Draw!\n")  
# end
# 
# 
# function on_update()
# --  io.write("Update!\n")
# end
# 
# 
# 
# 
# 
# 
# 
# 
