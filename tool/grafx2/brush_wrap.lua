-- Grafx2 brush script to wrap a brush's pixels around.
-- Beoran (beoran@rubyforge.org) 2011.

dofile "beoran_lib.lua"

function brush_wrap(dx, dy)
  local w, h   = getbrushsize()
  local image  = Image.from_brush_wrap(dx, dy);
  image:to_brush()
end

function brush_wrap_ui()
  local bw, bh = getbrushsize()
  if (bw < 2) or (bh < 2) then
    statusmessage("Brush too small!")
    return 
  end
  local ok, dx, dy = inputbox("Wrap Brush By",
  "X", 8, 0, bw, 0,
  "Y", 8, 0, bh, 0
  );
  brush_wrap(dx, dy)
end
  
brush_wrap_ui()
 
 
 