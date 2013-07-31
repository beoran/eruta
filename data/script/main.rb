# Main eruta script

# Load keycodes
script "keycode.rb"
# Load thing OO wrapper
script "thing.rb"
script "sprite.rb"
script "graph.rb"




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
  $thing_100  = Thing.make(1, 300, 400, 1, 32, 32, 100)
  $sprite_100 = Sprite.make(100)
  $sprite_100.load_ulpcss(1, "body/female/light.png")
  $sprite_100.load_ulpcss(SPRITELAYER_TORSO, "torso/dress_female/underdress.png")
  $sprite_100.load_ulpcss(SPRITELAYER_HAIR, "hair/female/bangslong.png")
  $sprite_100.tint_hair(0, 255, 0)
  $sprite_100.tint_torso(255, 64, 64)
  $thing_100.sprite    = $sprite_100
  $thing_100.direction = SPRITE_SOUTH
  $thing_100.pose      = SPRITE_STAND
  
# ulpcss_load $sprite_100.id, 1, "body/female/dark.png"   
#   thing_new  101, 0, 400, 400, 1, 32, 32
#   sprite_new 101
#   ulpcss_load 101, 1, "body/female/dark.png"
#   ulpcss_load 101, SPRITELAYER_TORSO, "torso/dress_w_sash_female.png"
#   ulpcss_load 101, SPRITELAYER_HAIR , "hair/female/bangsshort.png"
#   hair_tint   101, 255, 255, 0
#   torso_tint  101,  128,  128, 255
#   sprite_tint_rgba  1,  2,  64,  64, 255, 255
#   
# 
#   thing_sprite_ 101, 101
#   thing_pose_ 101, SPRITE_STAND
#   thing_direction_ 101, SPRITE_SOUTH
 
  puts "Things and sprites loaded."
  
end

ZIGZAG_LEAF = 10001

def start_load_stuff
  res = load_bitmap(ZIGZAG_LEAF, "image/ui/icon/gin/zigzag-leaf_64.png")
  puts "start_load_stuff: #{res}"
  res = store_kind(ZIGZAG_LEAF)
  puts "type: #{res}"
  res = store_kind(ZIGZAG_LEAF + 1)
  puts "type: #{res}"
  h   = Eruta::Store.bitmap_height(ZIGZAG_LEAF)
  w   = Eruta::Store.bitmap_width(ZIGZAG_LEAF)
  puts "h: #{h} w: #{w}"
  
end


def start_setup_ui
  box = Eruta::Graph.make_box(51, 570, 10, 60, 240, 4, 4, -1)
  Eruta::Graph.background_color_(box, 20, 128, 20, 190)
  Eruta::Graph.border_color_(box, 255, 255, 255, 255)
  Eruta::Graph.border_thickness_(box, 1)
  res = load_bitmap(ZIGZAG_LEAF, "image/ui/icon/gin/zigzag-leaf_64.png")
  puts "start_load_stuff: #{res}"
  img = Eruta::Graph.make_image(52, 123, 245, ZIGZAG_LEAF, -1)
  Eruta::Graph.color_(img, 20, 200, 20, 190)
  Eruta::Graph.size_(img, 32, 32)
  Eruta::Graph.angle_(img, 1.23)
  puts "img: #{img}"
end

def on_start(*args)
  puts "on_start #{args}"
  start_load_sprites
  start_load_stuff
  start_setup_ui
  return :ok
end


def on_reload(*args)
  puts "on_reload #{args}"
  puts "load thing ok: #{Thing.methods - Object.methods}"
  puts "load thing ok: #{Eruta::Thing.methods - Object.methods}"
  $t1 = Thing.make(1, 700, 700, 1, 32, 32) 
  return :ok
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
  # puts "Update nr #{$count}!"
  if ($count % 71) == 0  
    thing_direction_ 101, 1 << (8 + $count % 4)
  end
  return nil
end

def actor_switch(new_id)
  actor_id = actor_index
  # Ensure the told thing stops walking. 
  Thing.v_(actor_id, 0, 0)
  actor_index_(new_id)
  camera_track(new_id)
  return actor_id
end


# Handle key down 
def on_key_down(time, key)
  actor_id = actor_index
  vx, vy = Eruta::Thing.v(actor_id)
  case key
  when KEY_A
    actor_switch(1)
  when KEY_B
    actor_switch(100)
  when KEY_H
    Eruta::Graph.visible_(52, 0)
  when KEY_S
    Eruta::Graph.visible_(52, 1)
  when KEY_UP
    vy -= 50.0
  when KEY_DOWN 
    vy += 50.0
  when KEY_LEFT
    vx -= 50.0
  when KEY_RIGHT
    vx += 50.0
  else
  end 
  Eruta::Thing.v_(actor_id, vx, vy)
  return nil
end

# Handle key up
def on_key_up(time, key)
  actor_id = actor_index
  vx, vy = Eruta::Thing.v(actor_id)
  case key
  when KEY_UP
    vy = 0.0
  when KEY_DOWN 
    vy = 0.0
  when KEY_LEFT
    vx = 0.0
  when KEY_RIGHT
    vx = 0.0
  else
  end 
  Eruta::Thing.v_(actor_id, vx, vy) 
  return nil
end


# Called when an input event occurs 
def on_poll(*args)
  type = args.shift
  meth = "on_#{type}".to_sym
  if self.respond_to?(meth) 
    self.send(meth, *args)
  else
    # ignore 
    # puts "input #{type} #{meth} #{args}"
  end  
end


# log "this is for the console."
# 
# test()
# log "Hi from Ruby! Björn"
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
