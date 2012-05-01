
function Alex(text)
  io.write("Alex: ", text, "\n" )
end

function Berun(text)
  io.write("Berun: ", text, "\n")
end


--[[
Put your director's instructions and comments here.

Alex "I will have you know that it's possible to talk like this!"
Berun Sure, but like this is also "possible"... 
Alex 'I this is also "fine".'

]]




log "this is for the console."

test()
log "Hi from lua! Björn"
path = PathForData "script/main.lua"
if path then
  log(path:to_s())
else
  log("path not found")
end
path = nil
collectgarbage()

mainmenu_image = Image("image/background/eruta_mainmenu.png")
if mainmenu_image then
  local w = mainmenu_image:w()
  local h = mainmenu_image:h()
  log("Image dimensions:" .. w .. "," .. h)
end

font_tuffy = Font("Tuffy.ttf", 16, 0)
if font_tuffy then
  local w = font_tuffy:w("M")
  local h = font_tuffy:h()
  log("Font:" .. w .. "," .. h)
end



function on_start(s)
  log("Start! " .. s)
end

le = nil

function on_event(eve)
  le = eve
  local type = eve:type()
  log("Event type: " .. type)
  eve = nil
  collectgarbage()
--  io.write("Draw!\n")  
end


function on_draw(s)
  -- log("draw! " .. s)
  -- log("Draw!")
--  io.write("Draw!\n")  
end


function on_update()
--  io.write("Update!\n")
end








