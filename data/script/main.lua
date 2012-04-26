

test()
io.write("Hi from lua!\n")


function on_start(s)
  io.write("Start!\n", s, "\n")
end

function on_drw()
  blargs()
  io.write("Draw!\n")  
end


function on_update()
  io.write("Update!\n")
end








