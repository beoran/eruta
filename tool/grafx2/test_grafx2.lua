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
 
 