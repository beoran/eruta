-- Grafx2 helper library
-- Beoran (beoran@rubyforge.org) 2011.
  
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

function table.val_to_str ( v )
  if "string" == type( v ) then
    v = string.gsub( v, "\n", "\\n" )
    if string.match( string.gsub(v,"[^'\"]",""), '^"+$' ) then
      return "'" .. v .. "'"
    end
    return '"' .. string.gsub(v,'"', '\\"' ) .. '"'
  else
    return "table" == type( v ) and table.tostring( v ) or
      tostring( v )
  end
end

function table.key_to_str ( k )
  if "string" == type( k ) and string.match( k, "^[_%a][_%a%d]*$" ) then
    return k
  else
    return "[" .. table.val_to_str( k ) .. "]"
  end
end

function table.tostring( tbl )
  local result, done = {}, {}
  for k, v in ipairs( tbl ) do
    table.insert( result, table.val_to_str( v ) )
    done[ k ] = true
  end
  for k, v in pairs( tbl ) do
    if not done[ k ] then
      table.insert( result,
        table.key_to_str( k ) .. "=" .. table.val_to_str( v ) )
    end
  end
  return "{" .. table.concat( result, "," ) .. "}"
end

Class = {}

-- Ruby-ish classes. 

function Class:new(...)
  local instance  = {}
  local metatable = { __index = self }
  setmetatable(instance, metatable)  
  if instance.initialize then
    instance:initialize(...)
  end
  return instance
end

function Class:inherit()
  local klass     = {} 
  local metatable = { _index = Class }
  setmetatable(klass, metatable) 
  return klass  
end


Image = Class

function Image:initialize(w, h)
  self._h    = h
  self._w    = w
  self._data = {}
  for yy = 1, self:h() do
    self._data[yy] = {}
  end
  return self:fill(2)  
end

function Image:fill(color)
  for yy = 1, self:h() do
    for xx = 1, self:w() do
      self:set(xx, yy, color)
    end
  end
  return self
end

function Image:w()
  return self._w
end  

function Image:h()
  return self._h
end  

function Image:outside(x, y) 
 if (x < 0) or (x >= self:w()) then return true end
 if (y < 0) or (y >= self:h()) then return true end
 return false
end

function Image:get(x, y)
  if self:outside(x, y) then return -1 end
  return self._data[y+1][x+1] 
end

function Image:set(x, y, color)
  if self:outside(x, y) then return -1 end  
  self._data[y+1][x+1] = color
  return color
end

function Image:to_brush()
  for yy = 0, self._h do
    for xx = 0, self._w do
      local color = self:get(xx, yy)
      putbrushpixel(xx, yy, color) 
    end
  end
end

function Image.from_brush()
  local w, h    = getbrushsize()
  local image   = Image:new(w, h)
  for yy = 0, image:h() do
    for xx = 0, image:w() do
      local color = getbrushpixel(xx, yy)
      image:set(xx, yy, color)
    end
  end
  local xx, yy = 1, 1
  return image
end

function Image.from_brush_wrap(dx, dy)
  local w, h    = getbrushsize()
  local image   = Image:new(w, h)
  for yy = 0, image:h() do
    for xx = 0, image:w() do
      local sx, sy = (xx - dx) , (yy - dy)
      if sx < 0 then sx = sx + image:w() end
      if sy < 0 then sy = sy + image:h() end
      local color = getbrushpixel(sx, sy)
      image:set(xx, yy, color)
    end
  end
  local xx, yy = 1, 1
  return image
end


 
 