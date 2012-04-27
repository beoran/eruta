

test()
io.write "Hi from lua!\n"
path = PathForData "script/main.lua"
if path then
  io.write(path:to_s(), "\n")
else
  io.write("path not found\n")
end
path = nil
collectgarbage()

mainmenu_image = Image("image/background/eruta_mainmenu.png")
if mainmenu_image then
  local w = mainmenu_image:w()
  local h = mainmenu_image:h()
  io.write("Image dimensions:", w, "," , h, "\n")
end

font_tuffy = Font("Tuffy.ttf", 16, 0)
if font_tuffy then
  local w = font_tuffy:w("M")
  local h = font_tuffy:h()
  io.write("Font:", w, "," , h, "\n")
end



function on_start(s)
  io.write("Start!\n", s, "\n")
end

function on_draw()
--  io.write("Draw!\n")  
end


function on_update()
--  io.write("Update!\n")
end








