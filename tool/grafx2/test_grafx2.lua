-- Grafx2 test script
-- Beoran (beoran@rubyforge.org) 2011.
 
-- messagebox("Hello Grafx2!")
statusmessage("Hello Grafx2!") 
 
function getmouseclick(prompt)
  local moved, key, mx, my, mb = waitinput(0)
  msg = string.format("%d %d %d %d", key, mx, my, mb)
  while (key == 0) and (mb == 0) do
     msg = string.format("%s X:%d Y:%d", prompt, mx, my)
     statusmessage(msg)
     moved, key, mx, my, mb = waitinput(0)
  end
  if mb == 0 then
    return nil, nil, nil
  end
  -- Unfortunately this gets the /screen/ coordinates
  -- I'd hope to get the active pixel coordinates.
  return mb, mx, my
end

-- range(a) returns an iterator from 1 to a (step = 1)
-- range(a, b) returns an iterator from a to b (step = 1)
-- range(a, b, step) returns an iterator from a to b, counting by step.
function range(a, b, step)
  if not b then
    b = a
    a = 1
  end
  step = step or 1
  local f =
    step > 0 and
      function(_, lastvalue)
        local nextvalue = lastvalue + step
        if nextvalue <= b then return nextvalue end
      end or
    step < 0 and
      function(_, lastvalue)
        local nextvalue = lastvalue + step
        if nextvalue >= b then return nextvalue end
      end or
      function(_, lastvalue) return lastvalue end
  return f, nil, a - step
end


-- http://code.google.com/p/grafx2/wiki/BrushFactory

function brush_wrap(dx, dy)
  local w, h   = getbrushsize()
  local xx, yy = 0, 0
  for yy in range(h) do
    for xx in range(w) do
      local sx, sy = (sx - dx) , (sy - dy)
      if sx < 0 then; sx = sx + w; end
      if sy < 0 then; sy = sy + h; end
      pix = getbrushpixel(sx, sy)
    end
  end
end

-- getmouseclick("Corner?")
ok, a, b ,c, d = inputbox("Your choice",
  "A",    1, 0, 1,-1,
  "B",    0, 0, 1,-1,
  "C",    0, 0, 1,-1,
  "D",    0, 0, 1,-1,
  "X-Flip", 0, 0, 1,0,
  "Y-Flip", 0, 0, 1,0,
  "Width", 0, 1,100,0,
  "Height",0, 1,100,0

);
 
 