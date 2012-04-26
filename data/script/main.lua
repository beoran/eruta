

test()
io.write "Hi from lua!\n"
path = PathForData "script/main.lua"
if path then
  io.write(path:to_s(), "\n")
else
  io.write("path not found\n")
end
path = nil
collectgarbage();

function on_start(s)
  io.write("Start!\n", s, "\n")
end

function on_draw()
--  io.write("Draw!\n")  
end


function on_update()
--  io.write("Update!\n")
end








