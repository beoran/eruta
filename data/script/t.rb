
@t1 ||= actor_index
@t2 ||= nil
@s1 ||= sprite_getornew(1)
@s2 ||= sprite_getornew(2)

def make_pc(sid) 
  res  = thing_new(1, 200, 70, 1, 32, 32)
  spri = sprite_getornew(sid)
  sprite_loadulpcss spri, 1, "image/ulpcss/body/female/dark.png"
  thing_sprite_  res, spri
  thing_pose_ res, 2
  thing_direction_ res, 1
  return res, spri
end
  

if !@t2
  @t2 = thing_new(1, 200, 70, 1, 32, 32)
  sprite_loadulpcss @s2, 1, "image/ulpcss/body/female/dark.png"
  thing_sprite_  @t2, @s2
  thing_pose_ @t2, 1
  thing_direction_ @t2, 1
end

def actor_(t) 
  p t
  actor_index_ t
  camera_track t
end

@t3, @s3 = make_pc(3)

actor_ @t3












 

